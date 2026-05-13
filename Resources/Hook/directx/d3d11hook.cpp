#include "d3d11hook.hpp"

#include "../../../Core/Engine/Anvil/scimitar.hpp"
#include "../../../Core/Engine/engine.hpp"
#include "Rendering/interface.hpp"
#include "Rendering/Render.hpp"
#include "../../../Core/Crash Handler/crash_handler.hpp"
#include "../../../ice/IceBox.hpp"
#include "../../../ice/UbiHookBypass/UbiHook.hpp"
#include "../../../Core/Utils/Haru Hook/haru_hook.hpp"
#include "../../../Core/Utils/Haru Hook/mid_hook.hpp"
#include "../../../Resources/config.hpp"

#include <iostream>

#ifdef _WIN64
extern "C" float aspect_ratio_live;
#endif

typedef long( __stdcall* present_t )( IDXGISwapChain*, UINT, UINT );
present_t p_present = nullptr;
present_t p_present_target = nullptr;
extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

namespace {

	static bool s_tp_key_was_down = false;
	static bool s_tp_toggled_on = false;

	void third_person_tick( float dt )
	{
		if ( !visuals::ThirdPerson ) {
			s_tp_key_was_down = false;
			s_tp_toggled_on = false;
			IceBox::third_person( false, dt );
			return;
		}
		const bool key_down = ( GetAsyncKeyState( visuals::ThirdPersonVk ) & 0x8000 ) != 0;
		if ( key_down && !s_tp_key_was_down )
			s_tp_toggled_on = !s_tp_toggled_on;
		s_tp_key_was_down = key_down;
		IceBox::third_person( s_tp_toggled_on, dt );
	}

#ifdef _WIN64
	void sync_install_toggle( bool& enabled,
	                          bool& last,
	                          bool ( *install )( ),
	                          void ( *uninstall )( ),
	                          bool ( *installed )( ) )
	{
		if ( enabled != last ) {
			if ( enabled ) {
				if ( !install( ) )
					enabled = false;
			} else
				uninstall( );
			last = enabled;
		} else if ( enabled && !installed( ) )
			install( );
	}
#endif

} // namespace

namespace d3d11 {

	bool init = false;
	volatile bool g_shutting_down = false;
	HWND window = NULL;
	ID3D11Device* p_device = NULL;
	ID3D11DeviceContext* p_context = NULL;
	ID3D11RenderTargetView* mainRenderTargetView = NULL;
	WNDPROC oWndProc = nullptr;

	auto get_present_pointer( ) -> bool
	{
		DXGI_SWAP_CHAIN_DESC sd{};
		sd.BufferCount = 2;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = GetForegroundWindow( );
		sd.SampleDesc.Count = 1;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		IDXGISwapChain* swap_chain = nullptr;
		ID3D11Device* device = nullptr;

		const D3D_FEATURE_LEVEL feature_levels[ ] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
		if ( D3D11CreateDeviceAndSwapChain(
		         NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, feature_levels, 2, D3D11_SDK_VERSION, &sd, &swap_chain,
		         &device, nullptr, nullptr ) != S_OK )
			return false;

		void** p_vtable = *reinterpret_cast< void*** >( swap_chain );
		swap_chain->Release( );
		device->Release( );
		p_present_target = ( present_t ) p_vtable[ 8 ];
		return true;
	}

	auto __stdcall WndProc( const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) -> LRESULT
	{
		if ( !g_shutting_down && Render::menu_open &&
		     ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam ) )
			return true;
		return CallWindowProc( oWndProc, hWnd, uMsg, wParam, lParam );
	}

	static long __stdcall detour_present( IDXGISwapChain* p_swap_chain, UINT sync_interval, UINT flags )
	{
		if ( g_shutting_down )
			return p_present( p_swap_chain, sync_interval, flags );

		if ( !init ) {
			if ( !SUCCEEDED( p_swap_chain->GetDevice( __uuidof( ID3D11Device ), ( void** ) &p_device ) ) )
				return p_present( p_swap_chain, sync_interval, flags );
			p_device->GetImmediateContext( &p_context );
			DXGI_SWAP_CHAIN_DESC sd{};
			p_swap_chain->GetDesc( &sd );
			window = sd.OutputWindow;

			ID3D11Texture2D* pBackBuffer = nullptr;
			p_swap_chain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* ) &pBackBuffer );
			p_device->CreateRenderTargetView( pBackBuffer, NULL, &mainRenderTargetView );
			pBackBuffer->Release( );

			oWndProc = ( WNDPROC ) SetWindowLongPtr( window, GWLP_WNDPROC, ( LONG_PTR ) WndProc );

			ImGui::CreateContext( );
			ImGui::GetIO( ).ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
			ImGui_ImplWin32_Init( window );
			ImGui_ImplDX11_Init( p_device, p_context );
			init = true;
		}

		ImGui_ImplDX11_NewFrame( );
		ImGui_ImplWin32_NewFrame( );

		const bool round_active = Scimitar::round_state::CurrentState( Scimitar::round_state::Prep ) ||
		                          Scimitar::round_state::CurrentState( Scimitar::round_state::Action );

		if ( round_active )
			IceBox::rage_bot_run( visuals::RageBot );

		ImGui::NewFrame( );
		const float dt = ImGui::GetIO( ).DeltaTime;

		IceBox::raycast_debug_hit_marker_decay( dt );
		IceBox::jitter_peek_tick( dt );
		IceBox::chat_spammer_tick( );

#ifdef _WIN64
		static bool s_aspect_toggle_last = false;
		static bool s_run_shoot_last = false;
		static bool s_unlock_all_last = false;
		static bool s_no_spread_last = false;
		static bool s_no_recoil_last = false;

		if ( !round_active ) {
			IceBox::aspect_ratio_uninstall( );
			IceBox::run_and_shoot_uninstall( );
			s_aspect_toggle_last = visuals::AspectRatioHook;
			s_run_shoot_last = visuals::RunAndShoot;
		} else {
			sync_install_toggle( visuals::AspectRatioHook, s_aspect_toggle_last, IceBox::aspect_ratio_install,
			                     IceBox::aspect_ratio_uninstall, IceBox::aspect_ratio_installed );
			sync_install_toggle( visuals::RunAndShoot, s_run_shoot_last, IceBox::run_and_shoot_install,
			                     IceBox::run_and_shoot_uninstall, IceBox::run_and_shoot_installed );
		}

		sync_install_toggle( visuals::UnlockAllMidHook, s_unlock_all_last, IceBox::unlock_all_install,
		                     IceBox::unlock_all_uninstall, IceBox::unlock_all_installed );

		sync_install_toggle( visuals::NoSpread, s_no_spread_last, IceBox::no_spread_install, IceBox::no_spread_uninstall,
		                     IceBox::no_spread_installed );

		sync_install_toggle( visuals::NoRecoil, s_no_recoil_last, IceBox::no_recoil_install, IceBox::no_recoil_uninstall,
		                     IceBox::no_recoil_installed );

		if ( round_active && visuals::AspectRatioHook && IceBox::aspect_ratio_installed( ) )
			aspect_ratio_live = visuals::AspectRatio;
#endif

		if ( round_active )
			IceBox::raycast_closest_player_debug_tick( dt );

		Render::Renderables( );
		Render::user_interface( );

		if ( round_active ) {
			IceBox::world_modulation_monitor_game( );
			IceBox::camera_fx_apply_fov( );
			IceBox::world_modulation_apply( );
			IceBox::world_glow_apply( );
			IceBox::long_melee( visuals::LongMelee );
			third_person_tick( dt );
		}

		ImGui::EndFrame( );
		ImGui::Render( );
		p_context->OMSetRenderTargets( 1, &mainRenderTargetView, NULL );
		ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );

		return p_present( p_swap_chain, sync_interval, flags );
	}

	auto __stdcall EjectThread( LPVOID lpParameter ) -> DWORD {
		Sleep( 300 );
		FreeLibraryAndExitThread( static_cast< HMODULE >( lpParameter ), 0 );
		return 0;
	}

	auto __stdcall main( ) -> int
	{
		const auto release_hooks_and_cave = []( ) {
			mid_hook::uninstall_all( );
			HaruHook::release_injection_cave( );
		};

		Engine::Get( )->SetupConsole( );

		constexpr size_t kCaveBytes = 4096;
		HaruHook::injection_code_cave = HaruHook::allocate_code_cave( kCaveBytes );
		HaruHook::injection_code_cave_bytes = HaruHook::injection_code_cave ? kCaveBytes : 0;
		if ( !HaruHook::injection_code_cave )
			std::cout << "[Haru Hook] injection code cave allocation FAILED\n";

		CrashCatch::Config crash_config;
		crash_config.dumpFolder = "./crash_dumps/";
		crash_config.dumpFileName = "dx11hook_crash";
		crash_config.appVersion = "1.3.0";
		crash_config.buildConfig = "Release";
		crash_config.additionalNotes = "StackTrace example - v1.3.0";
		crash_config.includeStackTrace = true;
		crash_config.onCrash = []( const CrashCatch::CrashContext& ctx ) {
			std::cout << "\n[CrashCatch] Crash captured!\n"
			          << "  Timestamp : " << ctx.timestamp << "\n"
			          << "  Log file  : " << ctx.logFilePath << "\n"
#ifdef CRASHCATCH_PLATFORM_WINDOWS
			          << "  Dump file : " << ctx.dumpFilePath << "\n"
#endif
			          << "  Signal    : " << ctx.signalOrCode << "\n";
		};
		crash_config.showCrashDialog = true;
		CrashCatch::initialize( crash_config );

		Scimitar::init( );

		auto bail = [ & ] {
			release_hooks_and_cave( );
			return 1;
		};
		if ( !get_present_pointer( ) || !init_hooks( ) || MH_CreateHook( reinterpret_cast< void** >( p_present_target ),
		                                                                   &detour_present,
		                                                                   reinterpret_cast< void** >( &p_present ) ) != MH_OK ||
		     MH_EnableHook( p_present_target ) != MH_OK )
			return bail( );

		while ( true ) {
			Sleep( 10 );
			if ( GetAsyncKeyState( VK_INSERT ) & 1 )
				Render::menu_open = !Render::menu_open;
			if ( ( GetAsyncKeyState( VK_END ) & 1 ) || should_uninject ) {
				IceBox::world_modulation_prepare_uninject( );
				IceBox::world_glow_prepare_uninject( );
				IceBox::camera_fx_prepare_uninject( );
				IceBox::no_recoil_prepare_uninject( );
				break;
			}
		}

		g_shutting_down = true;
		IceBox::third_person_reset( );
		release_hooks_and_cave( );
		unhook( );
		MH_DisableHook( MH_ALL_HOOKS );
		Sleep( 500 );
		MH_Uninitialize( );

		ImGui_ImplDX11_Shutdown( );
		ImGui_ImplWin32_Shutdown( );
		ImGui::DestroyContext( );

		if ( mainRenderTargetView ) {
			mainRenderTargetView->Release( );
			mainRenderTargetView = NULL;
		}
		if ( p_context ) {
			p_context->Release( );
			p_context = NULL;
		}
		if ( p_device ) {
			p_device->Release( );
			p_device = NULL;
		}
		if ( window && oWndProc )
			SetWindowLongPtr( window, GWLP_WNDPROC, ( LONG_PTR ) oWndProc );

		Sleep( 100 );
		CreateThread( nullptr, 0, EjectThread, dll_handle, 0, nullptr );
		return 0;
	}
} // namespace d3d11
