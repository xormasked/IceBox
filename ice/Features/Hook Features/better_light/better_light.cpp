#include "../../../IceBox.hpp"

#ifdef _WIN64

#include "../../../../Core/Utils/Haru Hook/haru_hook_install.hpp"

extern "C" {
extern unsigned long long better_light_resume_normal;
extern unsigned long long better_light_branch_taken;
void better_light_mid_stub( );
}

namespace {
mid_hook::MidHook g;
}

bool IceBox::better_light_install( )
{
	better_light_resume_normal = Memory::ImageBase + 0xFB066E;
	better_light_branch_taken = Memory::ImageBase + 0xFB07D4;
	return HaruHook::install( g, 0xFB0664, 10, reinterpret_cast<void*>( better_light_mid_stub ), nullptr );
}

void IceBox::better_light_uninstall( )
{
	HaruHook::remove( g );
}

bool IceBox::better_light_installed( )
{
	return g.installed( );
}

void IceBox::better_light_prepare_uninject( ) noexcept
{
	better_light_uninstall( );
}

#else

bool IceBox::better_light_install( )
{
	return true;
}

void IceBox::better_light_uninstall( )
{
}

bool IceBox::better_light_installed( )
{
	return false;
}

void IceBox::better_light_prepare_uninject( ) noexcept
{
}

#endif
