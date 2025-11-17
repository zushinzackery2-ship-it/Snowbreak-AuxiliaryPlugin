#pragma once
#include "../../imgui/imgui.h"

class AimbotConfig {
private:
    static bool s_enabled;
    static bool s_visibleOnly;
    static bool s_boneMode;
    static bool s_crosshairMode;
    static float s_fovRadius;
    static float s_smoothing;
    static int s_aimKey;
    static bool s_targetMonsters;
    static bool s_targetBoss;
    static ImVec4 s_fovColor;
    
public:
    static void Initialize();
    
    static bool IsEnabled() { return s_enabled; }
    static void SetEnabled(bool enabled) { s_enabled = enabled; }
    
    static bool VisibleOnly() { return s_visibleOnly; }
    static void SetVisibleOnly(bool v) { s_visibleOnly = v; }
    
    static bool BoneMode() { return s_boneMode; }
    static void SetBoneMode(bool v) { s_boneMode = v; }
    
    static bool CrosshairMode() { return s_crosshairMode; }
    static void SetCrosshairMode(bool v) { s_crosshairMode = v; }
    
    static float GetFovRadius() { return s_fovRadius; }
    static void SetFovRadius(float v) { s_fovRadius = v; }
    
    static float GetSmoothing() { return s_smoothing; }
    static void SetSmoothing(float v) { s_smoothing = v; }
    
    static int GetAimKey() { return s_aimKey; }
    static void SetAimKey(int v) { s_aimKey = v; }
    
    static bool TargetMonsters() { return s_targetMonsters; }
    static void SetTargetMonsters(bool v) { s_targetMonsters = v; }
    
    static bool TargetBoss() { return s_targetBoss; }
    static void SetTargetBoss(bool v) { s_targetBoss = v; }
    
    static ImVec4 GetFovColor() { return s_fovColor; }
    static void SetFovColor(const ImVec4& color) { s_fovColor = color; }
    
    static bool* GetEnabledPtr() { return &s_enabled; }
    static bool* GetVisibleOnlyPtr() { return &s_visibleOnly; }
    static bool* GetBoneModePtr() { return &s_boneMode; }
    static bool* GetCrosshairModePtr() { return &s_crosshairMode; }
    static float* GetFovRadiusPtr() { return &s_fovRadius; }
    static float* GetSmoothingPtr() { return &s_smoothing; }
    static int* GetAimKeyPtr() { return &s_aimKey; }
    static bool* GetTargetMonstersPtr() { return &s_targetMonsters; }
    static bool* GetTargetBossPtr() { return &s_targetBoss; }
    static ImVec4* GetFovColorPtr() { return &s_fovColor; }
};
