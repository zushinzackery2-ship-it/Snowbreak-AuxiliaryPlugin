#include "skeleton_renderer.h"
#include "../data/actor_data.h"
#include "../config/esp_config.h"

void SkeletonRenderer::Render(const ActorInfo& actor, ImDrawList* drawList, float screenW, float screenH, ImU32 cornerColor, ImU32 textColor) {
    bool didDrawSkeleton = false;
    if (ESPConfig::ShowSkeleton() && !actor.bones.empty()) {
        DrawBonePointsAndNames(actor, drawList, screenW, screenH, cornerColor, textColor);
        didDrawSkeleton = true;
    }

    if (!didDrawSkeleton) {
        if (actor.screenPos.z >= 0) {
            if (actor.className.find("BP_Mon") != std::string::npos ||
                actor.className.find("BP_Boss") != std::string::npos) {
                float x = actor.screenPos.x;
                float y = actor.screenPos.y;
                if (!actor.bones.empty()) {
                    const auto& b = actor.bones[0];
                    if (b.isValid && b.screenPos.z >= 0) {
                        x = b.screenPos.x;
                        y = b.screenPos.y;
                    }
                }
                x = floorf(x) + 0.5f;
                y = floorf(y) + 0.5f;
                if (!(x < 0 || x > screenW || y < 0 || y > screenH)) {
                    float distM = actor.distanceCm <= 0.0f ? 0.0f : (actor.distanceCm / 100.0f);
                    float boxSize = 10.0f + 20.0f * (distM / (distM + 10.0f));
                    float cornerLen = boxSize * 0.4f;
                    float thickness = 2.0f;
                    float left = x - boxSize;
                    float right = x + boxSize;
                    float top = y - boxSize;
                    float bottom = y + boxSize;
                    // corners
                    drawList->AddLine(ImVec2(left, top), ImVec2(left + cornerLen, top), cornerColor, thickness);
                    drawList->AddLine(ImVec2(left, top), ImVec2(left, top + cornerLen), cornerColor, thickness);
                    drawList->AddLine(ImVec2(right, top), ImVec2(right - cornerLen, top), cornerColor, thickness);
                    drawList->AddLine(ImVec2(right, top), ImVec2(right, top + cornerLen), cornerColor, thickness);
                    drawList->AddLine(ImVec2(left, bottom), ImVec2(left + cornerLen, bottom), cornerColor, thickness);
                    drawList->AddLine(ImVec2(left, bottom), ImVec2(left, bottom - cornerLen), cornerColor, thickness);
                    drawList->AddLine(ImVec2(right, bottom), ImVec2(right - cornerLen, bottom), cornerColor, thickness);
                    drawList->AddLine(ImVec2(right, bottom), ImVec2(right, bottom - cornerLen), cornerColor, thickness);
                    // center plus
                    float cross = cornerLen * 0.6f;
                    drawList->AddLine(ImVec2(x - cross, y), ImVec2(x + cross, y), cornerColor, 1.5f);
                    drawList->AddLine(ImVec2(x, y - cross), ImVec2(x, y + cross), cornerColor, 1.5f);
                }
            }
        }
    }
}

void SkeletonRenderer::DrawBonePointsAndNames(const ActorInfo& actor, ImDrawList* drawList, float screenW, float screenH, ImU32 cornerColor, ImU32 textColor) {
    
    if (!actor.bones.empty()) {
        const auto& b = actor.bones[0];
        if (b.isValid && b.screenPos.z >= 0) {
            float x = b.screenPos.x;
            float y = b.screenPos.y;
            x = floorf(x) + 0.5f;
            y = floorf(y) + 0.5f;
            if (!(x < 0 || x > screenW || y < 0 || y > screenH)) {
                float distM = actor.distanceCm <= 0.0f ? 0.0f : (actor.distanceCm / 100.0f);
                float boxSize = 10.0f + 20.0f * (distM / (distM + 10.0f));
                float cornerLen = boxSize * 0.4f;
                float thickness = 2.0f;
                float left = x - boxSize;
                float right = x + boxSize;
                float top = y - boxSize;
                float bottom = y + boxSize;
                drawList->AddLine(ImVec2(left, top), ImVec2(left + cornerLen, top), cornerColor, thickness);
                drawList->AddLine(ImVec2(left, top), ImVec2(left, top + cornerLen), cornerColor, thickness);
                drawList->AddLine(ImVec2(right, top), ImVec2(right - cornerLen, top), cornerColor, thickness);
                drawList->AddLine(ImVec2(right, top), ImVec2(right, top + cornerLen), cornerColor, thickness);
                drawList->AddLine(ImVec2(left, bottom), ImVec2(left + cornerLen, bottom), cornerColor, thickness);
                drawList->AddLine(ImVec2(left, bottom), ImVec2(left, bottom - cornerLen), cornerColor, thickness);
                drawList->AddLine(ImVec2(right, bottom), ImVec2(right - cornerLen, bottom), cornerColor, thickness);
                drawList->AddLine(ImVec2(right, bottom), ImVec2(right, bottom - cornerLen), cornerColor, thickness);
                
                // no text labels per requirement
                // center plus
                float cross = cornerLen * 0.6f;
                drawList->AddLine(ImVec2(x - cross, y), ImVec2(x + cross, y), cornerColor, 1.5f);
                drawList->AddLine(ImVec2(x, y - cross), ImVec2(x, y + cross), cornerColor, 1.5f);
            }
        }
    }
}
