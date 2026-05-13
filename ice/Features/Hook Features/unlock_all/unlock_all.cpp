#include "../../../IceBox.hpp"

#ifdef _WIN64

#include "../../../../Core/Utils/Haru Hook/haru_hook_install.hpp"

extern "C" {
	extern unsigned long long unlock_all_mid_resume;
	void unlock_all_mid_stub( );
}

namespace {
	mid_hook::MidHook g;
}

bool IceBox::unlock_all_install( )
{
	return HaruHook::install( g, 0x3843080, 7, reinterpret_cast< void* >( unlock_all_mid_stub ),
		reinterpret_cast< std::uint64_t* >( &unlock_all_mid_resume ) );
}

void IceBox::unlock_all_uninstall( )
{
	HaruHook::remove( g );
}

bool IceBox::unlock_all_installed( )
{
	return g.installed( );
}

#else

bool IceBox::unlock_all_install( )
{
	return true;
}

void IceBox::unlock_all_uninstall( )
{
}

bool IceBox::unlock_all_installed( )
{
	return false;
}

#endif
