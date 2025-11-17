#include "aimbot_config.h"
#include <windows.h>

bool AimbotConfig::s_enabled = false;
bool AimbotConfig::s_visibleOnly = true;
bool AimbotConfig::s_boneMode = true;
bool AimbotConfig::s_crosshairMode = true;
float AimbotConfig::s_fovRadius = 100.0f;
float AimbotConfig::s_smoothing = 1.0f;
int AimbotConfig::s_aimKey = VK_RBUTTON;
bool AimbotConfig::s_targetMonsters = true;
bool AimbotConfig::s_targetBoss = true;
ImVec4 AimbotConfig::s_fovColor = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);

void AimbotConfig::Initialize() {
    s_enabled = false;
    s_visibleOnly = true;
    s_boneMode = true;
    s_crosshairMode = true;
    s_fovRadius = 100.0f;
    s_smoothing = 1.0f;
    s_aimKey = VK_RBUTTON;
    s_targetMonsters = true;
    s_targetBoss = true;
    s_fovColor = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
}
