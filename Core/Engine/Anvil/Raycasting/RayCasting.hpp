#pragma once

#include "../../../Maths/havok_math.hpp"
#include "../../../Utils/memory.hpp"
#include "../../../Utils/definitions.hpp"

#include <cstring>
#include <type_traits>
#include <vector>

#include "../scimitar.hpp"


#define decl_rva(address) reinterpret_cast<void*>(std::uintptr_t(::GetModuleHandleA(nullptr)) + (address))



template<typename t>
inline static auto valid_pointer( t pointer ) -> bool
{
    std::uintptr_t raw = 0;
    if constexpr ( std::is_pointer_v<t> )
        raw = reinterpret_cast< std::uintptr_t >( pointer );
    else
        raw = static_cast< std::uintptr_t >( pointer );
    return raw > 0x400000ULL && raw < 0x7FFFFFFFFFFFULL;
}

namespace __RaycastData {

    inline static auto is_visible( ubiVector4 start, ubiVector4 end ) -> bool
    {
        auto manager = *reinterpret_cast< std::uintptr_t* >( decl_rva( 0x5B1F818 ) );
        if ( !valid_pointer( manager ) )
            return false;

        auto world = *reinterpret_cast< std::uintptr_t* >( manager + 0x20 );
        if ( !valid_pointer( world ) )
            return false;

        auto physic_world = *reinterpret_cast< std::uintptr_t* >( world + 0xE8 );
        if ( !valid_pointer( physic_world ) )
            return false;

        bool( __fastcall * fnRayCast )( const std::uintptr_t PhysicWorld, void* a2, ubiVector4 a3, ubiVector4 a4, __int64* a5 ) = reinterpret_cast< decltype( fnRayCast ) >( decl_rva( 0x1244E40 ) );
        void( __fastcall * CleanUp )( void* a1 ) = ( decltype( CleanUp ) ) ( decl_rva( 0x11A4700 ) );

        __int64 v27[ 29 ];

        memset( v27, 0, 0xD0ui64 );
        LODWORD( v27[ 2 ] ) = 0x126045;
        WORD2( v27[ 2 ] ) = 0x100;
        LOBYTE( v27[ 14 ] ) = 0x1;

        *( uint8_t* ) ( v27 + 0x15 ) = false;
        auto hm = fnRayCast( physic_world, v27, start, end, &v27[ 15 ] );

        auto count = ( v27[ 17 ] & 0x3FFFFFFF );

        CleanUp( &v27[ 15 ] );

        return count == 0;
    }


    static auto is_penetrable( ubiVector4 src, ubiVector4 dst ) -> __int64
    {
        try
        {
            auto manager = *reinterpret_cast< std::uintptr_t* >( Memory::ImageBase + 0x5B1F818 );
            if ( !valid_pointer( manager ) )
                return 1LL;

            auto world = *reinterpret_cast< std::uintptr_t* >( manager + 0x20 );
            if ( !valid_pointer( world ) )
                return 1LL;

            auto physicsWorld = *reinterpret_cast< std::uintptr_t* >( world + 0xE8 );
            if ( !valid_pointer( physicsWorld ) )
                return 1LL;

            auto fnRayCast = reinterpret_cast< bool( __fastcall* )( std::uintptr_t, void*, ubiVector4, ubiVector4, __int64* ) >( decl_rva( 0x1244E40 ) );

            auto fnCleanUp = reinterpret_cast< void( __fastcall* )( void* ) >( decl_rva( 0x11A4700 ) );

            __int64 rayData[ 29 ];

            memset( rayData, 0, 0xD0ui64 );
            LODWORD( rayData[ 2 ] ) = 35665087;
            WORD2( rayData[ 2 ] ) = 256;
            LOBYTE( rayData[ 14 ] ) = 32;
            LOBYTE( rayData[ 21 ] ) = 0;

            *( uint8_t* ) ( rayData + 0x15 ) = false;

            if ( !fnRayCast( physicsWorld, rayData, dst, src, &rayData[ 15 ] ) )
            {
                fnCleanUp( &rayData[ 15 ] );
                return 3LL;
            }

            const uint64_t hitCount = rayData[ 17 ] & 0x3FFFFFFF;
            if ( !hitCount )
            {
                fnCleanUp( &rayData[ 15 ] );
                return 3LL;
            }

            if ( !LOBYTE( rayData[ 18 ] ) )
            {
                fnCleanUp( &rayData[ 15 ] );
                return 0LL;
            }

            if ( hitCount == ( uint64_t ) LOBYTE( rayData[ 14 ] ) )
            {
                fnCleanUp( &rayData[ 15 ] );
                return 1LL;
            }

            auto hitArray = rayData[ 16 ];
            if ( !valid_pointer( ( void* ) hitArray ) )
            {
                fnCleanUp( &rayData[ 15 ] );
                return 1LL;
            }

            float remainingDamage = 99999.0f;
            __int64 result = 1LL;
            const __int64 hitArraySize = 80LL * hitCount;

            for ( __int64 offset = 0; offset < hitArraySize; offset += 80LL )
            {
                auto entryBase = hitArray + offset;
                if ( !valid_pointer( ( void* ) entryBase ) || !valid_pointer( ( void* ) ( entryBase + 0x8 ) ) )
                    continue;

                auto hitObject = *reinterpret_cast< uint64_t* >( entryBase );
                if ( !valid_pointer( ( void* ) hitObject ) || !*reinterpret_cast< _QWORD* >( hitObject ) )
                    continue;

                auto hitInfo = *reinterpret_cast< uint64_t* >( entryBase + 0x8 );
                if ( !valid_pointer( ( void* ) hitInfo ) )
                    continue;

                auto material = *reinterpret_cast< uint64_t* >( hitInfo );
                if ( !valid_pointer( ( void* ) material ) || !valid_pointer( ( void* ) ( material + 0x38 ) ) )
                    continue;

                auto surface = *reinterpret_cast< uint64_t* >( material + 0x38 );
                if ( !valid_pointer( ( void* ) surface ) )
                    continue;

                auto surfaceData = *reinterpret_cast< uint64_t* >( surface );
                if ( !valid_pointer( ( void* ) surfaceData ) )
                    continue;

                if ( !valid_pointer( ( void* ) ( surfaceData + 0x18 ) ) || !valid_pointer( ( void* ) ( surfaceData + 0x24 ) ) )
                    continue;

                const unsigned __int8 penetrationLoss = *reinterpret_cast< unsigned __int8* >( surfaceData + 0x24 );
                if ( penetrationLoss == 255 )
                    break;

                if ( *reinterpret_cast< float* >( surfaceData + 0x18 ) >= 90.0f )
                    break;

                remainingDamage -= penetrationLoss;
                if ( remainingDamage < 0.0f )
                    break;

                if ( offset + 80 == hitArraySize )
                {
                    result = 3LL;
                    break;
                }
            }

            fnCleanUp( &rayData[ 15 ] );
            return result;
        }
        catch ( ... )
        {
            return 1LL;
        }
    }

}