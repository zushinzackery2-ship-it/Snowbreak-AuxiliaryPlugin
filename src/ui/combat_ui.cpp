#include "combat_ui.h"
#include "../config/combat_config.h"
#include "../../imgui/imgui.h"

bool CombatUI::s_showUI = true;

void CombatUI::Initialize() {
    s_showUI = true;
}

void CombatUI::Render() {
    if (s_showUI) {
        RenderConfigWindow();
    }
}

void CombatUI::SetModernDarkStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
}

void CombatUI::RenderConfigWindow() {
    ImGui::SetNextWindowPos(ImVec2(560, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(420, 220), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("战斗增强 [INS]", nullptr, 0)) {
        ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.2f, 1.0f), "战斗增强设置");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Checkbox("快速恢复", CombatConfig::GetFastRecoverEnabledPtr());

        ImGui::Checkbox("防御倍率", CombatConfig::GetDefenseEnabledPtr());
        ImGui::SameLine();
        ImGui::SliderFloat("##DefenseMul", CombatConfig::GetDefenseMultiplierPtr(), 1.0f, 5.0f, "x%.1f");

        ImGui::Checkbox("加速倍率", CombatConfig::GetSpeedEnabledPtr());
        ImGui::SameLine();
        ImGui::SliderFloat("##SpeedMul", CombatConfig::GetSpeedMultiplierPtr(), 1.0f, 5.0f, "x%.1f");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "按 Insert 开关此窗口");
    }
    ImGui::End();
}
