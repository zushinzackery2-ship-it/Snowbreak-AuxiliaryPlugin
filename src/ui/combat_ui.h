#pragma once
#include <windows.h>

class CombatUI {
private:
    static bool s_showUI;
public:
    static void Initialize();
    static void Render();
    static void Toggle() { s_showUI = !s_showUI; }
    static bool IsVisible() { return s_showUI; }
    static void SetModernDarkStyle();
private:
    static void RenderConfigWindow();
};
