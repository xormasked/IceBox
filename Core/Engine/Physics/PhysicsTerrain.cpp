#include "PhysicsTerrain.hpp"

#include "PhysicsClearance.hpp"
#include "PhysicsConfig.hpp"
#include "PhysicsMath.hpp"
#include "PhysicsWorld.hpp"

#include <cmath>

namespace IcePhysics::Terrain {

float vertical_clear_below( const ubiVector3& sample_origin )
{
	return Clearance::max_clear_prefix_m(
	    sample_origin,
	    World::kGravityDown,
	    Config::kTerrainVerticalProbeDepthM );
}

float tilted_clear_from_center(
    const ubiVector3& center,
    const ubiVector3& horiz_unit,
    float ray_length_m )
{
	ubiVector3 v(
	    horiz_unit.x * Config::kTerrainTiltRayHorizBlend,
	    horiz_unit.y * Config::kTerrainTiltRayHorizBlend,
	    World::kGravityDown.z * Config::kTerrainTiltRayDownBlend );
	v.Normalize( );
	return Clearance::max_clear_prefix_m( center, v, ray_length_m );
}

float estimate_surface_gradient_xy(
    const ubiVector3& center,
    float query_radius_m,
    float& out_gx,
    float& out_gy )
{
	const float L = Config::kTerrainGradientSampleOffsetM;
	const float cxp = vertical_clear_below( center + ubiVector3( L, 0.f, 0.f ) );
	const float cxm = vertical_clear_below( center - ubiVector3( L, 0.f, 0.f ) );
	const float cyp = vertical_clear_below( center + ubiVector3( 0.f, L, 0.f ) );
	const float cym = vertical_clear_below( center - ubiVector3( 0.f, L, 0.f ) );

	float gx = cxp - cxm;
	float gy = cyp - cym;

	const float tilt_len =
	    query_radius_m + Config::kTerrainTiltProbeExtraM;
	ubiVector3 dirs[ Config::kHorizontalSampleDirections ];
	Math::horizontal_directions_xy( dirs );
	gx += Config::kTerrainDiagramRayGain *
	    ( tilted_clear_from_center( center, dirs[ 5 ], tilt_len )
		- tilted_clear_from_center( center, dirs[ 7 ], tilt_len ) );

	out_gx = gx;
	out_gy = gy;
	return sqrtf( gx * gx + gy * gy );
}

} // namespace IcePhysics::Terrain
