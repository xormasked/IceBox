#include "../../../IceBox.hpp"
#include "../../../../Core/Engine/Anvil/PRender/W2S.hpp"
#include "../../../../Resources/config.hpp"
#include <imgui/imgui.h>

#include <cmath>

namespace {

	float g_phase = 0.f;

} // namespace

auto IceBox::render_raycast_debug_overlay( float dt, ImDrawList* dl ) -> void
{
	if ( !visuals::RaycastClosestDebug || !dl || !ImGui::GetCurrentContext( ) )
		return;

	const float step = ( dt > 0.f && std::isfinite( dt ) ) ? dt : ( 1.f / 60.f );
	g_phase += step;
	if ( !std::isfinite( g_phase ) ) g_phase = 0.f;

	constexpr float T = 1.25f;
	while ( g_phase > T * 512.f ) g_phase -= T * 512.f;

	ubiVector3 w{};
	if ( !IceBox::raycast_debug_hit_marker_world( w ) )
		return;

	ubiVector2 s{};
	if ( !W2S( w, s ) || !std::isfinite( s.x ) || !std::isfinite( s.y ) )
		return;

	const ImVec2 c( s.x, s.y );
	constexpr float r0 = 7.f, span = 52.f;

	for ( int i = 0; i < 3; ++i ) {
		float u = std::fmod( g_phase + T * ( i / 3.f ), T ) / T;
		if ( !std::isfinite( u ) ) u = 0.f;
		float rad = r0 + u * span;
		if ( std::isfinite( rad ) && rad > 0.f && rad < 1e5f ) {
			int a = static_cast< int >( 190.f * ( 1.f - u ) * ( 1.f - u ) );
			if ( a > 2 ) dl->AddCircle( c, rad, IM_COL32( 90, 175, 255, a ), 48, 2.f );
		}
	}

	dl->AddCircleFilled( c, r0, IM_COL32( 35, 105, 230, 95 ), 32 );
	dl->AddCircleFilled( c, r0 * 0.58f, IM_COL32( 130, 195, 255, 140 ), 24 );
	dl->AddCircle( c, r0, IM_COL32( 190, 225, 255, 210 ), 32, 1.75f );
	dl->AddCircle( c, r0 * 0.92f, IM_COL32( 240, 248, 255, 55 ), 32, 1.f );
}
