#pragma once

#include "../../Maths/havok_math.hpp"

namespace IcePhysics::Clearance {

	// Longest prefix along dir_unit from o that stays unobstructed (game ray visibility).
	float max_clear_prefix_m( const ubiVector3& o, const ubiVector3& dir_unit, float max_len );

} // namespace IcePhysics::Clearance
