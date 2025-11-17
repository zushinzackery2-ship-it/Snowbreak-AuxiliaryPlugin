#include "esp_renderer.h"
#include "../skeleton/skeleton_renderer.h"
#include "../data/actor_data.h"
#include "../config/esp_config.h"
#include "../aimbot/aimbot_core.h"
#include <string>
#include "../../CppSDK/SDK/Basic.hpp"
#include "../../CppSDK/SDK/CoreUObject_classes.hpp"
#include "../../CppSDK/SDK/Engine_classes.hpp"
#include "../../CppSDK/SDK/Game_classes.hpp"

using namespace SDK;

void ESPRenderer::Initialize() {
    // Reserved for future initialization
}

void ESPRenderer::Render() {
    if (ImGui::GetCurrentContext() == nullptr) return;
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    float screenW = io.DisplaySize.x;
    float screenH = io.DisplaySize.y;
    
    if (!ESPConfig::IsEnabled()) {
        return;
    }
    
    RenderActorESP();
}

void ESPRenderer::RenderActorESP() {
    if (ImGui::GetCurrentContext() == nullptr) return;
    auto actors = ActorDataManager::GetActors();
    if (actors.empty()) return;

    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    
    float screenW = io.DisplaySize.x;
    float screenH = io.DisplaySize.y;

    SDK::UWorld* world = SDK::UWorld::GetWorld();
    SDK::APlayerController* pc = world ? SDK::UGameplayStatics::GetPlayerController(world, 0) : nullptr;
    SDK::APlayerCameraManager* camMgr = pc ? pc->PlayerCameraManager : nullptr;
    SDK::FVector cameraLoc = camMgr ? camMgr->CameraCachePrivate.POV.Location : SDK::FVector(0,0,0);

    for (const auto& actor : actors) {
        if (!actor.isValid) continue;
        
        if (actor.screenPos.z < 0) continue;
        
        if (actor.isDead || actor.hpCurrent <= 0.0f) continue;
        
        float x = actor.screenPos.x;
        float y = actor.screenPos.y;
        if (x < 0 || x > screenW || y < 0 || y > screenH) continue;

        bool los = true;
        if (pc && actor.actorPtr) {
            SDK::AActor* aptr = reinterpret_cast<SDK::AActor*>(actor.actorPtr);
            los = pc->LineOfSightTo(aptr, cameraLoc, true);
        }
        ImU32 cornerCol = los ? IM_COL32(255, 0, 0, 220) : IM_COL32(255, 255, 0, 220);
        ImU32 textCol = los ? IM_COL32(255, 255, 255, 220) : IM_COL32(255, 255, 0, 220);
        
        // Only skeleton and fallback rendering with color
        SkeletonRenderer::Render(actor, drawList, screenW, screenH, cornerCol, textCol);
    }
}

void ESPRenderer::DrawActorInfo(const ActorInfo& actor, ImDrawList* drawList, float screenW, float screenH) {}
