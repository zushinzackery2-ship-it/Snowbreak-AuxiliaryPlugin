#pragma once
#include <windows.h>

class UIManager {
private:
    static bool s_showUI;
    
public:
    static void Initialize();
    static void Render();
    static void ToggleUI() { s_showUI = !s_showUI; }
    static void HideUI() { s_showUI = false; }
    static bool IsUIVisible() { return s_showUI; }
    static void SetModernDarkStyle();
    
private:
    static void RenderMainWindows();
};
