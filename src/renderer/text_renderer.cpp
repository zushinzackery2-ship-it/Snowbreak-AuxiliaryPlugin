#include "text_renderer.h"
#include "../data/actor_data.h"
#include "../config/esp_config.h"

void TextRenderer::DrawTextWithShadow(ImDrawList* drawList, float x, float y, ImU32 color, const char* text) {
    ImVec2 textSize = ImGui::CalcTextSize(text);
    ImVec2 textPos(x - textSize.x / 2, y);
    
    // Shadow
    drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 255), text);
    // Text
    drawList->AddText(textPos, color, text);
}

void TextRenderer::RenderClassName(const ActorInfo& actor, ImDrawList* drawList, float x, float y) {
    if (!ESPConfig::ShowClassName()) return;
    
    ImU32 textCol = ImGui::ColorConvertFloat4ToU32(ESPConfig::GetTextColor());
    DrawTextWithShadow(drawList, x, y, textCol, actor.className.c_str());
}

void TextRenderer::RenderObjectName(const ActorInfo& actor, ImDrawList* drawList, float x, float y) {
    if (!ESPConfig::ShowNames()) return;
    
    ImU32 textCol = ImGui::ColorConvertFloat4ToU32(ESPConfig::GetTextColor());
    DrawTextWithShadow(drawList, x, y, textCol, actor.objectName.c_str());
}

void TextRenderer::RenderDistance(const ActorInfo& actor, ImDrawList* drawList, float x, float y) {
    if (!ESPConfig::ShowDistance()) return;
    
    float dist = actor.worldPos.Length() / 100.0f;
    char distBuffer[64];
    snprintf(distBuffer, sizeof(distBuffer), "%.1fm", dist);
    
    DrawTextWithShadow(drawList, x, y, IM_COL32(200, 200, 200, 255), distBuffer);
}

void TextRenderer::RenderPointer(const ActorInfo& actor, ImDrawList* drawList, float x, float y) {
    char ptrBuffer[32];
    snprintf(ptrBuffer, sizeof(ptrBuffer), "[0x%llX]", (unsigned long long)actor.actorPtr);
    
    DrawTextWithShadow(drawList, x, y, IM_COL32(255, 255, 0, 255), ptrBuffer);
}
