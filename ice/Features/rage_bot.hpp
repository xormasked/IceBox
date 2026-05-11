#pragma once

#include "../../Core/Engine/Anvil/scimitar.hpp"
#include "../../Core/Engine/Anvil/Raycasting/RayCasting.hpp"
#include "../../Resources/config.hpp"

#include <Windows.h>
#include <cmath>

namespace rage_bot {

    inline const BipedBoneID bones[ ] = {
        BipedBoneID::BONE_NECK,
        BipedBoneID::BONE_SPINE,
        BipedBoneID::BONE_SPINE2,
        BipedBoneID::BONE_SPINE1,
        BipedBoneID::BONE_LEFTSHOULDER,
        BipedBoneID::BONE_LEFTARM,
        BipedBoneID::BONE_LEFTELBOW,
        BipedBoneID::BONE_LEFTHAND,
        BipedBoneID::BONE_RIGHTSHOULDER,
        BipedBoneID::BONE_RIGHTARM,
        BipedBoneID::BONE_RIGHTELBOW,
        BipedBoneID::BONE_RIGHTHAND,
        BipedBoneID::BONE_LHIP,
        BipedBoneID::BONE_LKNEE,
        BipedBoneID::BONE_LFOOT,
        BipedBoneID::BONE_RHIP,
        BipedBoneID::BONE_RKNEE,
        BipedBoneID::BONE_RFOOT
    };

    inline bool valid_bone_position( const havok::Vec4& p )
    {
        return std::isfinite( p.x ) && std::isfinite( p.y ) && std::isfinite( p.z ) &&
            ( p.x != 0.0f || p.y != 0.0f || p.z != 0.0f );
    }

    inline void run( bool enable )
    {
        if ( !enable ) return;

        auto* game_manager = Scimitar::game_manager::get( );

        auto* local_controller = game_manager->get_local_controller( );
        auto* local_skeleton = local_controller->pawn_decrypt( )->entity_decrypt( )->get_skeleton( );

        auto* weap = local_controller->get_current_weapon( );
        if ( !Memory::valid_pointer( weap ) ) return;

        const havok::Vec4 local_head = local_skeleton->bone( BipedBoneID::BONE_HEAD );
        if ( !valid_bone_position( local_head ) ) return;
        const ubiVector4 shot_source( local_head.x, local_head.y, local_head.z, 1.0f );

        auto* controller_list = game_manager->get_controller_list( );
        int controller_count = game_manager->get_controller_size( );
        if ( !Memory::valid_pointer( controller_list ) || controller_count <= 0 ) return;
        if ( controller_count > 256 ) controller_count = 256;

        static DWORD last_shot_tick = 0;
        const DWORD now = GetTickCount( );
        constexpr DWORD kMinShotIntervalMs = 35;
        if ( now - last_shot_tick < kMinShotIntervalMs ) return;

        for ( int i = 0; i < controller_count; ++i ) {
            auto* controller = *reinterpret_cast< Scimitar::Controller** >(
                reinterpret_cast< uintptr_t >( controller_list ) + ( 0x8 * i ) );
            if ( !Memory::valid_pointer( controller ) || controller == local_controller ) continue;

            auto* pawn = controller->pawn_decrypt( );
            if ( !Memory::valid_pointer( pawn ) ) continue;
            auto* entity = pawn->entity_decrypt( );
            if ( !Memory::valid_pointer( entity ) ) continue;

            auto* skeleton = entity->get_skeleton( );
            if ( !Memory::valid_pointer( skeleton ) ) continue;

            for ( const auto bone_id : bones ) {
                const havok::Vec4 bone_pos = skeleton->bone( bone_id );
                if ( !valid_bone_position( bone_pos ) ) continue;

                const ubiVector4 shot_destination( bone_pos.x, bone_pos.y, bone_pos.z, 1.0f );

                if ( __RaycastData::is_visible( shot_source, shot_destination ) ) {
                    weap->create_bullet( shot_source, shot_destination );
                    last_shot_tick = now;
                    return;
                }
            }
        }
    }
}
