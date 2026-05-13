// Dear ImGui standalone example application for Windows API + DirectX 11
// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#pragma once
#include <tchar.h>
#include <impl/includes.hpp>
#include <core/drawing/watermark.hpp>

// Data
static ID3D11Device * g_pd3dDevice = nullptr;
static ID3D11DeviceContext * g_pd3dDeviceContext = nullptr;
static IDXGISwapChain * g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0 , g_ResizeHeight = 0;
static ID3D11RenderTargetView * g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D( HWND hWnd );
void CleanupDeviceD3D( );
void CreateRenderTarget( );
void CleanupRenderTarget( );
LRESULT WINAPI WndProc( HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam );

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam );

int main( int , char ** )
{
    // Make process DPI aware and obtain main monitor scale
    ImGui_ImplWin32_EnableDpiAwareness( );
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor( ::MonitorFromPoint( POINT { 0, 0 } , MONITOR_DEFAULTTOPRIMARY ) );

    // Create application window
    WNDCLASSEXW wc = { sizeof( wc ), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle( nullptr ), nullptr, nullptr, nullptr, nullptr, L"Calamari-Rust Menu", nullptr };
    ::RegisterClassExW( &wc );
    HWND hwnd = ::CreateWindowW( wc.lpszClassName , L"Calamari-Rust Menu" , WS_OVERLAPPEDWINDOW , 100 , 100 , ( int ) ( 1280 * main_scale ) , ( int ) ( 800 * main_scale ) , nullptr , nullptr , wc.hInstance , nullptr );

    // Initialize Direct3D
    if ( !CreateDeviceD3D( hwnd ) )
    {
        CleanupDeviceD3D( );
        ::UnregisterClassW( wc.lpszClassName , wc.hInstance );
        return 1;
    }

    // Show the window
    ::ShowWindow( hwnd , SW_SHOWDEFAULT );
    ::UpdateWindow( hwnd );

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION( );
    ImGui::CreateContext( );
    ImGuiIO & io = ImGui::GetIO( ); ( void ) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark( );

    // Setup scaling
    ImGuiStyle & style = ImGui::GetStyle( );
    style.ScaleAllSizes( main_scale );
    style.FontScaleDpi = main_scale;

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init( hwnd );
    ImGui_ImplDX11_Init( g_pd3dDevice , g_pd3dDeviceContext );

    // Initialize menu
    menu_interface::c_interface g_interface;
    bool menu_initialized = false;
    bool menu_visible = true;
    std::string menu_error_msg = "";

    try
    {
        
        menu_initialized = true;
        OutputDebugStringA( "[Menu] Initialization successful\n" );
    }
    catch ( const std::exception & e )
    {
        menu_error_msg = std::string( "[Exception] " ) + e.what( );
        OutputDebugStringA( "[Menu] Exception during initialization: " );
        OutputDebugStringA( e.what( ) );
        OutputDebugStringA( "\n" );
    }
    catch ( ... )
    {
        menu_error_msg = "[Unknown Exception] Menu initialization failed";
        OutputDebugStringA( "[Menu] Unknown error during initialization\n" );
    }

    ImVec4 clear_color = ImVec4( 0.06f , 0.06f , 0.06f , 1.00f );



	// main loop sonnna

    bool  done = false;
    bool  g_menuVisible = true;   // INSERT toggles this
    float g_menuAlpha   = 1.0f;   // 0..1, animated
    bool  g_insertWasDown = false;

    while ( !done )
    {
        // Poll and handle messages (inputs, window resize, etc.)
        MSG msg;
        while ( ::PeekMessage( &msg , nullptr , 0U , 0U , PM_REMOVE ) )
        {
            ::TranslateMessage( &msg );
            ::DispatchMessage( &msg );
            if ( msg.message == WM_QUIT )
                done = true;
        }
        if ( done )
            break;

        // Handle window being minimized or screen locked
        if ( g_SwapChainOccluded && g_pSwapChain->Present( 0 , DXGI_PRESENT_TEST ) == DXGI_STATUS_OCCLUDED )
        {
            ::Sleep( 10 );
            continue;
        }
        g_SwapChainOccluded = false;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if ( g_ResizeWidth != 0 && g_ResizeHeight != 0 )
        {
            CleanupRenderTarget( );
            g_pSwapChain->ResizeBuffers( 0 , g_ResizeWidth , g_ResizeHeight , DXGI_FORMAT_UNKNOWN , 0 );
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget( );
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame( );
        ImGui_ImplWin32_NewFrame( );
        ImGui::NewFrame( );

        // INSERT key: toggle menu visibility with fade
        {
            bool insertDown = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
            if (insertDown && !g_insertWasDown)
                g_menuVisible = !g_menuVisible;
            g_insertWasDown = insertDown;

            float dt = ImGui::GetIO().DeltaTime;
            float target = g_menuVisible ? 1.0f : 0.0f;
            g_menuAlpha += (target - g_menuAlpha) * ImClamp(dt * 8.0f, 0.0f, 1.0f);
        }

        // Draw menu (faded)
        if (menu_initialized && g_menuAlpha > 0.01f)
        {
            try
            {
                // Push global alpha for fade effect
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, g_menuAlpha);
                g_interface.draw();
                ImGui::PopStyleVar();
            }
            catch (const std::exception& e)
            {
                OutputDebugStringA("[Menu Draw] Exception: ");
                OutputDebugStringA(e.what());
                OutputDebugStringA("\n");
            }
            catch (...)
            {
                OutputDebugStringA("[Menu Draw] Unknown error\n");
            }
        }
        else if (!menu_initialized && g_menuAlpha > 0.01f)
        {
            ImGui::Begin( "Menu Error" , nullptr , ImGuiWindowFlags_AlwaysAutoResize );
            ImGui::TextWrapped( "Menu failed to initialize" );
            ImGui::Separator( );
            if ( !menu_error_msg.empty( ) )
            {
                ImGui::TextColored( ImVec4( 1 , 0.5f , 0.5f , 1 ) , "%s" , menu_error_msg.c_str( ) );
            }
            ImGui::TextWrapped( "Note: Make sure all dependencies are properly loaded." );
            ImGui::End( );
        }

        // Watermark — always drawn (fades with menu)
        Watermark::Draw(g_menuAlpha);

        // Rendering
        ImGui::Render( );
        const float clear_color_with_alpha [ 4 ] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets( 1 , &g_mainRenderTargetView , nullptr );
        g_pd3dDeviceContext->ClearRenderTargetView( g_mainRenderTargetView , clear_color_with_alpha );
        ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );

        // Present
        HRESULT hr = g_pSwapChain->Present( 1 , 0 );
        g_SwapChainOccluded = ( hr == DXGI_STATUS_OCCLUDED );
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown( );
    ImGui_ImplWin32_Shutdown( );
    ImGui::DestroyContext( );

    CleanupDeviceD3D( );
    ::DestroyWindow( hwnd );
    ::UnregisterClassW( wc.lpszClassName , wc.hInstance );

    return 0;
}

// Helper functions

bool CreateDeviceD3D( HWND hWnd )
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd , sizeof( sd ) );
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray [ 2 ] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    HRESULT res = D3D11CreateDeviceAndSwapChain( nullptr , D3D_DRIVER_TYPE_HARDWARE , nullptr , createDeviceFlags , featureLevelArray , 2 , D3D11_SDK_VERSION , &sd , &g_pSwapChain , &g_pd3dDevice , &featureLevel , &g_pd3dDeviceContext );
    if ( res == DXGI_ERROR_UNSUPPORTED )
        res = D3D11CreateDeviceAndSwapChain( nullptr , D3D_DRIVER_TYPE_WARP , nullptr , createDeviceFlags , featureLevelArray , 2 , D3D11_SDK_VERSION , &sd , &g_pSwapChain , &g_pd3dDevice , &featureLevel , &g_pd3dDeviceContext );
    if ( res != S_OK )
        return false;

    CreateRenderTarget( );
    return true;
}

void CleanupDeviceD3D( )
{
    CleanupRenderTarget( );
    if ( g_pSwapChain ) { g_pSwapChain->Release( ); g_pSwapChain = nullptr; }
    if ( g_pd3dDeviceContext ) { g_pd3dDeviceContext->Release( ); g_pd3dDeviceContext = nullptr; }
    if ( g_pd3dDevice ) { g_pd3dDevice->Release( ); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget( )
{
    ID3D11Texture2D * pBackBuffer;
    g_pSwapChain->GetBuffer( 0 , IID_PPV_ARGS( &pBackBuffer ) );
    g_pd3dDevice->CreateRenderTargetView( pBackBuffer , nullptr , &g_mainRenderTargetView );
    pBackBuffer->Release( );
}

void CleanupRenderTarget( )
{
    if ( g_mainRenderTargetView ) { g_mainRenderTargetView->Release( ); g_mainRenderTargetView = nullptr; }
}

// Win32 message handler
LRESULT WINAPI WndProc( HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam )
{
    if ( ImGui_ImplWin32_WndProcHandler( hWnd , msg , wParam , lParam ) )
        return true;

    switch ( msg )
    {
    case WM_SIZE:
        if ( wParam == SIZE_MINIMIZED )
            return 0;
        g_ResizeWidth = ( UINT ) LOWORD( lParam );
        g_ResizeHeight = ( UINT ) HIWORD( lParam );
        return 0;
    case WM_SYSCOMMAND:
        if ( ( wParam & 0xfff0 ) == SC_KEYMENU )
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage( 0 );
        return 0;
    }
    return ::DefWindowProcW( hWnd , msg , wParam , lParam );
}
