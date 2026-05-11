#include "Render.hpp"

#include "../../../../Core/Engine/Anvil/scimitar.hpp"
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
}

namespace Render {

    void Renderables( ) {
        auto* gameManager = Scimitar::game_manager::get( );
        if ( !gameManager ) return;

        auto* localController = gameManager->get_local_controller( );
        if ( !localController ) return;
        auto* localPawn = localController->pawn_decrypt( );
        if ( !localPawn ) return;
        auto* localEntity = localPawn->entity_decrypt( );
        if ( !localEntity ) return;
        const BYTE localTeam = localEntity->GetAlliance( );

        auto controllerList = gameManager->controller_list_decrypt( );
        int controllerCount = gameManager->controller_size_decrypt( );
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
            if ( visuals::TeamCheck && entity->GetAlliance( ) == localTeam ) continue;

            auto* skeleton = entity->get_skeleton( );
            if ( !skeleton ) continue;

            for ( const auto& bones : g_SkeletonPairs ) {
                const havok::Vec4 p1 = skeleton->bone( bones.first );
                const havok::Vec4 p2 = skeleton->bone( bones.second );

                ubiVector2 s1{}, s2{};
                if ( WorldToScreen( { p1.x, p1.y, p1.z }, s1 ) && WorldToScreen( { p2.x, p2.y, p2.z }, s2 ) ) {
                    fg->AddLine(
                        ImVec2( s1.x, s1.y ),
                        ImVec2( s2.x, s2.y ),
                        ImGui::ColorConvertFloat4ToU32( visuals::SkeletonColor ),
                        visuals::SkeletonThickness
                    );
                }
            }
        }
    }

}