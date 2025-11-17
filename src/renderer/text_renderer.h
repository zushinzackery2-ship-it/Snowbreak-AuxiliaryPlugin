#pragma once
#include "../../imgui/imgui.h"
#include <string>

class TextRenderer {
public:
    // Render class name
    static void RenderClassName(const struct ActorInfo& actor, ImDrawList* drawList, float x, float y);
    
    // Render object name
    static void RenderObjectName(const struct ActorInfo& actor, ImDrawList* drawList, float x, float y);
    
    // Render distance
    static void RenderDistance(const struct ActorInfo& actor, ImDrawList* drawList, float x, float y);
    
    // Render actor pointer
    static void RenderPointer(const struct ActorInfo& actor, ImDrawList* drawList, float x, float y);
    
private:
    static void DrawTextWithShadow(ImDrawList* drawList, float x, float y, ImU32 color, const char* text);
};
