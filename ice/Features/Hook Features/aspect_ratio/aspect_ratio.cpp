#include "../../../IceBox.hpp"

#ifdef _WIN64

#include "../../../../Core/Utils/Haru Hook/haru_hook_install.hpp"

extern "C" {
extern unsigned long long aspect_mid_resume_storage;
void aspect_mid_stub( );
}

namespace {
mid_hook::MidHook g;
}

bool IceBox::aspect_ratio_install( )
{
	return HaruHook::install( g, 0xB2FA0E, 8, reinterpret_cast<void*>( aspect_mid_stub ),
	    reinterpret_cast<std::uint64_t*>( &aspect_mid_resume_storage ) );
}

void IceBox::aspect_ratio_uninstall( )
{
	HaruHook::remove( g );
}

bool IceBox::aspect_ratio_installed( )
{
	return g.installed( );
}

#else

bool IceBox::aspect_ratio_install( )
{
	return true;
}

void IceBox::aspect_ratio_uninstall( )
{
}

bool IceBox::aspect_ratio_installed( )
{
	return false;
}

#endif
