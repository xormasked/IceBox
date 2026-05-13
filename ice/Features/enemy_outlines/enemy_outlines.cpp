#include "../../IceBox.hpp"

#include "../../../Core/Engine/Anvil/scimitar.hpp"
#include "../../../Core/Maths/havok_math.hpp"
#include "../../../Core/Utils/memory.hpp"
#include "../../../Resources/config.hpp"

namespace {

	inline void iterate_outlines( bool enabled, int bgra ) noexcept
	{
		auto* const gm = Scimitar::game_manager::get( );
		if ( !Memory::valid_pointer( gm ) )
			return;

		auto* const lc = gm->get_local_controller( );
		if ( !Memory::valid_pointer( lc ) )
			return;

		Scimitar::Controller* const list = gm->get_controller_list( );
		if ( !Memory::valid_pointer( list ) )
			return;

		int n = gm->get_controller_size( );
		if ( n > 256 )
			n = 256;

		const std::uintptr_t list_u = reinterpret_cast< std::uintptr_t >( list );

		for ( int i = 0; i < n; ++i ) {
			auto* const oc = *reinterpret_cast< Scimitar::Controller** >(
			    list_u + static_cast< std::uintptr_t >( i ) * 8u );
			if ( !oc || oc == lc )
				continue;

			auto* const op = oc->pawn_decrypt( );
			if ( !Memory::valid_pointer( op ) )
				continue;
			auto* const ent = op->entity_decrypt( );
			if ( !Memory::valid_pointer( ent ) )
				continue;

			ent->apply_mesh_outline( enabled, bgra );
		}
	}

} // namespace

void IceBox::enemy_outlines_tick( )
{
	static bool s_was_on = false;
	const bool on = visuals::EnemyOutline;

	if ( !on ) {
		if ( s_was_on ) {
			iterate_outlines( false, 0 );
			s_was_on = false;
		}
		return;
	}

	s_was_on = true;
	const int bgra = havok::color_float_to_bgra(
	    visuals::EnemyOutlineColor.x,
	    visuals::EnemyOutlineColor.y,
	    visuals::EnemyOutlineColor.z,
	    visuals::EnemyOutlineColor.w );
	iterate_outlines( true, bgra );
}

void IceBox::enemy_outlines_prepare_uninject( ) noexcept
{
	iterate_outlines( false, 0 );
}
