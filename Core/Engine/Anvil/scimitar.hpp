#pragma once


#include "../../Utils/memory.hpp"

#include "../../Utils/definitions.hpp"
#include "../../Maths/havok_math.hpp"

#include "skeletons.h"

#include <cstring>
#include <xmmintrin.h>

namespace Scimitar {

    enum EDESrvComponents : uint32_t
    {
        EDrvAnimation = 0x2800E489,
        EDrvSprinting = 0xDFD83971,
        EDrvWeapon = 0xA6F78616,
        EDrvMelee = 0xB1531297,
        EDrvShooting = 0xCE425D1A,
        EDrvDrone = 0x903F7852,
        EDrvGadget = 0x799B767A,
        EDrvLeaning = 0x2434CD8B,
        EDrvVaulting = 0xF6D74D80,
        EDrvContext = 0x5F2FB343,
        EDrvNavigation = 0x529E3224,
        EDrvInteraction = 0xBDF390BC,


        ESrvR6AIData = 0x229E5633,
        ESrvEffect = 0xB1059FBA,
        ESrvMobility = 0x948CF73,
        ESrvDebug = 0x646F16A3,
        ESrvGroup = 0x650A15C1,
        ESrvCoordinator = 0x9CFFF5DD,
        ESrvScripting = 0x8ABC2229,
        ESrvPerception = 0xCF133C7C,
        ESrvLocation = 0x818666A4,
        ESrvAbilities = 0x65EAE982,
        ESrvHostage = 0xA0C9A8AB,

        Unknown1 = 0xE7BB7DA4,
        Unknown7 = 0xDBE19682,
        Unknown10 = 0x8CB9FFB7
    };

    class game_manager;
    class Controller;

    inline Controller* LocalPlayer = 0x0;

    class Pawn;
    class Entity;
    class cskeleton;
    class animationcomponent;
    class ecomponents;
    class weaponcomponent;
    class chealdweapon;

    using GetSkeletonComponentFn = cskeleton * ( __fastcall* )( unsigned __int8* a1, Entity* a2 );
    using GetAnimationComponentFn = animationcomponent*( __fastcall* )( unsigned __int8* a1, Entity* a2 );
    using GetBoneFn = void( * )( cskeleton* a1, int a2, __m128* a3 );
    using CreateShotFn = __int64( __fastcall* )( __int64 Weapon_Infoa1, __m128* source, const __m128i* Direction );
    using EDSrvListFn = uintptr_t( __fastcall* )( unsigned __int8* a1, Entity* a2 );
    using ResolveComponentFn = uintptr_t( __fastcall* )( uintptr_t a1, unsigned int type_hash, int unk );
    using SetGlobalMatrixFn = __int64( __fastcall* )( __int64 entity, __m128* matrix, char one );

    inline GetSkeletonComponentFn get_skeleton_component = nullptr;
    inline GetAnimationComponentFn get_animation_component_fn = nullptr;
    inline GetBoneFn get_bone_pos = nullptr;
    inline CreateShotFn create_shot = nullptr;
    inline EDSrvListFn ed_srv_list = nullptr;
    inline ResolveComponentFn resolve_component_game = nullptr;
    inline SetGlobalMatrixFn set_global_matrix_D871E0 = nullptr;

    inline auto get_camera_fx( )
    {
        uint64_t Base = Memory::ImageBase + 0x5D7D6F0;
        std::vector<uintptr_t> Chain = { 0x300, 0xF90 };

        return Memory::ReadPtr<uint64_t>( Base, Chain );
    }

    inline uintptr_t get_camera_fov( )
    {
        uint64_t Base = Memory::ImageBase + 0x7EF0EB8;
        std::vector<uintptr_t> Chain = { 0x10 };
        const uint64_t ctx = Memory::ReadPtr<uint64_t>( Base, Chain );
        return ctx ? static_cast< uintptr_t >( ctx ) + 0xBB8 : 0;
    }

    inline uintptr_t get_viewmodel_fov( )
    {
        uint64_t Base = Memory::ImageBase + 0x7EF0EB8;
        std::vector<uintptr_t> Chain = { 0x10 };
        const uint64_t ctx = Memory::ReadPtr<uint64_t>( Base, Chain );
        return ctx ? static_cast< uintptr_t >( ctx ) + 0xBBC : 0;
    }

    class view_translation {
    public:
        static view_translation* get( )
        {
            uint64_t camera = Memory::Read<uint64_t>( Memory::ImageBase + 0x5E32C50 );
            camera = Memory::Read<uint64_t>( camera + 0x88 );
            camera = Memory::Read<uint64_t>( camera + 0x0 );
            camera = Memory::Read<uint64_t>( camera + 0x220 );
            camera = Memory::Read<uint64_t>( camera + 0x410 );

            return reinterpret_cast< view_translation* >( camera );
        }

        ubiVector3 get_view_right( )
        {
            return *reinterpret_cast< ubiVector3* >( this + 0x7A0 );
        }

        ubiVector3 get_view_up( )
        {
            return *reinterpret_cast< ubiVector3* >( this + 0x7B0 );
        }

        ubiVector3 get_view_forward( )
        {
            return *reinterpret_cast< ubiVector3* >( this + 0x7C0 );
        }

        float get_view_fovX( )
        {
            return *reinterpret_cast< float* >( this + 0x7E0 );
        }

        float get_view_fovY( )
        {
            return *reinterpret_cast< float* >( this + 0x7F4 );
        }

        ubiVector3 get_view_translation( )
        {
            return *reinterpret_cast< ubiVector3* >( this + 0x7D0 );
        }
    };

    class round_state {
    public:
        static int get( )
        {
            uint64_t RoundBase = Memory::ImageBase + 0x070A5EE8;
            std::vector<uintptr_t> RoundChain = { 0x300 };
            int roundstate = Memory::ReadPtr<int>( RoundBase, RoundChain );
            return roundstate;
        }

        enum RoundState : int
        {
            Loading = 1, // Loading screen
            Prep = 2, // Prep Phase
            Action = 3, // In Round
            End = 4, // End Screen
            Menu = 5 // Menu
        };

        static bool CurrentState( RoundState round )
        {
            return round_state::get( ) == ( int ) round;
        }
    };

    class game_manager {
    public:
        static game_manager* get( ) {
            return reinterpret_cast< game_manager* >( Memory::Read<uint64_t>( Memory::ImageBase + 0x5E019A8 ) );
        }

        auto get_controller_list( ) -> Controller* {

            uint64_t entityList = *( uint64_t* ) ( this + 0xB0 );
            entityList += 0x0FFFFFFFFFFFFFFCA;
            uint64_t entityList1 = entityList >> 0x11;
            uint64_t entityList2 = entityList << 0x2F;
            entityList = entityList1 | entityList2;
            entityList += 0x0FFFFFFFFFFFFFFA8;

            return reinterpret_cast< Controller* >( entityList );

        }

        int get_controller_size( ) {

            uint64_t entityCount = *( uint64_t* ) ( this + 0xB8 );
            entityCount += 0x0FFFFFFFFFFFFFFCA;
            uint64_t entityCount1 = entityCount >> 0x11;
            uint64_t entityCount2 = entityCount << 0x2F;
            entityCount = entityCount1 | entityCount2;
            entityCount += 0x0FFFFFFFFFFFFFFA8;
            int count = ( int ) ( entityCount ^ 0x18C0000000 );
            return count;

        }

        Controller* get_local_controller( )
        {
            return Memory::call_virtual<Controller*>( this, 0x28 );
        }
    };

    class Controller {
    public:
        Pawn* pawn_decrypt( )
        {
            return Memory::call_virtual<Pawn*>( this, 0x25 );
        }


        auto get_cam_component( )
        {
            return reinterpret_cast< uintptr_t* >( *( uint64_t* ) reinterpret_cast< uint64_t >( this + 0x90 ) + 0x67C );
        }


        auto get_current_weapon( ) -> chealdweapon*
        {
            if ( !this || !Memory::valid_pointer( this ) ) return 0;

            uint64_t weaponInfo = *( uint64_t* ) ( this + 0x90 );
            weaponInfo = Memory::Read<uint64_t>( weaponInfo + 0x70 );
            weaponInfo = Memory::Read<uint64_t>( weaponInfo + 0x288 );
            weaponInfo -= 0x34;
            weaponInfo ^= 0x4F;
            weaponInfo -= 0x76;
            return (chealdweapon*) weaponInfo;
        }

    };

    class Pawn {
    public:
        Entity* entity_decrypt( ) {
            return reinterpret_cast< Entity* >( *( uint64_t* ) ( this + 0x18 ) );
        }

        auto view_angles( ) -> ubiVector4 {
            return *reinterpret_cast< ubiVector4* >( *reinterpret_cast< uint64_t* >( this + 0x1270 ) + 0xC0 );
        }

        Entity* get_camera_entity( )
        {
            const uintptr_t p = Memory::ReadPtr<uintptr_t>( Memory::ImageBase + 0x6806380, { 0x60, 0x60 } );
            return reinterpret_cast< Entity* >( p );
        }
    };

    class Entity {
    public:
        BYTE GetAlliance( )
        {
            if ( !this ) return 6;

            uint64_t teamInfo = *( uint64_t* ) ( this + 0xD0 );
            teamInfo = Memory::Read<uint64_t>( teamInfo + 0x98 );
            teamInfo = __ROL8__( teamInfo, 0x26 );
            teamInfo += 0x60A7E4A6C2B31A41;
            teamInfo = __ROL8__( teamInfo, 0x38 );
            BYTE team = Memory::Read<BYTE>( teamInfo + 0x30 );

            return team;
        }


        ubiVector3 Origin( ) {
            return *reinterpret_cast< ubiVector3* >( this + 0x50 );
        }

        ubiVector4 Origin4( ) const
        {
            return *reinterpret_cast< ubiVector4* >( reinterpret_cast< uintptr_t >( this ) + 0x50 );
        }

        cskeleton* get_skeleton( )
        {
            return get_skeleton_component( reinterpret_cast< unsigned __int8* >( this + 0x1CB ), this );
        }

        auto get_Animation_component( ) -> animationcomponent*
        {
            if ( !Memory::valid_pointer( this ) || !get_animation_component_fn )
                return nullptr;

            return get_animation_component_fn(
                reinterpret_cast< unsigned __int8* >( this + 0x1C3 ),
                this );
        }

        uintptr_t resolve_component( uint64_t hash ) noexcept
        {

            const uintptr_t list = ed_srv_list( reinterpret_cast< unsigned __int8* >( reinterpret_cast< uintptr_t >( this ) + 0x1A9 ), this );

            const uintptr_t list_header = Memory::Read< uintptr_t >( *reinterpret_cast< uintptr_t* >( list + 0x48 ) + 0x110 );

            return Memory::Read< uintptr_t >( resolve_component_game( list_header, hash, 0 ) );

        }

        void set_origin( const ubiVector4& origin, char one = 1 )
        {
            if ( !Memory::valid_pointer( this ) || !set_global_matrix_D871E0 )
                return;

            std::memcpy( reinterpret_cast< void* >( reinterpret_cast< uintptr_t >( this ) + 0x50 ), &origin,
                         sizeof( ubiVector4 ) );

            alignas( 16 ) __m128 matrix[ 4 ];
            std::memcpy( matrix, reinterpret_cast< void* >( reinterpret_cast< uintptr_t >( this ) + 0x20 ),
                         sizeof( matrix ) );

            set_global_matrix_D871E0( reinterpret_cast< __int64 >( this ), matrix, one );

            std::memcpy( matrix, &origin, sizeof( ubiVector4 ) );
        }

    };

    class GroundNavContext {
    public:
    };

    class animationcomponent {
    };

    class cskeleton {
    public:
        auto bone( BipedBoneID b ) -> havok::Vec4
        {
            havok::Vec4 ret = {};
            if ( !Memory::valid_pointer( this ) || !get_bone_pos ) return ret;
            __m128 output;

            get_bone_pos( this, b, &output );

            ret.x = output.m128_f32[ 0 ];
            ret.y = output.m128_f32[ 1 ];
            ret.z = output.m128_f32[ 2 ];
            ret.w = output.m128_f32[ 3 ];
            return ret;
        }
    };



    class chealdweapon {
    public:

        void create_bullet( ubiVector4 src, ubiVector4 dst )
        {
            if ( !this ) return;

            ubiVector4 difference = src - dst;
            float hypothenuse = sqrtf( difference.x * difference.x + difference.y * difference.y + difference.z * difference.z );
            if ( hypothenuse < 0.0001f ) return;

            float inv_hypothenuse = 1.0f / hypothenuse;
            ubiVector4 result( difference.x * inv_hypothenuse, difference.y * inv_hypothenuse, difference.z * inv_hypothenuse, 0.0f );
            ubiVector4 direction( -result.x, -result.y, -result.z, 0.0f );


            __m128 source128 = _mm_setr_ps( src.x, src.y, src.z, src.w );
            __m128 direction128 = _mm_setr_ps( direction.x, direction.y, direction.z, direction.w );

            create_shot
            (
                reinterpret_cast< __int64 >( this ),
                &source128,
                reinterpret_cast< const __m128i* >( &direction128 )
            );
        }
    };

    inline void init( )
    {
        get_skeleton_component = reinterpret_cast< GetSkeletonComponentFn >( Memory::ImageBase + 0xD7E9D0 );
        get_animation_component_fn = reinterpret_cast< GetAnimationComponentFn >( Memory::ImageBase + 0xD7EF50 );
        get_bone_pos = reinterpret_cast< GetBoneFn >( Memory::ImageBase + 0x631830 );
        create_shot = reinterpret_cast< CreateShotFn >( Memory::ImageBase + 0x1C4E7B0 );
        ed_srv_list = reinterpret_cast< EDSrvListFn >( Memory::ImageBase + 0xD7EDF0 );
        resolve_component_game = reinterpret_cast< ResolveComponentFn >( Memory::ImageBase + 0x181A810 );
        set_global_matrix_D871E0 = reinterpret_cast< SetGlobalMatrixFn >( Memory::ImageBase + 0xD871E0 );
    }
}

