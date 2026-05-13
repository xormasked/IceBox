#include "../../../IceBox.hpp"

#ifdef _WIN64

#include "../../../../Core/Utils/Haru Hook/haru_hook_install.hpp"

extern "C" {
	extern unsigned long long no_spread_mid_resume;
	void no_spread_mid_stub( );
}

namespace {
	mid_hook::MidHook g;
}

bool IceBox::no_spread_install( )
{
	return HaruHook::install( g, 0x1C3A2CA, 8, reinterpret_cast< void* >( no_spread_mid_stub ),
		reinterpret_cast< std::uint64_t* >( &no_spread_mid_resume ) );
}

void IceBox::no_spread_uninstall( )
{
	HaruHook::remove( g );
}

bool IceBox::no_spread_installed( )
{
	return g.installed( );
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
