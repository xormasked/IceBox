#include <charconv>
#include <wtypes.h>
#include <vector>
#include <xmmintrin.h>

struct vec4
{
	float x, y, z, w;
};

struct vec3
{
	float x, y, z;

	vec3 operator-( vec3 ape )
	{
		return { x - ape.x, y - ape.y, z - ape.z };
	}

	vec3 operator+( vec3 ape )
	{
		return { x + ape.x, y + ape.y, z + ape.z };
	}

	vec3 operator*( float ape )
	{
		return { x * ape, y * ape, z * ape };
	}

	vec3 operator/( float ape )
	{
		return { x / ape, y / ape, z / ape };
	}

	vec3 operator/=( float ape )
	{
		x /= ape;
		y /= ape;
		z /= ape;

		return *this;
	}

	vec3 operator+=( vec3 ape )
	{
		return { x += ape.x, y += ape.y, z += ape.z };
	}

	vec3 operator-=( vec3 ape )
	{
		return { x -= ape.x, y -= ape.y, z -= ape.z };
	}

	float dot( vec3 b )
	{
		return ( ( this->x * b.x ) + ( this->y * b.y ) + ( this->z * b.z ) );
	}

	void Normalize( )
	{
		while ( x < 0.0f ) {
			x += 360.0f;
		}
		while ( x > 360.0f ) {
			x = 360.0f;
		}

		while ( y < 0.0f ) {
			y += 360.0f;
		}
		while ( y > 360.0f ) {
			y = 360.0f;
		}

		z = 0.0f;
	}

	float Length( )
	{
		return sqrt( ( x * x ) + ( y * y ) + ( z * z ) );
	}

	float Length2D( )
	{
		return sqrt( ( x * x ) + ( y * y ) );
	}

	float DistTo( vec3 ape )
	{
		return ( *this - ape ).Length( );
	}

	float Dist2D( vec3 ape )
	{
		return ( *this - ape ).Length2D( );
	}

	float dist( vec3 b )
	{
		return sqrtf( ( this->x - b.x ) * ( this->x - b.x ) + ( this->y - b.y ) * ( this->y - b.y ) + ( this->z - b.z ) * ( this->z - b.z ) );
	}

	float Dot( vec3 ape )
	{
		return ( ( this->x * ape.x ) + ( this->y * ape.y ) + ( this->z * ape.z ) );
	}
	bool empty( ) {
		return ( !x && !y && !z );
	}
};

struct vec2
{
	float x, y;

	vec2 operator-( vec2 ape )
	{
		return { x - ape.x, y - ape.y };
	}

	vec2 operator+( vec2 ape )
	{
		return { x + ape.x, y + ape.y };
	}

	vec2 operator*( float ape )
	{
		return { x * ape, y * ape };
	}

	vec2 operator/( float ape )
	{
		return { x / ape, y / ape };
	}

	vec2 operator/=( float ape )
	{
		x /= ape;
		y /= ape;

		return *this;
	}

	vec2 operator+=( vec2 ape )
	{
		return { x += ape.x, y += ape.y };
	}

	vec2 operator-=( vec2 ape )
	{
		return { x -= ape.x, y -= ape.y };
	}
	bool empty( ) {
		return ( x == 0 && y == 0 );
	}
	void Normalize( )
	{
		while ( x < 0.0f ) {
			x += 360.0f;
		}
		while ( x > 360.0f ) {
			x = 360.0f;
		}

		while ( y < 0.0f ) {
			y += 360.0f;
		}
		while ( y > 360.0f ) {
			y = 360.0f;
		}
	}

	float Length2D( )
	{
		return sqrt( ( x * x ) + ( y * y ) );
	}

	float Dist2D( vec2 ape )
	{
		return ( *this - ape ).Length2D( );
	}
};


struct frameData
{
	vec2 root;
	vec2 head;
	vec2 neck;
	float distance;
};

struct frame
{
	std::vector<frameData> data;
};

#pragma once

#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>

class Vector2
{
public:
	Vector2( ) : x( 0.f ), y( 0.f )
	{

	}

	Vector2( float _x, float _y ) : x( _x ), y( _y )
	{

	}
	~Vector2( )
	{

	}

	float x;
	float y;

	Vector2& operator+=( const Vector2& v )
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	bool equals( Vector2 other )
	{
		return ( x == other.x && y == other.y );
	}

};

class Vector3
{
public:
	float x;
	float y;
	float z;

	Vector3( );
	Vector3( float x, float y, float z );

	Vector3 operator+( const Vector3& vector ) const;
	Vector3 operator-( const Vector3& vector ) const;
	Vector3 operator-( ) const;
	Vector3 operator*( float number ) const;
	Vector3 operator/( float number ) const;

	Vector3& operator+=( const Vector3& vector );
	Vector3& operator-=( const Vector3& vector );
	Vector3& operator*=( float number );
	Vector3& operator/=( float number );

	bool operator==( const Vector3& vector ) const;
	bool operator!=( const Vector3& vector ) const;

	inline float Dot( const Vector3& vector )
	{
		return x * vector.x + y * vector.y + z * vector.z;
	}

	inline float Distance( const Vector3& vector )
	{
		float xCoord = vector.x - x;
		float yCoord = vector.y - y;
		float zCoord = vector.z - z;
		return sqrtf( ( xCoord * xCoord ) + ( yCoord * yCoord ) + ( zCoord * zCoord ) );
	}

	inline float Length( )
	{
		return sqrt( ( x * x ) + ( y * y ) + ( z * z ) );
	}

	inline void Rotate2D( float angle )
	{
		// use local variables to find transformed components
		float Vx1 = cosf( angle ) * x - sinf( angle ) * y;
		float Vy1 = sinf( angle ) * x + cosf( angle ) * y;
		// store results thru the pointer
		x = Vx1;
		y = Vy1;

		return;
	}

	inline bool Normalize( )
	{
		if ( x != x || y != y || z != z ) return false;

		if ( x > 180 ) x -= 360.f;
		if ( x < -180 ) x += 360.f;
		if ( y > 180.f ) y -= 360.f;
		if ( y < -180.f ) y += 360.f;

		return x >= -180.f && x <= 180.f && y >= -180.f && y <= 180.f;
	}

	inline void Normalize2( )
	{
		float magnitude = std::sqrt( x * x + y * y + z * z );
		if ( magnitude != 0.0f )
		{
			x /= magnitude;
			y /= magnitude;
			z /= magnitude;
		}
	}

	void clamp( ) {
		if ( this->x < -180.f ) {
			this->x += 360.f;
		}
		if ( this->x > 180.f ) {
			this->x -= 360.f;
		}
		if ( this->y < -76.f ) {
			this->y += 152.f;
		}
		if ( this->y > 76.f ) {
			this->y -= 152.f;
		}
		this->z = 0.f;
	}
};

inline bool Vector3::operator==( const Vector3& vector ) const
{
	return x == vector.x && y == vector.y && z == vector.z;
}

inline bool Vector3::operator!=( const Vector3& vector ) const
{
	return x != vector.x || y != vector.y || z != vector.z;
}

inline Vector3 Vector3::operator+( const Vector3& vector ) const
{
	return Vector3( x + vector.x, y + vector.y, z + vector.z );
}

inline Vector3 Vector3::operator-( const Vector3& vector ) const
{
	return Vector3( x - vector.x, y - vector.y, z - vector.z );
}

inline Vector3 Vector3::operator-( ) const
{
	return Vector3( -x, -y, -z );
}

inline Vector3 Vector3::operator*( float number ) const
{
	return Vector3( x * number, y * number, z * number );
}

inline Vector3 Vector3::operator/( float number ) const
{
	return Vector3( x / number, y / number, z / number );
}

inline Vector3::Vector3( )
{
}

inline Vector3::Vector3( float x, float y, float z )
	: x( x ), y( y ), z( z )
{
}

inline Vector3& Vector3::operator+=( const Vector3& vector )
{
	x += vector.x;
	y += vector.y;
	z += vector.z;

	return *this;
}

inline Vector3& Vector3::operator-=( const Vector3& vector )
{
	x -= vector.x;
	y -= vector.y;
	z -= vector.z;

	return *this;
}

inline Vector3& Vector3::operator*=( float number )
{
	x *= number;
	y *= number;
	z *= number;

	return *this;
}

inline Vector3& Vector3::operator/=( float number )
{
	x /= number;
	y /= number;
	z /= number;

	return *this;
}

class alignas( 16 ) Vector4
{
public:
	float x;
	float y;
	float z;
	float w;
	Vector4( ) : x( 0.f ), y( 0.f ), z( 0.f ), w( 0.f )
	{

	}

	Vector4( float _x, float _y, float _z, float _w ) : x( _x ), y( _y ), z( _z ), w( _w )
	{

	}
	~Vector4( )
	{

	}

	inline Vector4 operator-( const Vector4& vector ) const
	{
		return Vector4( x - vector.x, y - vector.y, z - vector.z, w - vector.w );
	}

	inline Vector4 operator/( float number ) const
	{
		if ( number == 0.f ) return Vector4( x, y, z, w );
		return Vector4( x / number, y / number, z / number, w / number );
	}

};

inline float DistanceVec2( Vector2 src, Vector2 dst )
{
	float distance;
	distance = sqrtf( powf( src.x - dst.x, 2 ) + powf( src.y - dst.y, 2 ) );
	return distance;
}

inline float DistanceVec3( Vector3 src, Vector3 dst )
{
	float distance;
	distance = sqrtf( powf( src.x - dst.x, 2 ) + powf( src.y - dst.y, 2 ) + powf( src.z - dst.z, 2 ) );
	return distance;
}

typedef struct _Matrix4x4 {
	//float m[4][4];
	float m[ 16 ];
}Matrix4x4;

Matrix4x4 inverse( const Matrix4x4& matrix ) {
	Matrix4x4 inv;
	float det;

	det = matrix.m[ 0 ] * ( matrix.m[ 5 ] * ( matrix.m[ 10 ] * matrix.m[ 15 ] - matrix.m[ 11 ] * matrix.m[ 14 ] ) - matrix.m[ 6 ] * ( matrix.m[ 9 ] * matrix.m[ 15 ] - matrix.m[ 11 ] * matrix.m[ 13 ] ) + matrix.m[ 7 ] * ( matrix.m[ 9 ] * matrix.m[ 14 ] - matrix.m[ 10 ] * matrix.m[ 13 ] ) )
		- matrix.m[ 1 ] * ( matrix.m[ 4 ] * ( matrix.m[ 10 ] * matrix.m[ 15 ] - matrix.m[ 11 ] * matrix.m[ 14 ] ) - matrix.m[ 6 ] * ( matrix.m[ 8 ] * matrix.m[ 15 ] - matrix.m[ 11 ] * matrix.m[ 12 ] ) + matrix.m[ 7 ] * ( matrix.m[ 8 ] * matrix.m[ 14 ] - matrix.m[ 10 ] * matrix.m[ 12 ] ) )
		+ matrix.m[ 2 ] * ( matrix.m[ 4 ] * ( matrix.m[ 9 ] * matrix.m[ 15 ] - matrix.m[ 11 ] * matrix.m[ 13 ] ) - matrix.m[ 5 ] * ( matrix.m[ 8 ] * matrix.m[ 15 ] - matrix.m[ 11 ] * matrix.m[ 12 ] ) + matrix.m[ 7 ] * ( matrix.m[ 8 ] * matrix.m[ 13 ] - matrix.m[ 9 ] * matrix.m[ 12 ] ) )
		- matrix.m[ 3 ] * ( matrix.m[ 4 ] * ( matrix.m[ 9 ] * matrix.m[ 14 ] - matrix.m[ 10 ] * matrix.m[ 13 ] ) - matrix.m[ 5 ] * ( matrix.m[ 8 ] * matrix.m[ 14 ] - matrix.m[ 10 ] * matrix.m[ 12 ] ) + matrix.m[ 6 ] * ( matrix.m[ 8 ] * matrix.m[ 13 ] - matrix.m[ 9 ] * matrix.m[ 12 ] ) );

	if ( det == 0 ) {

		return Matrix4x4( );
	}


	float invDet = 1.0f / det;

	return inv;
}

#endif