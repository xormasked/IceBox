#include "d3d11hook.hpp"

#include "../../../Core/Engine/Anvil/scimitar.hpp"
#include "Rendering/interface.hpp"
#include "Rendering/Render.hpp"
#include "../../../Core/Crash Handler/crash_handler.hpp"

typedef long(__stdcall* present_t)(IDXGISwapChain*, UINT, UINT);
present_t p_present = nullptr;
present_t p_present_target = nullptr;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace d3d11 {

    bool init = false;
    HWND window = NULL;
    ID3D11Device* p_device = NULL;
    ID3D11DeviceContext* p_context = NULL;
    ID3D11RenderTargetView* mainRenderTargetView = NULL;
    WNDPROC oWndProc = nullptr;

    auto get_present_pointer() -> bool
    {
        DXGI_SWAP_CHAIN_DESC sd{};
        sd.BufferCount = 2;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = GetForegroundWindow();
        sd.SampleDesc.Count = 1;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        IDXGISwapChain* swap_chain = nullptr;
        ID3D11Device* device = nullptr;

        const D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
        if (D3D11CreateDeviceAndSwapChain(
            NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
            feature_levels, 2,
            D3D11_SDK_VERSION,
            &sd, &swap_chain, &device, nullptr, nullptr) == S_OK)
        {
            void** p_vtable = *reinterpret_cast<void***>(swap_chain);
            swap_chain->Release();
            device->Release();
            p_present_target = (present_t)p_vtable[8];
            return true;
        }
        return false;
    }

    auto __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
            return true;
        return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
    }

    static long __stdcall detour_present(IDXGISwapChain* p_swap_chain, UINT sync_interval, UINT flags)
    {
        if (!init) {
            if (SUCCEEDED(p_swap_chain->GetDevice(__uuidof(ID3D11Device), (void**)&p_device)))
            {
                p_device->GetImmediateContext(&p_context);
                DXGI_SWAP_CHAIN_DESC sd{};
                p_swap_chain->GetDesc(&sd);
                window = sd.OutputWindow;

                ID3D11Texture2D* pBackBuffer = nullptr;
                p_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
                p_device->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
                pBackBuffer->Release();

                oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

                ImGui::CreateContext();
                ImGuiIO& io = ImGui::GetIO();
                io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
                ImGui_ImplWin32_Init(window);
                ImGui_ImplDX11_Init(p_device, p_context);
                init = true;
            }
            else {
                return p_present(p_swap_chain, sync_interval, flags);
            }
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (Scimitar::round_state::get() == 2 || Scimitar::round_state::get() == 3) {
            Render::Renderables();
        }

        Render::user_interface();

        ImGui::EndFrame();
        ImGui::Render();
        p_context->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        return p_present(p_swap_chain, sync_interval, flags);
    }

    auto __stdcall EjectThread(LPVOID) -> DWORD {
        Sleep(100);
        FreeLibraryAndExitThread(dll_handle, 0);
        return 0;
    }

    auto __stdcall main() -> int
    {
        CrashCatch::Config crash_config;
        crash_config.dumpFolder = "./crash_dumps/";
        crash_config.dumpFileName = "dx11hook_crash";
        crash_config.appVersion = "1.3.0";
        crash_config.buildConfig = "Release";
        crash_config.additionalNotes = "StackTrace example - v1.3.0";
        crash_config.includeStackTrace = true;
        crash_config.onCrash = [](const CrashCatch::CrashContext& context) {
            std::cout << "\n[CrashCatch] Crash captured!\n";
            std::cout << "  Timestamp : " << context.timestamp << "\n";
            std::cout << "  Log file  : " << context.logFilePath << "\n";
#ifdef CRASHCATCH_PLATFORM_WINDOWS
            std::cout << "  Dump file : " << context.dumpFilePath << "\n";
#endif
            std::cout << "  Signal    : " << context.signalOrCode << "\n";
        };
        crash_config.showCrashDialog = true;
        CrashCatch::initialize(crash_config);

        Scimitar::init();

        if (!get_present_pointer()) return 1;
        if (MH_Initialize() != MH_OK) return 1;
        if (MH_CreateHook(reinterpret_cast<void**>(p_present_target), &detour_present, reinterpret_cast<void**>(&p_present)) != MH_OK) return 1;
        if (MH_EnableHook(p_present_target) != MH_OK) return 1;

        while (true) {
            Sleep(10);
            if (GetAsyncKeyState(VK_INSERT) & 1) {
                Render::menu_open = !Render::menu_open;
            }
            if ((GetAsyncKeyState(VK_END) & 1) || should_uninject) break;
        }

        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        if (mainRenderTargetView) { mainRenderTargetView->Release(); mainRenderTargetView = NULL; }
        if (p_context) { p_context->Release(); p_context = NULL; }
        if (p_device) { p_device->Release(); p_device = NULL; }
        if (window && oWndProc) SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);

        CreateThread(0, 0, EjectThread, 0, 0, 0);
        return 0;
    }
}
