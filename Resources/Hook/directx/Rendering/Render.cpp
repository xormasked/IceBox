#include "Render.hpp"

#include "../../../../Core/Engine/Anvil/scimitar.hpp"
#include "../../../../Core/Engine/Anvil/Raycasting/RayCasting.hpp"
#include "../../../../ice/features/rage_bot.hpp"
#include "../../../../Resources/config.hpp"
#include "../../../External/ImGui/imgui.h"

namespace {

    bool WorldToScreen( const ubiVector3& position, ubiVector2& screen ) {
        auto* view = Scimitar::view_translation::get( );
        if ( !view ) return false;

        const ubiVector3 viewTranslation = view->get_view_translation( );
        const ubiVector3 viewRight = view->get_view_right( );
        const ubiVector3 viewUp = view->get_view_up( );
        const ubiVector3 viewForward = view->get_view_forward( );

        const ubiVector3 temp = position - viewTranslation;
        const float x = temp.x * viewRight.x + temp.y * viewRight.y + temp.z * viewRight.z;
        const float y = temp.x * viewUp.x + temp.y * viewUp.y + temp.z * viewUp.z;
        const float z = -( temp.x * viewForward.x + temp.y * viewForward.y + temp.z * viewForward.z );
        if ( z < 1.0f ) return false;

        const float width = static_cast< float >( GetSystemMetrics( SM_CXSCREEN ) );
        const float height = static_cast< float >( GetSystemMetrics( SM_CYSCREEN ) );
        screen.x = ( width / 2.0f ) * ( 1.0f + x / view->get_view_fovX( ) / z );
        screen.y = ( height / 2.0f ) * ( 1.0f - y / view->get_view_fovY( ) / z );
        return true;
    }

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

        rage_bot::run( visuals::RageBot );

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
        auto* fg = ImGui::GetForegroundDrawList( );

        for ( int i = 0; i < controllerCount; ++i ) {
            auto* controller = *reinterpret_cast< Scimitar::Controller** >( reinterpret_cast< uintptr_t >( controllerList ) + ( 0x8 * i ) );
            if ( !controller || controller == localController ) continue;

            auto* pawn = controller->pawn_decrypt( );
            if ( !pawn ) continue;
            auto* entity = pawn->entity_decrypt( );
            if ( !entity ) continue;

            ubiVector2 screenPos{};
            if ( visuals::Tracers && WorldToScreen( entity->Origin( ), screenPos ) ) {
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
                if ( WorldToScreen( { p1.x, p1.y, p1.z }, s1 ) && WorldToScreen( { p2.x, p2.y, p2.z }, s2 ) ) {
                    const ImU32 skeletonColor = visuals::SkeletonVisCheck
                        ? ( segmentVisible ? IM_COL32( 0, 255, 0, 255 ) : IM_COL32( 255, 0, 0, 255 ) )
                        : ImGui::ColorConvertFloat4ToU32( visuals::SkeletonColor );
                    fg->AddLine(
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