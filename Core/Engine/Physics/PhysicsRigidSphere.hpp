#pragma once

#include "../../Maths/havok_math.hpp"

namespace IcePhysics::RigidSphere {

    // Single spherical rigid body vs ray-tested terrain (walls XY + floor along gravity).
    void simulate_step(
        float sub_dt,
        ubiVector3& pos,
        ubiVector3& vel,
        float radius_m );

} // namespace IcePhysics::RigidSphere
