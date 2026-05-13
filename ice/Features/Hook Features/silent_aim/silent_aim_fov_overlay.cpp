#include "../../../IceBox.hpp"

#include "../../../../Core/Maths/havok_math.hpp"
#include "../../../../Resources/config.hpp"
#include "../../../../Resources/External/ImGui/imgui.h"

void IceBox::render_silent_aim_fov_overlay( ImDrawList* dl ) noexcept
{
#ifdef _WIN64
	if ( !dl || !visuals::SilentAim || !visuals::SilentAimShowFovCircle )
		return;

	const ImVec4& q = visuals::SilentAimCircleColor;
	const float cam = visuals::EyeFovDegrees > 1.f ? visuals::EyeFovDegrees : 90.f;
	const float r = havok::screen_fov_cone_radius_px(
		globals::Width,
		visuals::SilentAimFovDegrees,
		cam );
	const ImVec2 c{ globals::Width * .5f, globals::Height * .5f };
	const float a = q.w * visuals::SilentAimCircleOpacity;
	const ImU32 fill = ImGui::ColorConvertFloat4ToU32( { q.x, q.y, q.z, a } );
	const float sa = ( a + .25f > 1.f ) ? 1.f : ( a + .25f );
	const ImU32 stroke = ImGui::ColorConvertFloat4ToU32( { q.x, q.y, q.z, sa } );
	if ( visuals::SilentAimCircleFilled )
		dl->AddCircleFilled( c, r, fill, 96 );
	dl->AddCircle( c, r, stroke, 96, visuals::SilentAimCircleFilled ? 1.5f : 2.f );
#else
	(void) dl;
#endif
}
