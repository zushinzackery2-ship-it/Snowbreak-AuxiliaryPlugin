#include "aimbot_core.h"
#include "aimbot_config.h"
#include "../data/actor_data.h"
#include "../../CppSDK/SDK/Basic.hpp"
#include "../../CppSDK/SDK/CoreUObject_classes.hpp"
#include "../../CppSDK/SDK/Engine_classes.hpp"
#include "../../CppSDK/SDK/Game_classes.hpp"
#include "../../imgui/imgui.h"
#include <cmath>
#include <algorithm>
#include <cfloat>
#include <cstdio>

using namespace SDK;

std::atomic<bool> AimbotCore::s_running{false};
HANDLE AimbotCore::s_threadHandle = nullptr;
AimTarget AimbotCore::s_currentTarget;
std::mutex AimbotCore::s_targetMutex;

static inline void GetScreenCenter(float& outX, float& outY) {
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGuiIO& io = ImGui::GetIO();
        outX = io.DisplaySize.x * 0.5f;
        outY = io.DisplaySize.y * 0.5f;
        return;
    }
    RECT rect{};
    HWND hwnd = GetForegroundWindow();
    if (hwnd && GetClientRect(hwnd, &rect)) {
        outX = (rect.right - rect.left) * 0.5f;
        outY = (rect.bottom - rect.top) * 0.5f;
    } else {
        outX = 960.0f;
        outY = 540.0f;
    }
}

void AimbotCore::Start() {
    if (s_running.load()) return;
    
    s_running.store(true);
    s_threadHandle = CreateThread(nullptr, 0, AimbotThread, nullptr, 0, nullptr);
}

void AimbotCore::Stop() {
    s_running.store(false);
    if (s_threadHandle) {
        WaitForSingleObject(s_threadHandle, 5000);
        CloseHandle(s_threadHandle);
        s_threadHandle = nullptr;
    }
}

DWORD WINAPI AimbotCore::AimbotThread(LPVOID param) {
    while (s_running.load()) {
        if (AimbotConfig::IsEnabled()) {
            ProcessAimbot();
        }
        Sleep(5);
    }
    return 0;
}

void AimbotCore::ProcessAimbot() {
    UWorld* world = UWorld::GetWorld();
    if (!world) return;
    
    APlayerController* pc = UGameplayStatics::GetPlayerController(world, 0);
    if (!pc) return;
    
    int aimKey = AimbotConfig::GetAimKey();
    bool aimingHeld = (GetAsyncKeyState(aimKey) & 0x8000) != 0;
    
    AimTarget target = FindBestTarget();
    {
        std::lock_guard<std::mutex> lock(s_targetMutex);
        s_currentTarget = target;
    }
    
    if (!target.isValid) {
        return;
    }
    if (!aimingHeld) {
        return;
    }
    
    APlayerCameraManager* camMgr = pc->PlayerCameraManager;
    if (!camMgr) return;
    
    FVector cameraLoc = camMgr->CameraCachePrivate.POV.Location;
    Vector3 camPos(cameraLoc.X, cameraLoc.Y, cameraLoc.Z);
    
    float screenCenterX = 0.0f, screenCenterY = 0.0f;
    GetScreenCenter(screenCenterX, screenCenterY);
    
    float dx = camPos.x - target.worldPos.x;
    float dy = camPos.y - target.worldPos.y;
    float dz = camPos.z - target.worldPos.z;
    
    Vector2D angles = CalculateAimAngles(camPos, target.worldPos);
    
    FRotator currentRot = pc->ControlRotation;
    float deltaYaw = angles.x - currentRot.Yaw;
    float deltaPitch = angles.y - currentRot.Pitch;
    
    while (deltaYaw > 180.0f) deltaYaw -= 360.0f;
    while (deltaYaw < -180.0f) deltaYaw += 360.0f;
    
    float smoothing = AimbotConfig::GetSmoothing();
    float finalYaw = 0.0f;
    float finalPitch = 0.0f;
    
    auto norm180 = [](float a) -> float { 
        while (a > 180.0f) a -= 360.0f; 
        while (a < -180.0f) a += 360.0f; 
        return a; 
    };
    float currYawNorm = norm180(pc->ControlRotation.Yaw);
    float currPitchNorm = norm180(pc->ControlRotation.Pitch);
    float calcYawNorm = norm180(angles.x);
    float calcPitchNorm = norm180(angles.y);
    float dyawNorm = norm180(calcYawNorm - currYawNorm);
    float dpitchNorm = norm180(calcPitchNorm - currPitchNorm);

    if (smoothing <= 1.0f) {
        finalYaw = calcYawNorm;
        finalPitch = calcPitchNorm;
    } else {
        finalYaw = currYawNorm + dyawNorm / smoothing;
        finalPitch = currPitchNorm + dpitchNorm / smoothing;
    }
    finalYaw = norm180(finalYaw);
    if (finalPitch > 89.0f) finalPitch = 89.0f;
    if (finalPitch < -89.0f) finalPitch = -89.0f;

    printf("\n[AIMBOT DEBUG]\n");
    printf("  Target: %s\n", target.name.c_str());
    printf("  ScreenResolution: (%.0f x %.0f)\n", screenCenterX * 2, screenCenterY * 2);
    printf("  ScreenCenter: (%.2f, %.2f)\n", screenCenterX, screenCenterY);
    printf("  TargetScreenPos: (%.2f, %.2f)\n", target.screenPos.x, target.screenPos.y);
    printf("  ScreenOffset: (%.2f, %.2f) - Target is %s and %s\n", 
           target.screenPos.x - screenCenterX, 
           target.screenPos.y - screenCenterY,
           target.screenPos.x > screenCenterX ? "RIGHT" : "LEFT",
           target.screenPos.y > screenCenterY ? "BELOW" : "ABOVE");
    printf("  CameraPos: (%.2f, %.2f, %.2f)\n", camPos.x, camPos.y, camPos.z);
    printf("  TargetPos: (%.2f, %.2f, %.2f)\n", target.worldPos.x, target.worldPos.y, target.worldPos.z);
    printf("  DeltaVector: (dx=%.2f, dy=%.2f, dz=%.2f)\n", dx, dy, dz);
    printf("  CurrentRot: (Yaw=%.2f, Pitch=%.2f) | Norm: (Yaw=%.2f, Pitch=%.2f)\n", pc->ControlRotation.Yaw, pc->ControlRotation.Pitch, currYawNorm, currPitchNorm);
    printf("  CalculatedAngles: (Yaw=%.2f, Pitch=%.2f) | Norm: (Yaw=%.2f, Pitch=%.2f)\n", angles.x, angles.y, calcYawNorm, calcPitchNorm);
    printf("  AngleDelta(raw): (Yaw=%.2f, Pitch=%.2f)\n", angles.x - pc->ControlRotation.Yaw, angles.y - pc->ControlRotation.Pitch);
    printf("  AngleDelta(norm): (Yaw=%.2f, Pitch=%.2f) - Need to turn %s and %s\n",
           dyawNorm,
           dpitchNorm,
           dyawNorm > 0 ? "RIGHT(+)" : (dyawNorm < 0 ? "LEFT(-)" : "NONE"),
           dpitchNorm > 0 ? "UP(+)" : (dpitchNorm < 0 ? "DOWN(-)" : "NONE"));
    printf("  Smoothing: %.2f\n", smoothing);
    printf("  WriteAngles: (Yaw=%.2f, Pitch=%.2f)\n", finalYaw, finalPitch);
    printf("--------------------------------------\n");

    pc->ControlRotation.Yaw = finalYaw;
    pc->ControlRotation.Pitch = finalPitch;
}

Vector2D AimbotCore::CalculateAimAngles(const Vector3& cameraPos, const Vector3& targetPos) {
    float dx = cameraPos.x - targetPos.x;
    float dy = cameraPos.y - targetPos.y;
    float dz = cameraPos.z - targetPos.z;
    
    Vector2D result;
    result.x = 0.0f;
    result.y = 0.0f;
    
    const double PI = 3.14159265358979323846;
    
    if (dx > 0.0 && dy > 0.0)
        result.x = (float)(atan(dy / dx) / PI * 180.0 - 180.0);
    else if (dx <= 0.0 && dy > 0.0)
        result.x = (float)(atan(dy / dx) / PI * 180.0);
    else if (dx <= 0.0 && dy < 0.0)
        result.x = (float)(atan(dy / dx) / PI * 180.0);
    else if (dx >= 0.0 && dy < 0.0)
        result.x = (float)(atan(dy / dx) / PI * 180.0 + 180.0);
    
    result.y = (float)(-(atan(dz / sqrt(dx * dx + dy * dy)) / PI * 180.0));
    
    return result;
}

AimTarget AimbotCore::FindBestTarget() {
    AimTarget bestTarget;
    bestTarget.isValid = false;
    bestTarget.distanceToCenter = 99999.0f;
    
    UWorld* world = UWorld::GetWorld();
    if (!world) return bestTarget;
    
    APlayerController* pc = UGameplayStatics::GetPlayerController(world, 0);
    if (!pc) return bestTarget;
    
    APlayerCameraManager* camMgr = pc->PlayerCameraManager;
    if (!camMgr) return bestTarget;
    
    FVector cameraLoc = camMgr->CameraCachePrivate.POV.Location;
    
    float screenCenterX = 0.0f, screenCenterY = 0.0f;
    GetScreenCenter(screenCenterX, screenCenterY);
    
    float fovRadius = AimbotConfig::GetFovRadius();
    bool visibleOnly = AimbotConfig::VisibleOnly();
    bool useCrosshair = AimbotConfig::CrosshairMode();
    bool boneMode = AimbotConfig::BoneMode();
    
    auto actors = ActorDataManager::GetActors();
    
    float bestBoneScore = FLT_MAX;
    Vector3 bestBonePos;
    std::string bestBoneName;
    float bestActorScore = FLT_MAX;
    Vector3 bestActorPos;
    std::string bestActorName;
    Vector2D bestBoneScreen = {0.f, 0.f};
    Vector2D bestActorScreen = {0.f, 0.f};
    
    // Debug throttle: print every N calls to avoid spamming
    static int dbgTick = 0;
    bool canPrint = ((++dbgTick % 30) == 0);

    int scannedCount = 0;
    int invalidTypeCount = 0;
    int nullPtrCount = 0;
    int losFailCount = 0;
    int projectFailCount = 0;
    int fovRejectCount = 0;
    
    for (const auto& actor : actors) {
        scannedCount++;
        if (!IsValidTarget(actor)) { invalidTypeCount++; continue; }
        
        AActor* actorPtr = static_cast<AActor*>(actor.actorPtr);
        if (!actorPtr) { nullPtrCount++; continue; }
        
        bool printedAlive = false;
        auto& clsName = actor.className;
        bool aliveEligible = ((clsName.find("BP_Mon") != std::string::npos) || (clsName.find("BP_Boss") != std::string::npos))
                             && (clsName.find("Prop") == std::string::npos)
                             && (clsName.find("Trap") == std::string::npos)
                             && (clsName.find("Collision") == std::string::npos)
                             && (clsName.find("Bullet") == std::string::npos)
                             && (clsName.find("Accessory") == std::string::npos)
                             && (clsName.find("Summon") == std::string::npos);
            
        if (visibleOnly) {
            // Basic vtable sanity check before calling into engine
            void*** vtblAddrActor = reinterpret_cast<void***>(actorPtr);
            if (!vtblAddrActor || !*vtblAddrActor) { nullPtrCount++; continue; }
            if (!pc->LineOfSightTo(actorPtr, cameraLoc, true)) {
                losFailCount++;
                continue;
            }
        }
        // Removed alive gating per request
        
        if (boneMode && !actor.bones.empty()) {
            for (const auto& bone : actor.bones) {
                if (!bone.isValid) continue;
                FVector2D screenPos;
                FVector boneWorld(bone.worldPos.x, bone.worldPos.y, bone.worldPos.z);
                if (!UGameplayStatics::ProjectWorldToScreen(pc, boneWorld, &screenPos, true)) { projectFailCount++; continue; }
                float dx = screenPos.X - screenCenterX;
                float dy = screenPos.Y - screenCenterY;
                float screenDist = sqrtf(dx * dx + dy * dy);
                if (screenDist > fovRadius) { fovRejectCount++; continue; }
                
                
                float dxw = cameraLoc.X - boneWorld.X;
                float dyw = cameraLoc.Y - boneWorld.Y;
                float bzw = cameraLoc.Z - boneWorld.Z;
                float worldDist = sqrtf(dxw*dxw + dyw*dyw + bzw*bzw);
                float score = useCrosshair ? screenDist : worldDist;
                if (score < bestBoneScore) {
                    bestBoneScore = score;
                    bestBonePos.x = bone.worldPos.x;
                    bestBonePos.y = bone.worldPos.y;
                    bestBonePos.z = bone.worldPos.z;
                    bestBoneName = actor.objectName;
                    bestBoneScreen.x = screenPos.X;
                    bestBoneScreen.y = screenPos.Y;
                }
            }
        }
        
        bool considerActor = (!boneMode) || actor.bones.empty();
        if (considerActor) {
            FVector2D screenPos;
            FVector actorWorld(actor.worldPos.x, actor.worldPos.y, actor.worldPos.z);
            if (!UGameplayStatics::ProjectWorldToScreen(pc, actorWorld, &screenPos, true)) { projectFailCount++; continue; }
            float dx = screenPos.X - screenCenterX;
            float dy = screenPos.Y - screenCenterY;
            float screenDist = sqrtf(dx * dx + dy * dy);
            if (screenDist > fovRadius) { fovRejectCount++; continue; }
            
            
            float dxw = cameraLoc.X - actorWorld.X;
            float dyw = cameraLoc.Y - actorWorld.Y;
            float bzw = cameraLoc.Z - actorWorld.Z;
            float worldDist = sqrtf(dxw*dxw + dyw*dyw + bzw*bzw);
            float score = useCrosshair ? screenDist : worldDist;
            if (score < bestActorScore) {
                bestActorScore = score;
                bestActorPos = actor.worldPos;
                bestActorName = actor.objectName;
                bestActorScreen.x = screenPos.X;
                bestActorScreen.y = screenPos.Y;
            }
        }
    }
    
    bool pickedBone = false;
    if (bestBoneScore < FLT_MAX && bestActorScore < FLT_MAX) {
        pickedBone = (bestBoneScore <= bestActorScore);
    } else if (bestBoneScore < FLT_MAX) {
        pickedBone = true;
    }
    
    if (pickedBone) {
        bestTarget.worldPos = bestBonePos;
        bestTarget.screenPos.x = bestBoneScreen.x;
        bestTarget.screenPos.y = bestBoneScreen.y;
        bestTarget.distanceToCenter = (useCrosshair ? sqrtf((bestBoneScreen.x - screenCenterX)*(bestBoneScreen.x - screenCenterX) + (bestBoneScreen.y - screenCenterY)*(bestBoneScreen.y - screenCenterY)) : 0.0f);
        bestTarget.name = bestBoneName;
        bestTarget.isValid = true;
    } else if (bestActorScore < FLT_MAX) {
        bestTarget.worldPos = bestActorPos;
        bestTarget.screenPos.x = bestActorScreen.x;
        bestTarget.screenPos.y = bestActorScreen.y;
        bestTarget.distanceToCenter = (useCrosshair ? sqrtf((bestActorScreen.x - screenCenterX)*(bestActorScreen.x - screenCenterX) + (bestActorScreen.y - screenCenterY)*(bestActorScreen.y - screenCenterY)) : 0.0f);
        bestTarget.name = bestActorName;
        bestTarget.isValid = true;
    }
    
    if (!bestTarget.isValid) {
        printf("[AIMBOT DEBUG] No valid target. Stats: scanned=%d, invalidType=%d, nullPtr=%d, losFail=%d, projectFail=%d, fovReject=%d, fovRadius=%.1f, visibleOnly=%d, boneMode=%d, useCrosshair=%d\n",
               scannedCount, invalidTypeCount, nullPtrCount, losFailCount, projectFailCount, fovRejectCount,
               fovRadius, (int)visibleOnly, (int)boneMode, (int)useCrosshair);
    }
    return bestTarget;
}

bool AimbotCore::IsValidTarget(const ActorInfo& actor) {
    bool isMonster = actor.className.find("BP_Mon") != std::string::npos;
    bool isBoss = actor.className.find("BP_Boss") != std::string::npos;
    
    if (isMonster && !AimbotConfig::TargetMonsters()) return false;
    if (isBoss && !AimbotConfig::TargetBoss()) return false;
    if (!isMonster && !isBoss) return false;
    if (actor.isDead || actor.hpCurrent <= 0.0f) return false;
    
    return true;
}

Vector3 AimbotCore::GetTargetPosition(const ActorInfo& actor) {
    if (AimbotConfig::BoneMode() && !actor.bones.empty()) {
        const auto& bone = actor.bones[0];
        if (bone.isValid) {
            return bone.worldPos;
        }
    }
    
    return actor.worldPos;
}

AimTarget AimbotCore::GetCurrentTarget() {
    std::lock_guard<std::mutex> lock(s_targetMutex);
    return s_currentTarget;
}

void AimbotCore::RenderFOV(ImDrawList* drawList, float screenW, float screenH) {
    if (!AimbotConfig::IsEnabled()) return;
    
    float centerX = screenW * 0.5f;
    float centerY = screenH * 0.5f;
    float radius = AimbotConfig::GetFovRadius();
    
    ImVec4 color = AimbotConfig::GetFovColor();
    ImU32 col = IM_COL32(
        (int)(color.x * 255),
        (int)(color.y * 255),
        (int)(color.z * 255),
        (int)(color.w * 255)
    );
    
    drawList->AddCircle(ImVec2(centerX, centerY), radius, col, 64, 2.0f);
    
    AimTarget target = GetCurrentTarget();
    if (target.isValid) {
        float x1 = floorf(target.screenPos.x) + 0.5f;
        float y1 = floorf(target.screenPos.y) + 0.5f;
        // target marker
        drawList->AddCircleFilled(
            ImVec2(x1, y1),
            5.0f,
            IM_COL32(255, 0, 0, 255),
            12
        );
        // dashed line from center to target
        float x0 = floorf(centerX) + 0.5f, y0 = floorf(centerY) + 0.5f;
        float dx = x1 - x0;
        float dy = y1 - y0;
        float len = sqrtf(dx*dx + dy*dy);
        if (len > 1.0f) {
            float dirx = dx / len;
            float diry = dy / len;
            float dashLen = 8.0f;
            float gapLen = 6.0f;
            float t = 0.0f;
            float thickness = 1.5f;
            while (t < len) {
                float tEnd = t + dashLen;
                if (tEnd > len) tEnd = len;
                ImVec2 p0(x0 + dirx * t, y0 + diry * t);
                ImVec2 p1(x0 + dirx * tEnd, y0 + diry * tEnd);
                drawList->AddLine(p0, p1, col, thickness);
                t = tEnd + gapLen;
            }
        }
    }
}
