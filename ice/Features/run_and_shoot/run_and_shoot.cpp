#include "../../IceBox.hpp"

#ifdef _WIN64

#include "../../../Core/Utils/Haru Hook/mid_hook.hpp"
#include "../../../Core/Utils/memory.hpp"

extern "C" {
extern unsigned long long run_shoot_mid_resume_a;
extern unsigned long long run_shoot_mid_resume_b;
void run_shoot_mid_stub_a( );
void run_shoot_mid_stub_b( );
}

namespace {

	mid_hook::MidHook g_hook_a;
	mid_hook::MidHook g_hook_b;

} // namespace

bool IceBox::run_and_shoot_install( )
{
	const bool a = mid_hook::midhook
	(
		g_hook_a,
		Memory::ImageBase + 0x2523DF1,
		7,
		reinterpret_cast< void* >( run_shoot_mid_stub_a ),
		reinterpret_cast< std::uint64_t* >( &run_shoot_mid_resume_a )
	);


	const bool b = mid_hook::midhook
	(
		g_hook_b,
		Memory::ImageBase + 0x36B263D,
		7,
		reinterpret_cast< void* >( run_shoot_mid_stub_b ),
		reinterpret_cast< std::uint64_t* >( &run_shoot_mid_resume_b )
	);

	return a && b;
}

void IceBox::run_and_shoot_uninstall( )
{
	mid_hook::midhook_remove( g_hook_a );
	mid_hook::midhook_remove( g_hook_b );
}

bool IceBox::run_and_shoot_installed( )
{
	return g_hook_a.installed( ) && g_hook_b.installed( );
}

#else

bool IceBox::run_and_shoot_install( )
{
	return true;
}

void IceBox::run_and_shoot_uninstall( )
{
}

bool IceBox::run_and_shoot_installed( )
{
	return false;
}

#endif
