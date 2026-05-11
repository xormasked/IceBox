#include "../../IceBox.hpp"

#ifdef _WIN64

#include "../../../Core/Utils/Haru Hook/mid_hook.hpp"
#include "../../../Core/Utils/memory.hpp"

extern "C" {
extern unsigned long long aspect_mid_resume_storage;
void aspect_mid_stub( );
}

namespace {

	constexpr std::uintptr_t k_rva = 0xB2FA0E;

	mid_hook::MidHook g_hook;

} // namespace

bool IceBox::aspect_ratio_install( )
{
	return mid_hook::midhook( g_hook, Memory::ImageBase + k_rva, 8, reinterpret_cast< void* >( aspect_mid_stub ),
		reinterpret_cast< std::uint64_t* >( &aspect_mid_resume_storage ) );
}

void IceBox::aspect_ratio_uninstall( )
{
	mid_hook::midhook_remove( g_hook );
}

bool IceBox::aspect_ratio_installed( )
{
	return g_hook.installed( );
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
