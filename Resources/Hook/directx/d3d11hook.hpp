#ifndef _Render
#define _Render

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "../../External/Minhook/MinHook.h"
#if _WIN64 
#pragma comment(lib, "libMinHook.x64.lib")
#else
#pragma comment(lib, "libMinHook.x86.lib")
#endif

#include "../../External/ImGui/imgui.h"
#include "../../External/ImGui/imgui_internal.h"
#include "../../External/ImGui/imgui_impl_dx11.h"
#include "../../External/ImGui/imgui_impl_win32.h"

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")


#include "Rendering/interface.hpp"



namespace d3d11 {

	inline HINSTANCE dll_handle;


	auto get_present_pointer( ) -> bool;

	auto __stdcall WndProc( const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )->LRESULT;

	static long __stdcall detour_present( IDXGISwapChain* p_swap_chain, UINT sync_interval, UINT flags );

	auto __stdcall EjectThread( LPVOID lpParameter )->DWORD;

	auto __stdcall main( ) -> int;

	inline bool should_uninject = false;

}


#endif