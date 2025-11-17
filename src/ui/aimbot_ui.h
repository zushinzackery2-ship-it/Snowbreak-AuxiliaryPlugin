#pragma once
#include "../../imgui/imgui.h"

class AimbotUI {
private:
    static bool s_showUI;
    
public:
    static void Initialize();
    static void Render();
    static void RenderConfigWindow();
    
    static bool IsVisible() { return s_showUI; }
    static void SetVisible(bool visible) { s_showUI = visible; }
    static void Toggle() { s_showUI = !s_showUI; }
    
    static void SetModernDarkStyle();
};
