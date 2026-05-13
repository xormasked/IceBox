#include "../../IceBox.hpp"

#ifdef _WIN64

#include "../../../Core/Utils/Haru Hook/mid_hook.hpp"
#include "../../../Core/Utils/memory.hpp"

extern "C" {
extern unsigned long long no_spread_mid_resume;
void no_spread_mid_stub( );
}

namespace {

	constexpr std::uintptr_t k_spread_movss_rva = 0x1C3A2CA;

	mid_hook::MidHook g_hook;

} // namespace

bool IceBox::no_spread_install( )
{
	return mid_hook::midhook( g_hook,
	                          Memory::ImageBase + k_spread_movss_rva,
	                          8,
	                          reinterpret_cast< void* >( no_spread_mid_stub ),
	                          reinterpret_cast< std::uint64_t* >( &no_spread_mid_resume ) );
}

void IceBox::no_spread_uninstall( )
{
	mid_hook::midhook_remove( g_hook );
}

bool IceBox::no_spread_installed( )
{
	return g_hook.installed( );
}

#else

bool IceBox::no_spread_install( )
{
	return true;
}

void IceBox::no_spread_uninstall( )
{
}

bool IceBox::no_spread_installed( )
{
	return false;
}

#endif
