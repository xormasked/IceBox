#include "../../../IceBox.hpp"
#include "../../../../Core/Engine/Anvil/scimitar.hpp"
#include "../../../../Core/Engine/Anvil/Raycasting/RayCasting.hpp"
#include "../../../../Core/Engine/Anvil/skeletons.h"
#include "../../../../Resources/config.hpp"

#include <cmath>
#include <cstdio>

namespace {

	ubiVector3 g_hit{};
	float g_hit_ttl = 0.f;

	bool finite_xyz( const havok::Vec4& v )
	{
		return std::isfinite( v.x ) && std::isfinite( v.y ) && std::isfinite( v.z ) && ( v.x || v.y || v.z );
	}

} // namespace

auto IceBox::raycast_debug_hit_marker_decay( float dt ) -> void
{
	if ( g_hit_ttl <= 0.f ) return;
	g_hit_ttl -= dt;
	if ( g_hit_ttl < 0.f ) g_hit_ttl = 0.f;
}

bool IceBox::raycast_debug_hit_marker_world( ubiVector3& out_world ) noexcept
{
	if ( g_hit_ttl <= 0.f ) return false;
	out_world = g_hit;
	return true;
}

auto IceBox::raycast_closest_player_debug_tick( float ) -> void
{
	if ( !visuals::RaycastClosestDebug ||
	     ( !Scimitar::round_state::CurrentState( Scimitar::round_state::Prep ) &&
	         !Scimitar::round_state::CurrentState( Scimitar::round_state::Action ) ) )
		return;

	auto* gm = Scimitar::game_manager::get( );
	auto* lc = gm ? gm->get_local_controller( ) : nullptr;
	if ( !Memory::valid_pointer( lc ) ) return;

	auto* lp = lc->pawn_decrypt( );
	auto* le = lp && Memory::valid_pointer( lp ) ? lp->entity_decrypt( ) : nullptr;
	auto* ls = le && Memory::valid_pointer( le ) ? le->get_skeleton( ) : nullptr;
	if ( !Memory::valid_pointer( ls ) ) return;

	const havok::Vec4 lh = ls->bone( BipedBoneID::BONE_HEAD );
	if ( !finite_xyz( lh ) ) return;

	const ubiVector4 src( lh.x, lh.y, lh.z, 1.f );
	auto* cl = gm->get_controller_list( );
	int nc = gm->get_controller_size( );
	if ( !Memory::valid_pointer( cl ) || nc <= 0 ) return;
	if ( nc > 256 ) nc = 256;

	float best = 1e30f;
	ubiVector4 tgt( 0.f, 0.f, 0.f, 1.f );
	bool ok = false;

	for ( int i = 0; i < nc; ++i ) {
		auto* oc = *reinterpret_cast< Scimitar::Controller** >( reinterpret_cast< uintptr_t >( cl ) + uintptr_t( i ) * 8 );
		if ( !oc || oc == lc ) continue;
		auto* op = oc->pawn_decrypt( );
		auto* oe = op ? op->entity_decrypt( ) : nullptr;
		auto* sk = oe ? oe->get_skeleton( ) : nullptr;
		if ( !sk ) continue;
		const havok::Vec4 h = sk->bone( BipedBoneID::BONE_HEAD );
		if ( !finite_xyz( h ) ) continue;
		const float dx = h.x - lh.x, dy = h.y - lh.y, dz = h.z - lh.z;
		const float d2 = dx * dx + dy * dy + dz * dz;
		if ( d2 < best ) {
			best = d2;
			tgt = ubiVector4( h.x, h.y, h.z, 1.f );
			ok = true;
		}
	}
	if ( !ok ) return;

	ubiVector3 hit{};
	if ( __RaycastData::is_visible( src, tgt, true, &hit ) ) return;

	g_hit = hit;
	g_hit_ttl = 0.5f;
	std::printf( "[Raycast] hit %.2f %.2f %.2f | target %.2f %.2f %.2f\n", hit.x, hit.y, hit.z, tgt.x, tgt.y, tgt.z );
	std::fflush( stdout );
}
