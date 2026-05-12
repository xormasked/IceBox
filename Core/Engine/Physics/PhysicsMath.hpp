#pragma once

#include "PhysicsConfig.hpp"
#include "PhysicsWorld.hpp"

#include <cmath>


namespace IcePhysics::Math {

	inline float len3( const ubiVector3& v ) noexcept
	{
		return sqrtf( v.Dot( v ) );
	}

	inline void horizontal_directions_xy(
		ubiVector3 out[ Config::kHorizontalSampleDirections ] ) noexcept
	{
		const float inv_sqrt2 = 0.70710678f;
		out[ 0 ] = ubiVector3( 1.f, 0.f, 0.f );
		out[ 1 ] = ubiVector3( -1.f, 0.f, 0.f );
		out[ 2 ] = ubiVector3( 0.f, 1.f, 0.f );
		out[ 3 ] = ubiVector3( 0.f, -1.f, 0.f );
		out[ 4 ] = ubiVector3( inv_sqrt2, inv_sqrt2, 0.f );
		out[ 5 ] = ubiVector3( inv_sqrt2, -inv_sqrt2, 0.f );
		out[ 6 ] = ubiVector3( -inv_sqrt2, inv_sqrt2, 0.f );
		out[ 7 ] = ubiVector3( -inv_sqrt2, -inv_sqrt2, 0.f );
	}

	inline void clamp_velocity_horizontal_and_fall(
		ubiVector3& vel,
		float max_horizontal_speed,
		float max_fall_speed_along_gravity ) noexcept
	{
		const ubiVector3& g = World::kGravityDown;
		ubiVector3 tan = vel - g * vel.Dot( g );
		float lh = len3( tan );
		if ( lh > max_horizontal_speed )
			tan *= max_horizontal_speed / lh;
		const float v_dn = vel.Dot( g );
		const float v_dn_clamped =
			v_dn > max_fall_speed_along_gravity ? max_fall_speed_along_gravity : v_dn;
		vel = tan + g * v_dn_clamped;
	}

} // namespace IcePhysics::Math
