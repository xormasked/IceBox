#pragma once

#include "../../Core/Engine/Anvil/scimitar.hpp"
#include "../../Core/Engine/Anvil/Raycasting/RayCasting.hpp"
#include "../../Resources/config.hpp"

#include <cmath>

namespace rage_bot {

    inline constexpr BipedBoneID k_bones[ ] = {
        BipedBoneID::BONE_HEAD,
        BipedBoneID::BONE_NECK,
        BipedBoneID::BONE_SPINE2,
        BipedBoneID::BONE_SPINE1,
        BipedBoneID::BONE_SPINE,
        BipedBoneID::BONE_LEFTSHOULDER,
        BipedBoneID::BONE_RIGHTSHOULDER,
    };

    inline bool bone_ok( const havok::Vec4& p )
    {
        return std::isfinite( p.x ) && std::isfinite( p.y ) && std::isfinite( p.z ) &&
            ( p.x != 0.f || p.y != 0.f || p.z != 0.f );
    }

    inline void run( bool enable )
    {
        if ( !enable ) return;
        if ( !Scimitar::round_state::CurrentState( Scimitar::round_state::Prep ) &&
             !Scimitar::round_state::CurrentState( Scimitar::round_state::Action ) )
            return;

        auto* const gm = Scimitar::game_manager::get( );
        auto* const lc = gm ? gm->get_local_controller( ) : nullptr;
        if ( !Memory::valid_pointer( lc ) ) return;

        auto* const lp = lc->pawn_decrypt( );
        auto* const le = lp && Memory::valid_pointer( lp ) ? lp->entity_decrypt( ) : nullptr;
        auto* const ls = le && Memory::valid_pointer( le ) ? le->get_skeleton( ) : nullptr;
        if ( !Memory::valid_pointer( ls ) ) return;

        auto* const weap = lc->get_current_weapon( );
        if ( !Memory::valid_pointer( weap ) ) return;

        const havok::Vec4 lh = ls->bone( BipedBoneID::BONE_HEAD );
        if ( !bone_ok( lh ) ) return;
        const ubiVector4 src( lh.x, lh.y, lh.z, 1.f );

        auto* const clist = gm->get_controller_list( );
        int n = gm->get_controller_size( );
        if ( !Memory::valid_pointer( clist ) || n <= 0 ) return;
        if ( n > 256 ) n = 256;

        const uintptr_t list_u = reinterpret_cast< uintptr_t >( clist );

        for ( int i = 0; i < n; ++i ) {
            auto* const oc = *reinterpret_cast< Scimitar::Controller** >( list_u + uintptr_t( i ) * 8 );
            if ( !Memory::valid_pointer( oc ) || oc == lc ) continue;

            auto* const op = oc->pawn_decrypt( );
            auto* const oe = op && Memory::valid_pointer( op ) ? op->entity_decrypt( ) : nullptr;
            auto* const sk = oe && Memory::valid_pointer( oe ) ? oe->get_skeleton( ) : nullptr;
            if ( !Memory::valid_pointer( sk ) ) continue;

            for ( const auto id : k_bones ) {
                const havok::Vec4 b = sk->bone( id );
                if ( !bone_ok( b ) ) continue;
                const ubiVector4 dst( b.x, b.y, b.z, 1.f );

                bool ok = true;
                if ( visuals::RageBotPenCheck )
                    ok = ( __RaycastData::is_penetrable( src, dst ) == 3LL );
                else if ( visuals::RageBotVisCheck )
                    ok = __RaycastData::is_visible( src, dst );

                if ( ok ) {
                    weap->create_bullet( src, dst );
                    return;
                }
            }
        }
    }

} // namespace rage_bot
