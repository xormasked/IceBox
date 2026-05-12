#include "PhysicsClearance.hpp"

#include "PhysicsConfig.hpp"

#include "../Anvil/Raycasting/RayCasting.hpp"

namespace IcePhysics::Clearance {

	float max_clear_prefix_m( const ubiVector3& o, const ubiVector3& dir_unit, float max_len )
	{
		if ( max_len < 1e-5f )
			return 0.f;
		const ubiVector4 A( o.x, o.y, o.z, 1.f );
		const ubiVector4 Bend(
			o.x + dir_unit.x * max_len,
			o.y + dir_unit.y * max_len,
			o.z + dir_unit.z * max_len,
			1.f );
		if ( __RaycastData::is_visible( A, Bend ) )
			return max_len;

		float lo = 0.f;
		float hi = max_len;
		for ( int i = 0; i < Config::kClearanceBisectIters; ++i ) {
			const float mid = ( lo + hi ) * 0.5f;
			const ubiVector4 M(
				o.x + dir_unit.x * mid,
				o.y + dir_unit.y * mid,
				o.z + dir_unit.z * mid,
				1.f );
			if ( __RaycastData::is_visible( A, M ) )
				lo = mid;
			else
				hi = mid;
		}

		float t = lo - Config::kSurfaceSkinM;
		if ( t < 0.f )
			t = 0.f;
		return t;
	}

} // namespace IcePhysics::Clearance
