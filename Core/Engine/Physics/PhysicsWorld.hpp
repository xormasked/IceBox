#pragma once

#include "../../Maths/havok_math.hpp"

namespace IcePhysics::World {

	// Horizontal XY plane; gravity acts along +kGravityDown
	inline constexpr ubiVector3 kGravityDown{ 0.f, 0.f, -1.f };

} // namespace IcePhysics::World
