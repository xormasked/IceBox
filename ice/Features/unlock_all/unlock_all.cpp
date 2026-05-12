#include "../../IceBox.hpp"

#ifdef _WIN64

#include "../../../Core/Utils/Haru Hook/mid_hook.hpp"
#include "../../../Core/Utils/memory.hpp"

extern "C" {
	extern unsigned long long unlock_all_mid_resume;
	void unlock_all_mid_stub( );
}

namespace {

	mid_hook::MidHook g_hook;

} // namespace

bool IceBox::unlock_all_install( )
{
	return mid_hook::midhook(
		g_hook,
		Memory::ImageBase + 0x3843080,
		7,
		reinterpret_cast< void* >( unlock_all_mid_stub ),
		reinterpret_cast< std::uint64_t* >( &unlock_all_mid_resume ) );
}

void IceBox::unlock_all_uninstall( )
{
	mid_hook::midhook_remove( g_hook );
}

bool IceBox::unlock_all_installed( )
{
	return g_hook.installed( );
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
