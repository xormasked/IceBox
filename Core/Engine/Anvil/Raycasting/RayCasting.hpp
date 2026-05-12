#pragma once

#include "../../../Maths/havok_math.hpp"
#include "../../../Utils/memory.hpp"
#include "../../../Utils/definitions.hpp"

#include <cmath>
#include <cstring>

namespace __RaycastData {

    inline std::uintptr_t rva( std::uintptr_t off ) noexcept
    {
        return reinterpret_cast< std::uintptr_t >( ::GetModuleHandleA( nullptr ) ) + off;
    }

    inline bool vp( std::uintptr_t p ) noexcept
    {
        return Memory::valid_pointer( reinterpret_cast< void* >( p ) );
    }

    inline std::uintptr_t physic_world( ) noexcept
    {
        std::uintptr_t m = *reinterpret_cast< std::uintptr_t* >( rva( 0x5B1F818 ) );
        if ( !vp( m ) ) return 0;
        std::uintptr_t w = *reinterpret_cast< std::uintptr_t* >( m + 0x20 );
        if ( !vp( w ) ) return 0;
        std::uintptr_t p = *reinterpret_cast< std::uintptr_t* >( w + 0xE8 );
        return vp( p ) ? p : 0;
    }

    using ray_fn = bool( __fastcall* )( std::uintptr_t, void*, ubiVector4, ubiVector4, __int64* );
    using clean_fn = void( __fastcall* )( void* );

    inline ray_fn ray_cast( ) noexcept
    {
        return reinterpret_cast< ray_fn >( rva( 0x1244E40 ) );
    }
    inline clean_fn ray_clean( ) noexcept
    {
        return reinterpret_cast< clean_fn >( rva( 0x11A4700 ) );
    }

    inline void dbg_first_hit( bool dbg, ubiVector3* out, __int64* rd ) noexcept
    {
        if ( !dbg || !out || !( rd[ 17 ] & 0x3FFFFFFF ) ) return;
        std::uintptr_t e = static_cast< std::uintptr_t >( rd[ 16 ] );
        if ( !vp( e ) || !vp( e + 0x2F ) ) return;
        float x = *reinterpret_cast< float* >( e + 0x20 ), y = *reinterpret_cast< float* >( e + 0x24 ), z = *reinterpret_cast< float* >( e + 0x28 );
        if ( std::isfinite( x ) && std::isfinite( y ) && std::isfinite( z ) ) {
            out->x = x;
            out->y = y;
            out->z = z;
        }
    }

    inline bool is_visible( ubiVector4 start, ubiVector4 end, bool debug = false, ubiVector3* out_hit = nullptr ) noexcept
    {
        std::uintptr_t pw = physic_world( );
        if ( !pw ) return false;
        __int64 b[ 29 ];
        memset( b, 0, 0xD0ui64 );
        LODWORD( b[ 2 ] ) = 0x126045;
        WORD2( b[ 2 ] ) = 0x100;
        LOBYTE( b[ 14 ] ) = 0x1;
        *( uint8_t* ) ( b + 0x15 ) = false;
        ray_cast( )( pw, b, start, end, &b[ 15 ] );
        uint32_t n = static_cast< uint32_t >( b[ 17 ] & 0x3FFFFFFF );
        dbg_first_hit( debug, out_hit, b );
        ray_clean( )( &b[ 15 ] );
        return n == 0;
    }

    inline __int64 is_penetrable( ubiVector4 src, ubiVector4 dst, bool debug = false, ubiVector3* out_hit = nullptr ) noexcept
    {
        try {
            std::uintptr_t pw = physic_world( );
            if ( !pw ) return 1LL;
            __int64 r[ 29 ];
            memset( r, 0, 0xD0ui64 );
            LODWORD( r[ 2 ] ) = 35665087;
            WORD2( r[ 2 ] ) = 256;
            LOBYTE( r[ 14 ] ) = 32;
            LOBYTE( r[ 21 ] ) = 0;
            *( uint8_t* ) ( r + 0x15 ) = false;

            auto finish = [ & ] ( __int64 x ) -> __int64 {
                dbg_first_hit( debug, out_hit, r );
                ray_clean( )( &r[ 15 ] );
                return x;
                };

            if ( !ray_cast( )( pw, r, dst, src, &r[ 15 ] ) ) return finish( 3LL );
            uint64_t n = r[ 17 ] & 0x3FFFFFFF;
            if ( !n ) return finish( 3LL );
            if ( !LOBYTE( r[ 18 ] ) ) return finish( 0LL );
            if ( n == ( uint64_t ) LOBYTE( r[ 14 ] ) ) return finish( 1LL );

            std::uintptr_t ha = static_cast< std::uintptr_t >( r[ 16 ] );
            if ( !vp( ha ) ) return finish( 1LL );

            float dmg = 99999.f;
            __int64 res = 1LL;
            const __int64 sz = 80LL * static_cast< __int64 >( n );
            for ( __int64 o = 0; o < sz; o += 80LL ) {
                const std::uintptr_t eb = ha + static_cast< std::uintptr_t >( o );
                if ( !vp( eb ) || !vp( eb + 8 ) ) continue;
                uint64_t ho = *reinterpret_cast< uint64_t* >( eb );
                if ( !vp( ho ) || !*reinterpret_cast< uint64_t* >( ho ) ) continue;
                uint64_t hi = *reinterpret_cast< uint64_t* >( eb + 8 );
                if ( !vp( hi ) ) continue;
                uint64_t mat = *reinterpret_cast< uint64_t* >( hi );
                if ( !vp( mat ) || !vp( mat + 0x38 ) ) continue;
                uint64_t sf = *reinterpret_cast< uint64_t* >( mat + 0x38 );
                if ( !vp( sf ) ) continue;
                uint64_t sd = *reinterpret_cast< uint64_t* >( sf );
                if ( !vp( sd ) || !vp( sd + 0x18 ) || !vp( sd + 0x24 ) ) continue;
                if ( *reinterpret_cast< uint8_t* >( sd + 0x24 ) == 255 ) break;
                if ( *reinterpret_cast< float* >( sd + 0x18 ) >= 90.f ) break;
                dmg -= *reinterpret_cast< uint8_t* >( sd + 0x24 );
                if ( dmg < 0.f ) break;
                if ( o + 80LL == sz ) {
                    res = 3LL;
                    break;
                }
            }
            return finish( res );
        }
        catch ( ... ) {
            return 1LL;
        }
    }

}
