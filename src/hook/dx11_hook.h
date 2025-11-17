#pragma once
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>

class DX11Hook {
private:
    static HRESULT(WINAPI* OriginalPresent)(IDXGISwapChain*, UINT, UINT);
    static WNDPROC OriginalWndProc;
    static Microsoft::WRL::ComPtr<ID3D11Device> g_pDevice;
    static Microsoft::WRL::ComPtr<ID3D11DeviceContext> g_pContext;
    static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> g_pRenderTargetView;
    static bool g_bInitialized;
    static HWND g_hGameWindow;
    static HWND g_hDummyWindow;

public:
    static bool Initialize();
    static void Shutdown();
    
private:
    static bool HookPresent();
    static HRESULT WINAPI PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    static void InitializeImGui(IDXGISwapChain* pSwapChain);
    static LRESULT WINAPI WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static void RenderFrame();
    static HWND EnsureDummyWindow();
};
