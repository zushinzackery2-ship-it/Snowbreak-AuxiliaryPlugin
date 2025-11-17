#include "dx11_hook.h"
#include "../ui/ui_manager.h"
#include "../ui/aimbot_ui.h"
#include "../ui/combat_ui.h"
#include "../renderer/esp_renderer.h"
#include "../aimbot/aimbot_config.h"
#include "../aimbot/aimbot_core.h"
#include "../config/config_io.h"
#include "../config/combat_config.h"
#include "../../imgui/imgui.h"
#include "../../imgui/backends/imgui_impl_win32.h"
#include "../../imgui/backends/imgui_impl_dx11.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HRESULT(WINAPI* DX11Hook::OriginalPresent)(IDXGISwapChain*, UINT, UINT) = nullptr;
WNDPROC DX11Hook::OriginalWndProc = nullptr;
Microsoft::WRL::ComPtr<ID3D11Device> DX11Hook::g_pDevice;
Microsoft::WRL::ComPtr<ID3D11DeviceContext> DX11Hook::g_pContext;
Microsoft::WRL::ComPtr<ID3D11RenderTargetView> DX11Hook::g_pRenderTargetView;
bool DX11Hook::g_bInitialized = false;
HWND DX11Hook::g_hGameWindow = nullptr;
HWND DX11Hook::g_hDummyWindow = nullptr;

bool DX11Hook::Initialize() {
    g_hGameWindow = GetForegroundWindow();
    if (!g_hGameWindow) {
        g_hGameWindow = GetActiveWindow();
    }
    return HookPresent();
}

void DX11Hook::Shutdown() {
    if (g_bInitialized) {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
    
    g_pRenderTargetView.Reset();
    g_pContext.Reset();
    g_pDevice.Reset();
    
    if (g_hGameWindow && OriginalWndProc) {
        SetWindowLongPtr(g_hGameWindow, GWLP_WNDPROC, (LONG_PTR)OriginalWndProc);
    }
    
    g_bInitialized = false;
    if (g_hDummyWindow) {
        DestroyWindow(g_hDummyWindow);
        g_hDummyWindow = nullptr;
    }
}

bool DX11Hook::HookPresent() {
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    HWND targetWnd = g_hGameWindow ? g_hGameWindow : GetForegroundWindow();
    if (!targetWnd) targetWnd = EnsureDummyWindow();
    swapChainDesc.OutputWindow = targetWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    IDXGISwapChain* pTempSwapChain = nullptr;
    ID3D11Device* pTempDevice = nullptr;
    ID3D11DeviceContext* pTempContext = nullptr;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        &featureLevel, 1, D3D11_SDK_VERSION,
        &swapChainDesc, &pTempSwapChain, &pTempDevice, nullptr, &pTempContext
    );

    if (FAILED(hr)) {
        return false;
    }

    void** pVTable = *reinterpret_cast<void***>(pTempSwapChain);
    OriginalPresent = reinterpret_cast<HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT)>(pVTable[8]);

    DWORD oldProtect;
    if (VirtualProtect(&pVTable[8], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        pVTable[8] = &PresentHook;
        VirtualProtect(&pVTable[8], sizeof(void*), oldProtect, &oldProtect);
    }

    pTempSwapChain->Release();
    pTempDevice->Release();
    pTempContext->Release();

    return true;
}

HRESULT WINAPI DX11Hook::PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    if (!g_bInitialized) {
        InitializeImGui(pSwapChain);
    }

    if (g_bInitialized) {
        RenderFrame();
    }

    return OriginalPresent(pSwapChain, SyncInterval, Flags);
}

void DX11Hook::InitializeImGui(IDXGISwapChain* pSwapChain) {
    ID3D11Device* rawDevice = nullptr;
    if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&rawDevice))) {
        return;
    }
    g_pDevice.Attach(rawDevice);
    ID3D11DeviceContext* rawContext = nullptr;
    g_pDevice->GetImmediateContext(&rawContext);
    g_pContext.Attach(rawContext);

    ID3D11Texture2D* pBackBuffer = nullptr;
    if (FAILED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer))) {
        return;
    }

    if (FAILED(g_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, g_pRenderTargetView.GetAddressOf()))) {
        pBackBuffer->Release();
        return;
    }
    pBackBuffer->Release();

    DXGI_SWAP_CHAIN_DESC desc;
    pSwapChain->GetDesc(&desc);
    g_hGameWindow = desc.OutputWindow;

    OriginalWndProc = (WNDPROC)SetWindowLongPtr(g_hGameWindow, GWLP_WNDPROC, (LONG_PTR)WndProcHook);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImFont* chineseFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    if (!chineseFont) {
        io.Fonts->AddFontDefault();
    }

    UIManager::SetModernDarkStyle();
    AimbotUI::SetModernDarkStyle();
    CombatUI::SetModernDarkStyle();

    ImGui_ImplWin32_Init(g_hGameWindow);
    ImGui_ImplDX11_Init(g_pDevice.Get(), g_pContext.Get());

    g_bInitialized = true;
}

LRESULT WINAPI DX11Hook::WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_KEYDOWN) {
        if (wParam == VK_F2) {
            AimbotConfig::SetEnabled(!AimbotConfig::IsEnabled());
            return 0;
        }
        if (wParam == VK_INSERT) {
            UIManager::ToggleUI();
            CombatUI::Toggle();
            return 0;
        }
        if (wParam == VK_F8) {
            CombatConfig::TriggerF8Action();
            return 0;
        }
        // remove F9 per merged UI
    }

    if (DX11Hook::g_bInitialized && ImGui::GetCurrentContext() != nullptr) {
        if ((UIManager::IsUIVisible() || CombatUI::IsVisible()) && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
            return true;
        }
    }

    return CallWindowProc(OriginalWndProc, hWnd, msg, wParam, lParam);
}

void DX11Hook::RenderFrame() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ESPRenderer::Render();
    {
        ImGuiIO& io = ImGui::GetIO();
        ImDrawList* dl = ImGui::GetForegroundDrawList();
        AimbotCore::RenderFOV(dl, io.DisplaySize.x, io.DisplaySize.y);
    }
    UIManager::Render();
    CombatUI::Render();
    {
        static DWORD s_lastSave = 0;
        DWORD now = GetTickCount();
        if (now - s_lastSave >= 500) {
            ConfigIO::SaveIfNeeded();
            s_lastSave = now;
        }
    }

    ImGui::Render();
    {
        ID3D11RenderTargetView* rtv = g_pRenderTargetView.Get();
        g_pContext->OMSetRenderTargets(1, &rtv, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
}

HWND DX11Hook::EnsureDummyWindow() {
    if (g_hDummyWindow && IsWindow(g_hDummyWindow)) return g_hDummyWindow;
    WNDCLASSA wc{};
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = GetModuleHandleA(nullptr);
    wc.lpszClassName = "DX11DummyWnd";
    static ATOM s_atom = 0;
    if (!s_atom) {
        s_atom = RegisterClassA(&wc);
    }
    g_hDummyWindow = CreateWindowExA(0, wc.lpszClassName, "dx11_dummy", WS_OVERLAPPEDWINDOW,
                                     CW_USEDEFAULT, CW_USEDEFAULT, 100, 100,
                                     nullptr, nullptr, wc.hInstance, nullptr);
    return g_hDummyWindow;
}
