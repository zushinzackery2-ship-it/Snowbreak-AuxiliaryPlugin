#pragma once
#include "../../imgui/imgui.h"

class ESPConfig {
private:
    static bool s_espEnabled;
    static bool s_showNames;
    static bool s_showClassName;
    static bool s_showDistance;
    static bool s_showSkeleton;
    static bool s_skeletonFallback;
    static float s_maxDistance;
    static ImVec4 s_textColor;
    
public:
    static void Initialize();
    
    static bool IsEnabled() { return s_espEnabled; }
    static void SetEnabled(bool enabled) { s_espEnabled = enabled; }
    
    static bool ShowNames() { return s_showNames; }
    static void SetShowNames(bool show) { s_showNames = show; }
    
    static bool ShowClassName() { return s_showClassName; }
    static void SetShowClassName(bool show) { s_showClassName = show; }
    
    static bool ShowDistance() { return s_showDistance; }
    static void SetShowDistance(bool show) { s_showDistance = show; }
    
    static bool ShowSkeleton() { return s_showSkeleton; }
    static void SetShowSkeleton(bool show) { s_showSkeleton = show; }
    
    static bool SkeletonFallback() { return s_skeletonFallback; }
    static void SetSkeletonFallback(bool v) { s_skeletonFallback = v; }
    
    static float GetMaxDistance() { return s_maxDistance; }
    static void SetMaxDistance(float dist) { s_maxDistance = dist; }
    
    static ImVec4 GetTextColor() { return s_textColor; }
    static void SetTextColor(const ImVec4& color) { s_textColor = color; }
    
    static ImVec4* GetTextColorPtr() { return &s_textColor; }
    static bool* GetEnabledPtr() { return &s_espEnabled; }
    static bool* GetShowNamesPtr() { return &s_showNames; }
    static bool* GetShowClassNamePtr() { return &s_showClassName; }
    static bool* GetShowDistancePtr() { return &s_showDistance; }
    static bool* GetShowSkeletonPtr() { return &s_showSkeleton; }
    static bool* GetSkeletonFallbackPtr() { return &s_skeletonFallback; }
    static float* GetMaxDistancePtr() { return &s_maxDistance; }
};
