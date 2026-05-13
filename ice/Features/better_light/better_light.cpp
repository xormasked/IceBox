#include "../../IceBox.hpp"

#ifdef _WIN64

#include "../../../Core/Utils/Haru Hook/mid_hook.hpp"
#include "../../../Core/Utils/memory.hpp"

extern "C" {
extern unsigned long long better_light_resume_normal;
extern unsigned long long better_light_branch_taken;
void better_light_mid_stub( );
}

namespace {

	constexpr std::uintptr_t k_hook_rva = 0xFB0664;
	constexpr std::uintptr_t k_resume_rva = 0xFB066E;
	constexpr std::uintptr_t k_branch_rva = 0xFB07D4;

	mid_hook::MidHook g_hook;

} // namespace

bool IceBox::better_light_install( )
{
	better_light_resume_normal = Memory::ImageBase + k_resume_rva;
	better_light_branch_taken = Memory::ImageBase + k_branch_rva;
	return mid_hook::midhook( g_hook, Memory::ImageBase + k_hook_rva, 10, reinterpret_cast< void* >( better_light_mid_stub ), nullptr );
}

void IceBox::better_light_uninstall( )
{
	mid_hook::midhook_remove( g_hook );
}

bool IceBox::better_light_installed( )
{
	return g_hook.installed( );
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
