#include "../../../IceBox.hpp"
#include "../../../../Resources/config.hpp"

auto IceBox::self_revive( bool enable ) -> void {

	if ( !enable )
		return;

	auto local_pawn = Scimitar::game_manager::get( )->get_local_controller( )->pawn_decrypt( );

	if ( GetAsyncKeyState( visuals::SelfReviveVk ) & 1 ) {

		local_pawn->pawn_action( 6 );
		local_pawn->pawn_action( 0 );

	}
}