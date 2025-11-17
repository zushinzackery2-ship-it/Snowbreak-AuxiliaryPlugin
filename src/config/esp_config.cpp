#include "esp_config.h"

bool ESPConfig::s_espEnabled = true;
bool ESPConfig::s_showNames = true;
bool ESPConfig::s_showClassName = true;
bool ESPConfig::s_showDistance = true;
bool ESPConfig::s_showSkeleton = false;
bool ESPConfig::s_skeletonFallback = false;
float ESPConfig::s_maxDistance = 5000.0f;
ImVec4 ESPConfig::s_textColor = ImVec4(0.0f, 1.0f, 0.5f, 1.0f);

void ESPConfig::Initialize() {
    s_espEnabled = true;
    s_showNames = true;
    s_showClassName = true;
    s_showDistance = true;
    s_showSkeleton = false;
    s_skeletonFallback = false;
    s_maxDistance = 5000.0f;
    s_textColor = ImVec4(0.0f, 1.0f, 0.5f, 1.0f);
}
