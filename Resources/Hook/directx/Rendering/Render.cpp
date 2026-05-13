#include "Render.hpp"

#include "../../../../Core/Engine/Anvil/PRender/W2S.hpp"
#include "../../../../Core/Engine/Anvil/Raycasting/RayCasting.hpp"
#include "../../../../Core/Engine/Anvil/scimitar.hpp"
#include "../../../../Core/Engine/Anvil/skeletons.h"
#include "../../../../ice/IceBox.hpp"
#include "../../../../Resources/config.hpp"
#include "../../../External/ImGui/imgui.h"

namespace {

	bool IsSkeletonSegmentVisible( const havok::Vec4& p1, const havok::Vec4& p2 ) {
		auto* view = Scimitar::view_translation::get( );
		if ( !view ) return false;

		const ubiVector3 cameraPos3 = view->get_view_translation( );
		const ubiVector4 cameraPos( cameraPos3.x, cameraPos3.y, cameraPos3.z, 1.0f );
		const ubiVector4 dest1( p1.x, p1.y, p1.z, 1.0f );
		const ubiVector4 dest2( p2.x, p2.y, p2.z, 1.0f );

		return __RaycastData::is_visible( cameraPos, dest1 ) &&
			__RaycastData::is_visible( cameraPos, dest2 );
	}
}

namespace Render {

	void Renderables( ) {
		if ( !Scimitar::round_state::CurrentState( Scimitar::round_state::Prep ) &&
		     !Scimitar::round_state::CurrentState( Scimitar::round_state::Action ) )
			return;

		auto* gameManager = Scimitar::game_manager::get( );
		if ( !gameManager ) return;

		auto* localController = gameManager->get_local_controller( );
		if ( !localController ) return;

		auto controllerList = gameManager->get_controller_list( );
		int controllerCount = gameManager->get_controller_size( );
		if ( controllerCount > 256 ) controllerCount = 256;

		const float screenWidth = static_cast< float >( GetSystemMetrics( SM_CXSCREEN ) );
		const float screenHeight = static_cast< float >( GetSystemMetrics( SM_CYSCREEN ) );
		auto* bg = ImGui::GetBackgroundDrawList( );
		if ( bg ) {
			IceBox::render_raycast_debug_overlay( ImGui::GetIO( ).DeltaTime, bg );
			IceBox::render_silent_aim_fov_overlay( bg );
		}

		for ( int i = 0; i < controllerCount; ++i ) {
			auto* controller = *reinterpret_cast< Scimitar::Controller** >( reinterpret_cast< uintptr_t >( controllerList ) + ( 0x8 * i ) );
			if ( !controller || controller == localController ) continue;

			auto* pawn = controller->pawn_decrypt( );
			if ( !pawn ) continue;
			auto* entity = pawn->entity_decrypt( );
			if ( !entity ) continue;

			ubiVector2 screenPos{};
			if ( visuals::Tracers && W2S( entity->Origin( ), screenPos ) ) {
				if ( visuals::TracerSelected == 0 )
					bg->AddLine( { screenWidth / 2.0f, screenHeight }, { screenPos.x, screenPos.y }, ImGui::ColorConvertFloat4ToU32( visuals::TracerColor ) );
				else if ( visuals::TracerSelected == 1 )
					bg->AddLine( { screenWidth / 2.0f, screenHeight / 2.0f }, { screenPos.x, screenPos.y }, ImGui::ColorConvertFloat4ToU32( visuals::TracerColor ) );
				else
					bg->AddLine( { screenWidth / 2.0f, 0.0f }, { screenPos.x, screenPos.y }, ImGui::ColorConvertFloat4ToU32( visuals::TracerColor ) );
			}

			if ( !visuals::Skeleton ) continue;

			auto* skeleton = entity->get_skeleton( );
			if ( !skeleton ) continue;

			for ( const auto& bones : g_SkeletonPairs ) {
				const havok::Vec4 p1 = skeleton->bone( bones.first );
				const havok::Vec4 p2 = skeleton->bone( bones.second );

				bool segmentVisible = true;
				if ( visuals::SkeletonVisCheck )
					segmentVisible = IsSkeletonSegmentVisible( p1, p2 );

				ubiVector2 s1{}, s2{};
				if ( W2S( ubiVector3( p1.x, p1.y, p1.z ), s1 ) && W2S( ubiVector3( p2.x, p2.y, p2.z ), s2 ) ) {
				const ImU32 skeletonColor = visuals::SkeletonVisCheck
					? ( segmentVisible ? ImGui::ColorConvertFloat4ToU32( visuals::SkeletonVisVisibleColor )
					                   : ImGui::ColorConvertFloat4ToU32( visuals::SkeletonVisInvisColor ) )
					: ImGui::ColorConvertFloat4ToU32( visuals::SkeletonColor );
					bg->AddLine(
						ImVec2( s1.x, s1.y ),
						ImVec2( s2.x, s2.y ),
						skeletonColor,
						visuals::SkeletonThickness
					);
				}
			}
		}

	}

}
