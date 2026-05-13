#include "../../IceBox.hpp"
#include "../../../Core/Utils/memory.hpp"
#include "../../../Resources/config.hpp"
#include "../../../Resources/External/MinHook/MinHook.h"

#ifdef _WIN64

namespace {

	constexpr std::uintptr_t k_sub_1c62040_rva = 0x1C62040;

	void __fastcall no_recoil_detour( __int64 a1 )
	{
		( void ) a1;
	}

	void* g_target = nullptr;
	void* g_trampoline = nullptr;
	bool g_hook_created = false;
	bool g_hook_enabled = false;

} // namespace

bool IceBox::no_recoil_install( )
{
	g_target = reinterpret_cast< void* >( Memory::ImageBase + k_sub_1c62040_rva );
	if ( !g_hook_created ) {
		if ( MH_CreateHook( g_target, reinterpret_cast< LPVOID >( &no_recoil_detour ), &g_trampoline ) != MH_OK )
			return false;
		g_hook_created = true;
	}
	if ( MH_EnableHook( g_target ) != MH_OK )
		return false;
	g_hook_enabled = true;
	return true;
}

void IceBox::no_recoil_uninstall( )
{
	if ( !g_hook_created || !g_target )
		return;
	MH_DisableHook( g_target );
	g_hook_enabled = false;
}

bool IceBox::no_recoil_installed( )
{
	return g_hook_enabled;
}

void IceBox::no_recoil_prepare_uninject( ) noexcept
{
	if ( !g_hook_created || !g_target )
		return;
	MH_DisableHook( g_target );
	MH_RemoveHook( g_target );
	g_hook_created = false;
	g_trampoline = nullptr;
	g_hook_enabled = false;
	g_target = nullptr;
}

#else

bool IceBox::no_recoil_install( )
{
	return true;
}

void IceBox::no_recoil_uninstall( )
{
}

bool IceBox::no_recoil_installed( )
{
	return false;
}

void IceBox::no_recoil_prepare_uninject( ) noexcept
{
}

#endif
