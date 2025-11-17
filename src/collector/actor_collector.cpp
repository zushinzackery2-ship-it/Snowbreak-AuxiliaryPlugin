#include "actor_collector.h"
#include "../data/actor_data.h"
#include "../skeleton/skeleton_data.h"
#include "../config/esp_config.h"
#include "../aimbot/aimbot_config.h"
#include "../config/combat_config.h"
#include "../config/combat/combat_enhancer.h"
#include "../../CppSDK/SDK/Basic.hpp"
#include "../../CppSDK/SDK/CoreUObject_classes.hpp"
#include "../../CppSDK/SDK/Engine_classes.hpp"
#include "../../CppSDK/SDK/Game_classes.hpp"
#include <vector>
#include <cmath>
#include <cstring>
#include <Windows.h>

using namespace SDK;

static float SafeReadFloat(void* address)
{
    __try { return *(float*)address; }
    __except (EXCEPTION_EXECUTE_HANDLER) { return -1.0f; }
}

static void* SafeReadPointer(void* address)
{
    __try { return *(void**)address; }
    __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}

static bool ReadHpByOffsets(AActor* actor, float& outHp)
{
    if (!actor) return false;
    uintptr_t base = reinterpret_cast<uintptr_t>(actor);
    void* umgData = SafeReadPointer((void*)(base + 0x04F8));
    if (!umgData) return false;
    void* hpComp = SafeReadPointer((void*)((uintptr_t)umgData + 0x00F8));
    if (!hpComp) return false;
    void* hpWidget = SafeReadPointer((void*)((uintptr_t)hpComp + 0x05F0));
    if (!hpWidget) return false;
    float cur = SafeReadFloat((void*)((uintptr_t)hpWidget + 0x03A8));
    if (cur < 0.f) return false;
    outHp = cur;
    return true;
}

std::atomic<bool> ActorCollector::s_running{false};
HANDLE ActorCollector::s_threadHandle = nullptr;
static SDK::ULevel* g_prevLevel = nullptr;

void ActorCollector::Start() {
    if (s_running.load()) return;
    
    s_running.store(true);
    s_threadHandle = CreateThread(nullptr, 0, CollectorThread, nullptr, 0, nullptr);
}

void ActorCollector::Stop() {
    s_running.store(false);
    if (s_threadHandle) {
        WaitForSingleObject(s_threadHandle, 5000);
        CloseHandle(s_threadHandle);
        s_threadHandle = nullptr;
    }
}

DWORD WINAPI ActorCollector::CollectorThread(LPVOID param) {
    while (s_running.load()) {
        UWorld* worldCheck = UWorld::GetWorld();
        ULevel* levelCheck = worldCheck ? worldCheck->PersistentLevel : nullptr;
        if (levelCheck != g_prevLevel) {
            g_prevLevel = levelCheck;
            CombatEnhancer::Reset();
        }
        CollectActorData();
        Sleep(5);
    }
    return 0;
}

void ActorCollector::CollectActorData() {
    std::vector<ActorInfo> actors;
    
    UWorld* world = UWorld::GetWorld();
    if (!world) {
        ActorDataManager::UpdateActors(actors);
        return;
    }
    
    APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
    ULevel* level = world->PersistentLevel;
    
    if (!level || !playerController) {
        ActorDataManager::UpdateActors(actors);
        return;
    }
    
    TArray<AActor*>& actorArray = level->Actors;
    
    for (int32 i = 0; i < actorArray.Num(); ++i) {
        AActor* actor = actorArray[i];
        if (!actor) continue;
        
        std::string className;
        if (actor->Class) {
            className = actor->Class->GetName();
        }
        
        if (className.empty()) {
            continue;
        }
        
        // Only collect Blueprint objects (class names ending with _C)
        if (className.find("_C") == std::string::npos) {
            continue;
        }
        
        ActorInfo info;
        info.actorPtr = actor;
        info.isValid = true;
        info.className = className;
        info.objectName = actor->GetName();
        info.arrayIndex = i;
        
        FVector location = actor->K2_GetActorLocation();
        info.worldPos.x = location.X;
        info.worldPos.y = location.Y;
        info.worldPos.z = location.Z;
        
        float distCm = 0.0f;
        ACharacter* playerChar = UGameplayStatics::GetPlayerCharacter(world, 0);
        if (playerChar) {
            FVector playerLoc = playerChar->K2_GetActorLocation();
            float dx = location.X - playerLoc.X;
            float dy = location.Y - playerLoc.Y;
            float dz = location.Z - playerLoc.Z;
            distCm = sqrtf(dx*dx + dy*dy + dz*dz);
        } else {
            distCm = sqrtf(location.X*location.X + location.Y*location.Y + location.Z*location.Z);
        }
        info.distanceCm = distCm;
        
        FVector2D screenPos2D;
        if (UGameplayStatics::ProjectWorldToScreen(playerController, location, &screenPos2D, true)) {
            info.screenPos.x = screenPos2D.X;
            info.screenPos.y = screenPos2D.Y;
            info.screenPos.z = 1.0f;
        } else {
            info.screenPos.x = 0;
            info.screenPos.y = 0;
            info.screenPos.z = -1;
        }
        
        // Apply combat boosts to BP_Girl classes
        CombatConfig::ApplyToBPGirl(actor);

        // Collect skeleton data for ESP or when bone aimbot mode is enabled
        if (ESPConfig::ShowSkeleton() || AimbotConfig::BoneMode()) {
            SkeletonCollector::CollectBones(info, playerController);
        }
        
        if (actor->IsA(AGameCharacter::StaticClass())) {
            // Preferred: raw pointer path by offsets
            float hpVal = -1.0f;
            bool gotHp = ReadHpByOffsets(actor, hpVal);
            if (gotHp) {
                info.hpCurrent = hpVal;
            } else {
                // Fallback: scan ability attributes in memory
                AGameCharacter* gc = static_cast<AGameCharacter*>(actor);
                UAbilityComponent* abilRaw = gc->Ability;
                UGameAbilityComponent* ability = static_cast<UGameAbilityComponent*>(abilRaw);
                if (ability) {
                    const FAbilityAttributeArray& attrs = ability->AbilityAttributes;
                    auto scanArr = [&](const UC::TArray<FAbilityAttributeItem>& arr) {
                        if (!arr.IsValid()) return false;
                        int32 n = arr.Num();
                        for (int32 j = 0; j < n; ++j) {
                            if (!arr.IsValidIndex(j)) continue;
                            const FAbilityAttributeItem& item = arr[j];
                            if (!item.AbilityAttribute) continue;
                            const SDK::UObject* attrObj = reinterpret_cast<const SDK::UObject*>(item.AbilityAttribute);
                            std::string nm = attrObj->GetName();
                            if (_stricmp(nm.c_str(), "Health") == 0) {
                                info.hpCurrent = item.CurrentAttributeValue;
                                info.hpMax = item.MaxValue;
                                return true;
                            }
                        }
                        return false;
                    };
                    bool found = scanArr(attrs.RepAttributes);
                    if (!found) {
                        scanArr(attrs.NotRepAttributes);
                    }
                }
            }
            info.isDead = (info.hpCurrent <= 0.0f);
        }
        
        actors.push_back(info);
    }
    
    ActorDataManager::UpdateActors(actors);
}
