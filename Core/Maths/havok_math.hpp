#pragma once
#ifndef havok_math
#define havok_math

#include <iostream>
#include <Windows.h>
#include <vector>
#include "../Utils/memory.hpp"

#include <algorithm>

#define RVA(offset) (void*)(Memory::ImageBase + offset)

#define RVA_CALL(call, offset) call = reinterpret_cast<decltype(call)>(RVA(offset));

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define MEMBER(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

#define DEFINE_MEMBER(type, name, offset) \
	__forceinline auto name( ) const noexcept -> type { if ( !this ) return {}; return *reinterpret_cast< type* >( reinterpret_cast< uintptr_t >( this ) + offset ); } \
	__forceinline auto name( ) -> type&               { type crayon{}; if ( !this ) return crayon; return *reinterpret_cast< type* >( reinterpret_cast< uintptr_t >( this ) + offset ); }




class ubiQuaternion {
public:
    constexpr ubiQuaternion( float w = 1.f, float x = 0.f, float y = 0.f, float z = 0.f ) noexcept
        : w( w ), x( x ), y( y ), z( z ) {
    }

    constexpr ubiQuaternion operator+( const ubiQuaternion& other ) const noexcept {
        return ubiQuaternion( w + other.w, x + other.x, y + other.y, z + other.z );
    }

    constexpr ubiQuaternion operator-( const ubiQuaternion& other ) const noexcept {
        return ubiQuaternion( w - other.w, x - other.x, y - other.y, z - other.z );
    }

    constexpr ubiQuaternion operator*( float factor ) const noexcept {
        return ubiQuaternion( w * factor, x * factor, y * factor, z * factor );
    }

    constexpr ubiQuaternion operator*( const ubiQuaternion& other ) const noexcept {
        return ubiQuaternion(
            w * other.w - x * other.x - y * other.y - z * other.z,
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w
        );
    }

    ubiQuaternion& operator+=( const ubiQuaternion& other ) noexcept {
        w += other.w; x += other.x; y += other.y; z += other.z;
        return *this;
    }

    ubiQuaternion& operator-=( const ubiQuaternion& other ) noexcept {
        w -= other.w; x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }

    ubiQuaternion& operator*=( float factor ) noexcept {
        w *= factor; x *= factor; y *= factor; z *= factor;
        return *this;
    }

    ubiQuaternion& operator*=( const ubiQuaternion& other ) noexcept {
        *this = *this * other;
        return *this;
    }

    float w, x, y, z;
};

class ubiVector2 {
public:
    constexpr ubiVector2( float x = 0.f, float y = 0.f ) noexcept
        : x( x ), y( y ) {
    }

    constexpr ubiVector2 operator-( const ubiVector2& other ) const noexcept {
        return ubiVector2( x - other.x, y - other.y );
    }

    constexpr ubiVector2 operator+( const ubiVector2& other ) const noexcept {
        return ubiVector2( x + other.x, y + other.y );
    }

    constexpr ubiVector2 operator/( float factor ) const noexcept {
        return ubiVector2( x / factor, y / factor );
    }

    constexpr ubiVector2 operator*( float factor ) const noexcept {
        return ubiVector2( x * factor, y * factor );
    }

    ubiVector2& operator+=( const ubiVector2& other ) noexcept {
        x += other.x; y += other.y;
        return *this;
    }

    ubiVector2& operator-=( const ubiVector2& other ) noexcept {
        x -= other.x; y -= other.y;
        return *this;
    }

    ubiVector2& operator*=( float factor ) noexcept {
        x *= factor; y *= factor;
        return *this;
    }

    ubiVector2& operator/=( float factor ) noexcept {
        x /= factor; y /= factor;
        return *this;
    }

    float x, y;
};

class ubiVector3 {
public:
    constexpr ubiVector3( float x = 0.f, float y = 0.f, float z = 0.f ) noexcept
        : x( x ), y( y ), z( z ) {
    }

    constexpr ubiVector3 operator-( const ubiVector3& other ) const noexcept {
        return ubiVector3( x - other.x, y - other.y, z - other.z );
    }

    constexpr ubiVector3 operator+( const ubiVector3& other ) const noexcept {
        return ubiVector3( x + other.x, y + other.y, z + other.z );
    }

    constexpr ubiVector3 operator/( float factor ) const noexcept {
        return ubiVector3( x / factor, y / factor, z / factor );
    }

    constexpr ubiVector3 operator*( float factor ) const noexcept {
        return ubiVector3( x * factor, y * factor, z * factor );
    }

    ubiVector3& operator+=( const ubiVector3& other ) noexcept {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }

    ubiVector3& operator-=( const ubiVector3& other ) noexcept {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }

    ubiVector3& operator*=( float factor ) noexcept {
        x *= factor; y *= factor; z *= factor;
        return *this;
    }

    ubiVector3& operator/=( float factor ) noexcept {
        x /= factor; y /= factor; z /= factor;
        return *this;
    }

    ubiVector3 operator-( ) const {
        return { -x, -y, -z };
    }

    constexpr bool operator==( const ubiVector3& other ) const noexcept {
        return x == other.x && y == other.y && z == other.z;
    }

    inline float Dot( const ubiVector3& vector )
    {
        return x * vector.x + y * vector.y + z * vector.z;
    }

    void Normalize( ) {
        float magnitude = sqrt( x * x + y * y + z * z );
        if ( magnitude > 0.0f ) {
            x /= magnitude;
            y /= magnitude;
            z /= magnitude;
        }
    }

    void Normalize2( )
    {
        float magnitude = std::sqrt( x * x + y * y + z * z );
        if ( magnitude != 0.0f )
        {
            x /= magnitude;
            y /= magnitude;
            z /= magnitude;
        }
    }

    float x, y, z;
};


class ubiVector4 {
public:
    constexpr ubiVector4( float x = 0.f, float y = 0.f, float z = 0.f, float w = 0.f ) noexcept
        : x( x ), y( y ), z( z ), w( w ) {
    }

    constexpr ubiVector4 operator-( const ubiVector4& other ) const noexcept {
        return ubiVector4( x - other.x, y - other.y, z - other.z, w - other.w );
    }

    constexpr ubiVector4 operator+( const ubiVector4& other ) const noexcept {
        return ubiVector4( x + other.x, y + other.y, z + other.z, w + other.w );
    }

    constexpr ubiVector4 operator/( float factor ) const noexcept {
        return ubiVector4( x / factor, y / factor, z / factor, w / factor );
    }

    constexpr ubiVector4 operator*( float factor ) const noexcept {
        return ubiVector4( x * factor, y * factor, z * factor, w * factor );
    }

    ubiVector4& operator+=( const ubiVector4& other ) noexcept {
        x += other.x; y += other.y; z += other.z; w += other.w;
        return *this;
    }

    ubiVector4& operator-=( const ubiVector4& other ) noexcept {
        x -= other.x; y -= other.y; z -= other.z; w -= other.w;
        return *this;
    }

    ubiVector4& operator*=( float factor ) noexcept {
        x *= factor; y *= factor; z *= factor; w *= factor;
        return *this;
    }

    ubiVector4& operator/=( float factor ) noexcept {
        x /= factor; y /= factor; z /= factor; w /= factor;
        return *this;
    }

    ubiVector4 operator*( const  ubiVector4& other ) const
    {
        ubiVector4 result;

        result.w = w * other.w - x * other.x - y * other.y - z * other.z;
        result.x = w * other.x + x * other.w + y * other.z - z * other.y;
        result.y = w * other.y - x * other.z + y * other.w + z * other.x;
        result.z = w * other.z + x * other.y - y * other.x + z * other.w;

        return result;
    }

    void Normalize( )
    {
        float magnitude = std::sqrt( x * x + y * y + z * z + w * w );
        if ( magnitude != 0.0f )
        {
            x /= magnitude;
            y /= magnitude;
            z /= magnitude;
            w /= magnitude;
        }
    }

    inline float Length( )
    {
        return std::sqrt( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) );
    }

    float x, y, z, w;

};

inline int calculateDistance( const ubiVector4& p1, const ubiVector4& p2 )
{
    int distance = std::sqrt( std::pow( p2.x - p1.x, 2 ) + std::pow( p2.y - p1.y, 2 ) + std::pow( p2.z - p1.z, 2 ) );
    return distance;
}

inline ubiVector4 angles_to_quat( const ubiVector3& euler ) {
    ubiVector4 result{};

    auto yaw = ( euler.z * 0.01745329251f ) * 0.5f;
    auto sy = sinf( yaw );
    auto cy = cosf( yaw );

    auto roll = ( euler.x * 0.01745329251f ) * 0.5f;
    auto sr = sinf( roll );
    auto cr = cosf( roll );

    constexpr auto sp = 0.f;
    constexpr auto cp = 1.f;

    result.x = cy * sr * cp - sy * cr * sp;
    result.y = cy * cr * sp + sy * sr * cp;
    result.z = sy * cr * cp - cy * sr * sp;
    result.w = cy * cr * cp + sy * sr * sp;

    return ubiVector4( result.x, result.y, result.z, result.w );
}

inline ubiVector3 calculate_angles( const ubiVector4& target, const ubiVector4& source ) {
    const auto length = [ ] ( float x, float y, float z ) {
        return sqrtf( ( x * x ) + ( y * y ) + ( z * z ) );
        };


    auto dir = ubiVector3( target.x, target.y, target.z ) - ubiVector3( source.x, source.y, source.z );

    if ( dir.x == 0.f || dir == ubiVector3( ) ) return ubiVector3( );


    float x = asinf( dir.z / length( dir.x, dir.y, dir.z ) ) * 57.2957795131f; // Pitch
    float z = atanf( dir.y / dir.x ) * 57.2957795131f;

    if ( dir.x >= 0.f ) z += 180.f;

    if ( x > 179.99f ) x -= 360.f;
    else if ( x < -179.99f ) x += 360.f;


    return ubiVector3( x, 0.f, z + 90.f );
}

struct ubiViewMatrix {
    ubiViewMatrix( ) noexcept
        : m_data( ) {
    }

    float* operator[]( int index ) noexcept {
        return m_data[ index ];
    }

    const float* operator[]( int index ) const noexcept {
        return m_data[ index ];
    }

    float m_data[ 4 ][ 4 ];
};

struct IColor
{
    uint8_t B, R, G, A;

    static IColor float_to_icolor( float R, float G, float B, float A )
    {
        return IColor{ static_cast< uint8_t >( std::clamp( B, 0.0f, 1.0f ) * 255.0f ), static_cast< uint8_t >( std::clamp( G, 0.0f, 1.0f ) * 255.0f ), static_cast< uint8_t >( std::clamp( R, 0.0f, 1.0f ) * 255.0f ), static_cast< uint8_t >( std::clamp( A, 0.0f, 1.0f ) * 255.0f ) };
    }
};

namespace havok {
    using Vec2 = ubiVector2;
    using Vec3 = ubiVector3;
    using Vec4 = ubiVector4;

    inline Vec4 EulerToQuaternion( float pitch, float yaw, float roll )
    {
        float pitchRad = pitch * 0.01745329251f * 0.5f;
        float yawRad = yaw * 0.01745329251f * 0.5f;
        float rollRad = roll * 0.01745329251f * 0.5f;

        float sp = sinf( pitchRad ), cp = cosf( pitchRad );
        float sy = sinf( yawRad ), cy = cosf( yawRad );
        float sr = sinf( rollRad ), cr = cosf( rollRad );

        return Vec4(
            cy * sr * cp - sy * cr * sp,
            cy * cr * sp + sy * sr * cp,
            sy * cr * cp - cy * sr * sp,
            cy * cr * cp + sy * sr * sp
        );
    }

    inline int color_float_to_bgra( float r, float g, float b, float a )
    {
        return ( ( ( int ) ( b * 255.f ) ) & 0xFF ) |
            ( ( ( ( int ) ( g * 255.f ) & 0xFF ) << 8 ) ) |
            ( ( ( ( int ) ( r * 255.f ) & 0xFF ) << 16 ) ) |
            ( ( ( ( int ) ( a * 255.f ) & 0xFF ) << 24 ) );
    }


}


#endif // !havok_math