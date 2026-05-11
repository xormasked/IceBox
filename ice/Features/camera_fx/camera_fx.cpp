#include "../../IceBox.hpp"
#include "../../../Core/Engine/Anvil/scimitar.hpp"
#include "../../../Core/Maths/havok_math.hpp"
#include "../../../Resources/config.hpp"

auto IceBox::camera_fx_apply_fov( ) -> void
{
	const uintptr_t eye = Scimitar::get_camera_fov( );
	const uintptr_t vm = Scimitar::get_viewmodel_fov( );
	if ( !eye || !vm )
		return;

	if ( visuals::FovEnabled ) {
		Memory::Write<float>( eye, havok::deg_to_rad( visuals::EyeFovDegrees ) );
		Memory::Write<float>( vm, havok::deg_to_rad( visuals::ViewmodelFovDegrees ) );
	} else {
		Memory::Write<float>( eye, havok::deg_to_rad( 90.f ) );
		Memory::Write<float>( vm, havok::deg_to_rad( 50.f ) );
	}
}
