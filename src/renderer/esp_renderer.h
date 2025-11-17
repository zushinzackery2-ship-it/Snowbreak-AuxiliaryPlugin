#pragma once
#include "../../imgui/imgui.h"

class ESPRenderer {
public:
    static void Initialize();
    static void Render();
    
private:
    static void RenderActorESP();
    static void DrawActorInfo(const struct ActorInfo& actor, ImDrawList* drawList, float screenW, float screenH);
};
