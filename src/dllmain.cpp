#include <windows.h>
#include <cstdio>
#include "hook/dx11_hook.h"
#include "collector/actor_collector.h"
#include "config/esp_config.h"
#include "ui/ui_manager.h"
#include "ui/aimbot_ui.h"
#include "renderer/esp_renderer.h"
#include "aimbot/aimbot_config.h"
#include "aimbot/aimbot_core.h"
#include "config/config_io.h"
#include "config/combat_config.h"
#include "ui/combat_ui.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);

        CreateThread(nullptr, 0, [](LPVOID) -> DWORD {
            AllocConsole();
            FILE* fDummy;
            freopen_s(&fDummy, "CONOUT$", "w", stdout);
            freopen_s(&fDummy, "CONIN$", "r", stdin);
            
            printf("===========================================\n");
            printf("ActorESP with Aimbot Debug Mode\n");
            printf("===========================================\n");
            printf("Insert - Toggle ESP/Aimbot Config\n");
            printf("F2 - Quick Toggle Aimbot\n");
            printf("Right Mouse - Aim (default)\n");
            printf("===========================================\n\n");
            
            ESPConfig::Initialize();
            AimbotConfig::Initialize();
            CombatConfig::Initialize();
            ConfigIO::Initialize();
            UIManager::Initialize();
            AimbotUI::Initialize();
            CombatUI::Initialize();
            ESPRenderer::Initialize();
            
            ActorCollector::Start();
            AimbotCore::Start();
            
            return 0;
        }, nullptr, 0, nullptr);

        CreateThread(nullptr, 0, [](LPVOID) -> DWORD {
            DX11Hook::Initialize();
            return 0;
        }, nullptr, 0, nullptr);
        break;

    case DLL_PROCESS_DETACH:
        ActorCollector::Stop();
        AimbotCore::Stop();
        ConfigIO::SaveAll();
        DX11Hook::Shutdown();
        break;
    }
    return TRUE;
}
