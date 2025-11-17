#include "combat_config.h"
#include "esp_config.h"
#include "combat/combat_enhancer.h"
#include <algorithm>
#include <string>
#include <cstring>
#include "../../CppSDK/SDK/Basic.hpp"
#include "../../CppSDK/SDK/Engine_classes.hpp"
#include "../../CppSDK/SDK/Game_classes.hpp"
#include "../../CppSDK/SDK/GameTask_structs.hpp"
#include "../../CppSDK/SDK/GameTask_classes.hpp"
using namespace SDK;
static void SafeChangeDynAttr(UGameAbilityComponent* ability, const UAbilityAttribute* attr, float delta)
{
    __try { if (ability && attr) ability->ChangeDynamicAttributeValue(attr, delta, false); }
    __except (EXCEPTION_EXECUTE_HANDLER) { }
}

static void SafeWriteCurAttr(FAbilityAttributeItem* item, float v)
{
    __try { if (item) item->CurrentAttributeValue = v; }
    __except (EXCEPTION_EXECUTE_HANDLER) { }
}

bool CombatConfig::s_damageEnabled = false;
float CombatConfig::s_damageMultiplier = 1.0f;
bool CombatConfig::s_defenseEnabled = false;
float CombatConfig::s_defenseMultiplier = 1.0f;
bool CombatConfig::s_speedEnabled = false;
float CombatConfig::s_speedMultiplier = 1.0f;
bool CombatConfig::s_fastRecoverEnabled = false;
int  CombatConfig::s_f8ActionMode = 0;

void CombatConfig::Initialize() {
    s_damageEnabled = false;
    s_damageMultiplier = 1.0f;
    s_defenseEnabled = false;
    s_defenseMultiplier = 1.0f;
    s_speedEnabled = false;
    s_speedMultiplier = 1.0f;
    s_fastRecoverEnabled = false;
    s_f8ActionMode = 0;
}

static bool SetAttrByName(UGameAbilityComponent* ability, const char* name, float value) {
    if (!ability) return false;
    const FAbilityAttributeArray& attrs = ability->AbilityAttributes;
    bool ok = false;

    const UC::TArray<FAbilityAttributeItem>& rep = attrs.RepAttributes;
    if (rep.IsValid()) {
        int32 n = rep.Num();
        for (int32 i = 0; i < n; ++i) {
            if (!rep.IsValidIndex(i)) continue;
            const FAbilityAttributeItem& item = rep[i];
            if (!item.AbilityAttribute) continue;
            const UObject* attrObj = reinterpret_cast<const UObject*>(item.AbilityAttribute);
            std::string nm = attrObj->GetName();
            if (_stricmp(nm.c_str(), name) == 0) {
                FAbilityAttributeItem* mut = const_cast<FAbilityAttributeItem*>(&item);
                mut->CurrentAttributeValue = value;
                ok = true;
                break;
            }
        }
    }
    if (!ok) {
        const UC::TArray<FAbilityAttributeItem>& notRep = attrs.NotRepAttributes;
        if (notRep.IsValid()) {
            int32 n2 = notRep.Num();
            for (int32 i = 0; i < n2; ++i) {
                if (!notRep.IsValidIndex(i)) continue;
                const FAbilityAttributeItem& item = notRep[i];
                if (!item.AbilityAttribute) continue;
                const UObject* attrObj = reinterpret_cast<const UObject*>(item.AbilityAttribute);
                std::string nm = attrObj->GetName();
                if (_stricmp(nm.c_str(), name) == 0) {
                    FAbilityAttributeItem* mut = const_cast<FAbilityAttributeItem*>(&item);
                    mut->CurrentAttributeValue = value;
                    ok = true;
                    break;
                }
            }
        }
    }
    return ok;
}

static bool GetAttrValueByName(UGameAbilityComponent* ability, const char* name, float& outValue) {
    if (!ability) return false;
    const FAbilityAttributeArray& attrs = ability->AbilityAttributes;
    const UC::TArray<FAbilityAttributeItem>& rep = attrs.RepAttributes;
    if (rep.IsValid()) {
        int32 n = rep.Num();
        for (int32 i = 0; i < n; ++i) {
            if (!rep.IsValidIndex(i)) continue;
            const FAbilityAttributeItem& item = rep[i];
            if (!item.AbilityAttribute) continue;
            const UObject* attrObj = reinterpret_cast<const UObject*>(item.AbilityAttribute);
            std::string nm = attrObj->GetName();
            if (_stricmp(nm.c_str(), name) == 0) { outValue = item.CurrentAttributeValue; return true; }
        }
    }
    const UC::TArray<FAbilityAttributeItem>& notRep = attrs.NotRepAttributes;
    if (notRep.IsValid()) {
        int32 n2 = notRep.Num();
        for (int32 i = 0; i < n2; ++i) {
            if (!notRep.IsValidIndex(i)) continue;
            const FAbilityAttributeItem& item = notRep[i];
            if (!item.AbilityAttribute) continue;
            const UObject* attrObj = reinterpret_cast<const UObject*>(item.AbilityAttribute);
            std::string nm = attrObj->GetName();
            if (_stricmp(nm.c_str(), name) == 0) { outValue = item.CurrentAttributeValue; return true; }
        }
    }
    return false;
}

void CombatConfig::ApplyToBPGirl(SDK::AActor* actor) {
    CombatEnhancer::UpdateForActor(actor);
}

void CombatConfig::TriggerDirectWin() {
    CombatEnhancer::Reset();
    UWorld* world = UWorld::GetWorld();
    if (!world) return;
    ULevel* level = world->PersistentLevel;
    if (!level) return;
    TArray<AActor*>& arr = level->Actors;
    for (int32 i=0;i<arr.Num();++i) {
        AActor* actor = arr[i];
        if (!actor) continue;
        if (!actor->IsA(AGameTaskActor::StaticClass())) continue;
        AGameTaskActor* task = static_cast<AGameTaskActor*>(actor);
        __try {
            task->LevelFinishBroadCast(ELevelFinishResult::Success, ELevelFailedReason::Success, true);
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            // swallow
        }
    }
}

void CombatConfig::TriggerF8Action() {
    if (s_f8ActionMode == 0) {
        TriggerDirectWin();
    } else {
        TriggerCoopPoint();
    }
}

void CombatConfig::TriggerCoopPoint() {
    UWorld* world = UWorld::GetWorld();
    if (!world) return;
    APlayerController* apc = UGameplayStatics::GetPlayerController(world, 0);
    if (!apc || !apc->IsA(AGamePlayerController::StaticClass())) return;
    AGamePlayerController* gpc = static_cast<AGamePlayerController*>(apc);
    __try {
        gpc->Server_UpdateMultiLevelPoint(1000);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
    }
}
