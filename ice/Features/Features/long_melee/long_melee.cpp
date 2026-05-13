#include "../../../IceBox.hpp"
#include "../../../../Core/Engine/Anvil/scimitar.hpp"

auto IceBox::long_melee( bool enable ) -> void
{
	auto EDrvMelee = Scimitar::game_manager::get( )->get_local_controller( )->pawn_decrypt( )->entity_decrypt( )->resolve_component( Scimitar::EDrvMelee );

	if ( enable )
	{
		Memory::Write< float >( EDrvMelee + 0x60, 1000.0f );
		Memory::Write< float >( EDrvMelee + 0x64, 1000.0f );
	}
	else
	{
		Memory::Write< float >( EDrvMelee + 0x60, 1.2f );
		Memory::Write< float >( EDrvMelee + 0x64, 1.2f );
	}
}
