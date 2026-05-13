#ifndef RENDER_CPP
#define RENDER_CPP

#include <impl/includes.hpp>

auto render_interface::c_render::setup( const std::wstring & proc_name ) -> setup_error_code
{
	HWND game_window = FindWindowW( L"Notepad" , NULL );
	if ( !game_window )
		return setup_error_code::game_window_not_found;

	this->game_window = game_window;
	this->game_process_id = 0;

	bool game_dimensions = get_window_dimensions( );
	if ( !game_dimensions )
		return setup_error_code::game_window_dimensions_error;

	HWND overlay_window = CreateWindowExW(
		WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_TOPMOST ,
		L"Static" ,
		L"overlay" ,
		WS_POPUP ,
		game_rect.left , game_rect.top ,
		game_width , game_height ,
		NULL , NULL ,
		GetModuleHandleW( NULL ) ,
		NULL
	);

	if ( !overlay_window )
		return setup_error_code::overlay_process_not_found;

	MARGINS margin = { -1, -1, -1, -1 };
	DwmExtendFrameIntoClientArea( overlay_window , &margin );
	SetLayeredWindowAttributes( overlay_window , 0 , 255 , LWA_ALPHA );
	ShowWindow( overlay_window , SW_SHOW );

	this->overlay_window = overlay_window;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd , sizeof( sd ) );

	sd.BufferCount = 1;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = overlay_window;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL featureLevel;

	const D3D_FEATURE_LEVEL featureLevelArray [ 2 ] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};

	if ( D3D11CreateDeviceAndSwapChain( NULL , D3D_DRIVER_TYPE_HARDWARE , NULL , NULL , featureLevelArray , 2 , D3D11_SDK_VERSION , &sd , &g_pSwapChain , &g_pd3dDevice , &featureLevel , &g_pd3dDeviceContext ) != S_OK )
		return setup_error_code::d3d11_error;

	ID3D11Texture2D * pBackBuffer;
	g_pSwapChain->GetBuffer( 0 , IID_PPV_ARGS( &pBackBuffer ) );
	if ( pBackBuffer )
	{
		g_pd3dDevice->CreateRenderTargetView( pBackBuffer , NULL , &g_mainRenderTargetView );
		pBackBuffer->Release( );
	}

	IMGUI_CHECKVERSION( );
	ImGui::CreateContext( );

	if ( !ImGui_ImplWin32_Init( overlay_window ) )
		return setup_error_code::imgui_win32_error;

	if ( !ImGui_ImplDX11_Init( g_pd3dDevice , g_pd3dDeviceContext ) )
		return setup_error_code::imgui_d3d11_error;

	vars->load( );
	menu->initialize( );

	return setup_error_code::setup_success;
}

void render_interface::c_render::render_thread( )
{
	while ( msg.message != WM_QUIT )
	{
		if ( LI_FN( PeekMessageA ).safe_cached( )( &msg , overlay_window , 0 , 0 , PM_REMOVE ) )
		{
			LI_FN( TranslateMessage ).safe_cached( )( &msg );
			LI_FN( DispatchMessageA ).safe_cached( )( &msg );
		}

		this->begin_scene( );
		{
			this->update_cursor( );

			if ( vars->ui.menu.keybind.did_press( false ) )
				vars->ui.menu.state = !vars->ui.menu.state;

			if ( vars->ui.menu.is_enabled( ) )
				menu->draw( );
		}
		this->end_scene( );
	}
}

bool render_interface::c_render::get_window_dimensions( )
{
	bool rect = LI_FN( GetClientRect ).safe_cached( )( this->game_window, &this->game_rect );
	if ( !rect )
		return false;

	this->game_width = game_rect.right - game_rect.left;
	this->game_height = game_rect.bottom - game_rect.top;

	POINT screen_left_top = { game_rect.left, game_rect.top };

	bool screen = LI_FN( ClientToScreen ).safe_cached( )( this->game_window, &screen_left_top );
	if ( !screen )
		return false;

	this->game_rect.left = screen_left_top.x;
	this->game_rect.right = screen_left_top.x + this->game_width;

	this->game_rect.top = screen_left_top.y;
	this->game_rect.bottom = screen_left_top.y + this->game_height;

	return true;
}

bool render_interface::c_render::get_game_window_status( ) const
{
	auto foreground_wnd = LI_FN( GetForegroundWindow ).safe_cached( )( );
	auto active_wnd = LI_FN( GetActiveWindow ).safe_cached( )( );

	return this->game_window == foreground_wnd || this->game_window == active_wnd;
}

bool render_interface::c_render::resize_overlay( u32 width, u32 height, POINT screen )
{
	this->game_width = width;
	this->game_height = height;

	this->game_rect.left = screen.x;
	this->game_rect.right = screen.x + width;

	this->game_rect.top = screen.y;
	this->game_rect.bottom = screen.y + height;

	if ( g_mainRenderTargetView )
	{
		g_pd3dDeviceContext->OMSetRenderTargets( 0, nullptr, nullptr );
		g_mainRenderTargetView->Release( );
		g_mainRenderTargetView = nullptr;
	}

	g_pSwapChain->ResizeBuffers( 0, width, height, DXGI_FORMAT_UNKNOWN, 0 );

	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer( 0, IID_PPV_ARGS( &pBackBuffer ) );
	if ( pBackBuffer )
	{
		g_pd3dDevice->CreateRenderTargetView( pBackBuffer, nullptr, &g_mainRenderTargetView );
		pBackBuffer->Release( );
	}

	::SetWindowPos( this->overlay_window, HWND_TOP, screen.x, screen.y, width, height, SWP_NOZORDER );

	return true;
}

HWND render_interface::c_render::get_window_handle( u32 process_id )
{
	std::pair<HWND, DWORD> lParam = { HWND( ), process_id };

	BOOL result = LI_FN( EnumWindows ).safe_cached( )( [ ] ( HWND hwnd, LPARAM lParam ) -> int {
		auto pParams = ( std::pair<HWND, DWORD>* )( lParam );

		DWORD processId;
		if ( LI_FN( GetWindowThreadProcessId ).safe_cached( )( hwnd, &processId ) && processId == pParams->second ) 
		{
			pParams->first = hwnd;
			return false;
		}

		return true;
	}, reinterpret_cast< LPARAM >( &lParam ) );

	if ( !result )
		return lParam.first;

	return 0;
}

void render_interface::c_render::begin_scene( )
{
	ImGui_ImplDX11_NewFrame( );
	ImGui_ImplWin32_NewFrame( );
	ImGui::NewFrame( );
}

void render_interface::c_render::end_scene( )
{
	ImGui::Render( );

	const float clear_color_with_alpha [ 4 ] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_pd3dDeviceContext->OMSetRenderTargets( 1, &g_mainRenderTargetView, NULL );
	g_pd3dDeviceContext->ClearRenderTargetView( g_mainRenderTargetView, clear_color_with_alpha );

	ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );

	g_pSwapChain->Present( DXGI_SWAP_EFFECT_DISCARD, 0 );
}

void render_interface::c_render::update_cursor( ) const
{
	ImGuiIO& io = ImGui::GetIO( );
	static bool previous_keys_down [ 256 ] = { false };

	io.MouseDown [ 0 ] = GetAsyncKeyState( VK_LBUTTON ) & 0x8000;
	io.MouseDown [ 1 ] = GetAsyncKeyState( VK_RBUTTON ) & 0x8000;
	io.MouseDown [ 2 ] = GetAsyncKeyState( VK_MBUTTON ) & 0x8000;
	io.MouseDown [ 3 ] = GetAsyncKeyState( VK_XBUTTON1 ) & 0x8000;
	io.MouseDown [ 4 ] = GetAsyncKeyState( VK_XBUTTON2 ) & 0x8000;

	POINT cursorPos;
	GetCursorPos( &cursorPos );
	ScreenToClient( this->overlay_window, &cursorPos );
	io.MousePos = ImVec2( static_cast< float >( cursorPos.x ), static_cast< float >( cursorPos.y ) );

	io.AddKeyEvent( ImGuiKey_ModShift, ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) != 0 );
	io.AddKeyEvent( ImGuiKey_ModCtrl, ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) != 0 );
	io.AddKeyEvent( ImGuiKey_ModAlt, ( GetAsyncKeyState( VK_MENU ) & 0x8000 ) != 0 );

	for ( int i = 0; i < 256; i++ )
	{
		bool key_down = ( GetAsyncKeyState( i ) & 0x8000 ) != 0;

		if ( key_down != previous_keys_down [ i ] )
		{
			ImGuiKey imgui_key = ImGuiKey_None;

			switch ( i )
			{
			case VK_BACK: imgui_key = ImGuiKey_Backspace; break;
			case VK_DELETE: imgui_key = ImGuiKey_Delete; break;
			case VK_RETURN: imgui_key = ImGuiKey_Enter; break;
			case VK_ESCAPE: imgui_key = ImGuiKey_Escape; break;
			case VK_TAB: imgui_key = ImGuiKey_Tab; break;
			case VK_LEFT: imgui_key = ImGuiKey_LeftArrow; break;
			case VK_RIGHT: imgui_key = ImGuiKey_RightArrow; break;
			case VK_UP: imgui_key = ImGuiKey_UpArrow; break;
			case VK_DOWN: imgui_key = ImGuiKey_DownArrow; break;
			case VK_HOME: imgui_key = ImGuiKey_Home; break;
			case VK_END: imgui_key = ImGuiKey_End; break;
			case VK_PRIOR: imgui_key = ImGuiKey_PageUp; break;
			case VK_NEXT: imgui_key = ImGuiKey_PageDown; break;
			case VK_INSERT: imgui_key = ImGuiKey_Insert; break;
			case VK_SPACE: imgui_key = ImGuiKey_Space; break;
			default:
				if ( i >= 'A' && i <= 'Z' ) {
					imgui_key = ( ImGuiKey ) ( ImGuiKey_A + ( i - 'A' ) );
				}
				else if ( i >= '0' && i <= '9' ) {
					imgui_key = ( ImGuiKey ) ( ImGuiKey_0 + ( i - '0' ) );
				}
				break;
			}

			if ( imgui_key != ImGuiKey_None )
			{
				io.AddKeyEvent( imgui_key, key_down );
			}

			if ( key_down && !previous_keys_down [ i ] )
			{
				if ( i >= 'A' && i <= 'Z' ) {
					bool shift_pressed = ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) != 0;
					bool caps_lock = ( GetKeyState( VK_CAPITAL ) & 0x0001 ) != 0;

					if ( !shift_pressed && !caps_lock ) {
						io.AddInputCharacter( static_cast< ImWchar >( i + 32 ) );
					}
					else {
						io.AddInputCharacter( static_cast< ImWchar >( i ) );
					}
				}
				else if ( i >= '0' && i <= '9' ) {
					io.AddInputCharacter( static_cast< ImWchar >( i ) );
				}
				else if ( i == VK_SPACE ) {
					io.AddInputCharacter( static_cast< ImWchar >( ' ' ) );
				}
			}
		}

		previous_keys_down [ i ] = key_down;
	}
}

#endif // !RENDER_CPP