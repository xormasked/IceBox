#include "../../../IceBox.hpp"

#ifdef _WIN64

#include "../../../../Core/Utils/Haru Hook/haru_hook_install.hpp"
#include "../../../../Resources/config.hpp"

extern "C" {
extern float world_edit_params[ 4 ];
extern unsigned long long world_edit_mid_resume_storage;
void world_edit_mid_stub( );
}

namespace {
mid_hook::MidHook g;
}

bool IceBox::world_edit_install( )
{
	return HaruHook::install( g, 0xC3CBB4, 8, reinterpret_cast<void*>( world_edit_mid_stub ),
	    reinterpret_cast<std::uint64_t*>( &world_edit_mid_resume_storage ) );
}

void IceBox::world_edit_uninstall( )
{
	HaruHook::remove( g );
}

bool IceBox::world_edit_installed( )
{
	return g.installed( );
}

void IceBox::world_edit_tick( ) noexcept
{
	if ( !g.installed( ) )
		return;
	world_edit_params[ 0 ] = world_edit::clarity;
	world_edit_params[ 1 ] = world_edit::saturation;
	world_edit_params[ 2 ] = world_edit::brightness;
	world_edit_params[ 3 ] = world_edit::illumination;
}

void IceBox::world_edit_prepare_uninject( ) noexcept
{
	world_edit_uninstall( );
}

#else

bool IceBox::world_edit_install( )
{
	return true;
}

void IceBox::world_edit_uninstall( )
{
}

bool IceBox::world_edit_installed( )
{
	return false;
}

void IceBox::world_edit_tick( ) noexcept
{
}

void IceBox::world_edit_prepare_uninject( ) noexcept
{
}

#endif
