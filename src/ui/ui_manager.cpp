#include <Windows.h>
#include "ui_manager.h"
#include "../data/actor_data.h"
#include "../config/esp_config.h"
#include "../aimbot/aimbot_config.h"
#include "../config/combat_config.h"
#include "../../imgui/imgui.h"

bool UIManager::s_showUI = false;

void UIManager::Initialize() {
    s_showUI = true;
}

void UIManager::Render() {
    if (s_showUI) {
        RenderMainWindows();
    }
}

void UIManager::SetModernDarkStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.90f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.95f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.98f);
    colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 0.50f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.70f, 1.00f, 0.80f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.60f, 1.00f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.60f, 0.00f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.70f, 0.30f, 1.00f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.80f, 0.40f, 1.00f, 1.00f);
    
    style.WindowRounding = 0.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 8.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 6.0f;
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;
    style.WindowPadding = ImVec2(40.0f, 40.0f);
    style.FramePadding = ImVec2(12.0f, 8.0f);
    style.ItemSpacing = ImVec2(15.0f, 10.0f);
    style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
    style.IndentSpacing = 25.0f;
    style.ScrollbarSize = 16.0f;
    style.GrabMinSize = 12.0f;
}

void UIManager::RenderMainWindows() {
    // 绘瞄窗口（合并绘制与自瞄）
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(520, 520), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("绘瞄 [INS]", nullptr, 0)) {
        // ESP：仅保留一个开关
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.5f, 1.0f), "绘制设置");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Checkbox("启用绘制", ESPConfig::GetEnabledPtr());

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "自瞄设置");
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.2f, 1.0f), "F2开关自瞄");
        ImGui::Checkbox("启用自瞄", AimbotConfig::GetEnabledPtr());
        if (AimbotConfig::IsEnabled()) {
            ImGui::Spacing();
            ImGui::Text("目标设置");
            ImGui::Separator();
            ImGui::Checkbox("锁定怪物", AimbotConfig::GetTargetMonstersPtr());
            ImGui::Checkbox("锁定Boss", AimbotConfig::GetTargetBossPtr());
            ImGui::Checkbox("视线检测", AimbotConfig::GetVisibleOnlyPtr());

            

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("视野与平滑");
            ImGui::SliderFloat("视野半径(像素)", AimbotConfig::GetFovRadiusPtr(), 20.0f, 500.0f, "%.0f");
            ImGui::SliderFloat("平滑度", AimbotConfig::GetSmoothingPtr(), 1.0f, 20.0f, "%.1f");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("按键");
            int currentKey = AimbotConfig::GetAimKey();
            const char* keyName = "未知";
            if (currentKey == VK_RBUTTON) keyName = "鼠标右键";
            else if (currentKey == VK_LBUTTON) keyName = "鼠标左键";
            else if (currentKey == VK_MBUTTON) keyName = "鼠标中键";
            else if (currentKey == VK_XBUTTON1) keyName = "鼠标侧键4";
            else if (currentKey == VK_XBUTTON2) keyName = "鼠标侧键5";
            else if (currentKey == VK_SHIFT) keyName = "Shift";
            else if (currentKey == VK_CONTROL) keyName = "Ctrl";
            ImGui::Text("当前按键: %s", keyName);
            if (ImGui::Button("鼠标左键")) AimbotConfig::SetAimKey(VK_LBUTTON);
            ImGui::SameLine();
            if (ImGui::Button("鼠标右键")) AimbotConfig::SetAimKey(VK_RBUTTON);
            ImGui::SameLine();
            if (ImGui::Button("鼠标侧键4")) AimbotConfig::SetAimKey(VK_XBUTTON1);
            ImGui::SameLine();
            if (ImGui::Button("鼠标侧键5")) AimbotConfig::SetAimKey(VK_XBUTTON2);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("FOV圆颜色");
            ImGui::ColorEdit4("颜色", (float*)AimbotConfig::GetFovColorPtr());
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "按 Insert 开关此窗口");
    }
    ImGui::End();

    // 其他功能窗口（F8 行为选择）
    ImGui::SetNextWindowPos(ImVec2(980, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(360, 200), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("其他功能 [INS]", nullptr, 0)) {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "F8 功能选择");
        ImGui::Separator();
        ImGui::Spacing();
        int mode = CombatConfig::GetF8ActionMode();
        if (ImGui::RadioButton("一键胜利（单人）", mode == 0)) { CombatConfig::SetF8ActionMode(0); }
        if (ImGui::RadioButton("精神协作+1000（多人）", mode == 1)) { CombatConfig::SetF8ActionMode(1); }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.2f, 1.0f), "按 F8 触发当前选择");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "按 Insert 开关此窗口");
    }
    ImGui::End();
}
