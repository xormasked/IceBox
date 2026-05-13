#include "../../../IceBox.hpp"
#include "../../../../Core/Engine/Anvil/scimitar.hpp"
#include "../../../../Core/Maths/havok_math.hpp"
#include "../../../../Core/Utils/memory.hpp"
#include "../../../../Resources/config.hpp"

namespace {

	static float s_baseline_eye_fov_rad = 0.f;
	static float s_baseline_vm_fov_rad = 0.f;
	static bool s_have_fov_baseline = false;

} // namespace

auto IceBox::camera_fx_apply_fov( ) -> void
{
	const uintptr_t eye = Scimitar::get_camera_fov( );
	const uintptr_t vm = Scimitar::get_viewmodel_fov( );
	if ( !eye || !vm )
		return;

	if ( !s_have_fov_baseline ) {
		s_baseline_eye_fov_rad = Memory::Read<float>( eye );
		s_baseline_vm_fov_rad = Memory::Read<float>( vm );
		s_have_fov_baseline = true;
	}

	if ( visuals::FovEnabled ) {
		Memory::Write<float>( eye, havok::deg_to_rad( visuals::EyeFovDegrees ) );
		Memory::Write<float>( vm, havok::deg_to_rad( visuals::ViewmodelFovDegrees ) );
	} else {
		Memory::Write<float>( eye, s_baseline_eye_fov_rad );
		Memory::Write<float>( vm, s_baseline_vm_fov_rad );
	}
}

auto IceBox::camera_fx_prepare_uninject( ) -> void
{
	const uintptr_t eye = Scimitar::get_camera_fov( );
	const uintptr_t vm = Scimitar::get_viewmodel_fov( );
	if ( !s_have_fov_baseline )
		return;
	if ( eye )
		Memory::Write< float >( eye, s_baseline_eye_fov_rad );
	if ( vm )
		Memory::Write< float >( vm, s_baseline_vm_fov_rad );
	s_have_fov_baseline = false;
}
