

#include "Hook/directx/d3d11hook.hpp"

extern "C" __declspec( dllexport ) int BlindEyeHook( int code, WPARAM wParam, LPARAM lParam ) { return CallNextHookEx( NULL, code, wParam, lParam ); }

auto __stdcall DllMain( HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved ) -> bool
{
	if ( dwReason == DLL_PROCESS_ATTACH )
	{

		//Engine::Get()->SetupConsole();

		d3d11::dll_handle = hModule;
		CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE ) d3d11::main, NULL, 0, NULL );
	}
	else if ( dwReason == DLL_PROCESS_DETACH )
	{

	}
	return true;
}

