#include "camera_fx.hpp"

#include "../../Core/Engine/Anvil/scimitar.hpp"
#include "../../Core/Maths/havok_math.hpp"
#include "../../Resources/config.hpp"

namespace camera_fx {

    void apply_fov( )
    {
        if ( visuals::FovEnabled ) {

            Scimitar::set_PFOV( havok::deg_to_rad( visuals::EyeFovDegrees ) );
            Scimitar::set_WFOV( havok::deg_to_rad( visuals::ViewmodelFovDegrees ) );

        }
        else
        {
            Scimitar::set_PFOV( havok::deg_to_rad( 90.0f ) );
            Scimitar::set_WFOV( havok::deg_to_rad( 50.0f ) );
        }
    }



} // namespace camera_fx
