#pragma once

#include "../../Maths/havok_math.hpp"

namespace IcePhysics::Terrain {

float vertical_clear_below( const ubiVector3& sample_origin );

float tilted_clear_from_center(
    const ubiVector3& center,
    const ubiVector3& horiz_unit,
    float ray_length_m );

float estimate_surface_gradient_xy(
    const ubiVector3& center,
    float query_radius_m,
    float& out_gx,
    float& out_gy );

} // namespace IcePhysics::Terrain
