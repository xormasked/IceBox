#include "../../../IceBox.hpp"

#ifdef _WIN64

#include "../../../../Core/Utils/Haru Hook/haru_hook_install.hpp"

extern "C" {
	extern unsigned long long run_shoot_mid_resume_a, run_shoot_mid_resume_b;
	void run_shoot_mid_stub_a( ), run_shoot_mid_stub_b( );
}

namespace {
	mid_hook::MidHook g_a, g_b;
}

bool IceBox::run_and_shoot_install( )
{
	return HaruHook::install( g_a, 0x2523DF1, 7, reinterpret_cast< void* >( run_shoot_mid_stub_a ), &run_shoot_mid_resume_a )
		&& HaruHook::install( g_b, 0x36B263D, 7, reinterpret_cast< void* >( run_shoot_mid_stub_b ), &run_shoot_mid_resume_b );
}

void IceBox::run_and_shoot_uninstall( )
{
	HaruHook::remove( g_a );
	HaruHook::remove( g_b );
}

bool IceBox::run_and_shoot_installed( )
{
	return g_a.installed( ) && g_b.installed( );
}

#else

bool IceBox::run_and_shoot_install( )
{
	return true;
}
void IceBox::run_and_shoot_uninstall( ) { }
bool IceBox::run_and_shoot_installed( )
{
	return false;
}

#endif
