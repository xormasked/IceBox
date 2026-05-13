#ifndef RENDER_HPP
#define RENDER_HPP

namespace render_interface
{
    enum class setup_error_code
    {
        process_not_found,
        game_window_not_found,
        game_window_dimensions_error,
        bobi_error,
        overlay_process_not_found,
        d3d11_error,
        imgui_win32_error,
        imgui_d3d11_error,
        setup_success
    };

    class c_render
    {
    public:
        c_render( ) = default;
        ~c_render( ) = default;

    private:
        u32 game_process_id { };

    private:
        RECT game_rect { };
        HWND game_window { };

    public:
        float game_width { };
        float game_height { };

    private:
        HWND overlay_window { };
        MSG msg { };

    private:
        IDXGISwapChain* g_pSwapChain { };
        ID3D11DeviceContext* g_pd3dDeviceContext { };
        ID3D11RenderTargetView* g_mainRenderTargetView { };

    public:
        ID3D11Device* g_pd3dDevice { };
        setup_error_code setup( const std::wstring& proc_name );

    private:
        bool get_window_dimensions( );
        bool get_game_window_status( ) const;
        HWND get_window_handle( u32 process_id );
        bool resize_overlay( u32 width, u32 height, POINT screen );

    private:
        void end_scene( );
        void begin_scene( );
        void update_cursor( ) const;

    public:
        void render_thread( );
    };

} inline auto render = std::make_shared<render_interface::c_render>( );

#endif // !RENDER_HPP