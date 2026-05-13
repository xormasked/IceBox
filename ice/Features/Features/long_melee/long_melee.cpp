#include "../../../IceBox.hpp"
#include "../../../../Core/Engine/Anvil/scimitar.hpp"
#include "../../../../Core/Utils/memory.hpp"
#include "../../../../Resources/config.hpp"

namespace {

	bool resolve_edrv_melee( uintptr_t& out_edrv )
	{
		auto* const gm = Scimitar::game_manager::get( );
		if ( !Memory::valid_pointer( gm ) )
			return false;
		auto* const lc = gm->get_local_controller( );
		if ( !Memory::valid_pointer( lc ) )
			return false;
		auto* const lp = lc->pawn_decrypt( );
		if ( !Memory::valid_pointer( lp ) )
			return false;
		auto* const le = lp->entity_decrypt( );
		if ( !Memory::valid_pointer( le ) )
			return false;
		out_edrv = le->resolve_component( Scimitar::EDrvMelee );
		return Memory::valid_pointer( reinterpret_cast< const void* >( out_edrv ) );
	}

	void apply_long_melee_distance( uintptr_t edrv_melee, bool enable )
	{
		if ( enable ) {
			Memory::Write< float >( edrv_melee + 0x60, 1000.0f );
			Memory::Write< float >( edrv_melee + 0x64, 1000.0f );
		}
		else {
			Memory::Write< float >( edrv_melee + 0x60, 1.2f );
			Memory::Write< float >( edrv_melee + 0x64, 1.2f );
		}
	}

} // namespace

auto IceBox::long_melee( bool enable ) -> void
{
	uintptr_t edrv { };
	if ( !resolve_edrv_melee( edrv ) )
		return;
	apply_long_melee_distance( edrv, enable );
}

void IceBox::long_melee_prepare_uninject( ) noexcept
{
	visuals::LongMelee = false;
	uintptr_t edrv { };
	if ( !resolve_edrv_melee( edrv ) )
		return;
	apply_long_melee_distance( edrv, false );
}
