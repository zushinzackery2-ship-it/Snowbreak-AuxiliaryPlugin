#pragma once
#include "../../imgui/imgui.h"

class SkeletonRenderer {
public:
    static void Render(const struct ActorInfo& actor, ImDrawList* drawList, float screenW, float screenH, ImU32 cornerColor, ImU32 textColor);
    
private:
    static void DrawBonePointsAndNames(const struct ActorInfo& actor, ImDrawList* drawList, float screenW, float screenH, ImU32 cornerColor, ImU32 textColor);
};
