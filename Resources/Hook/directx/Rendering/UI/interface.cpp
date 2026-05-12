#include "../interface.hpp"

#include "../../../../config.hpp"
#include "../../d3d11hook.hpp"

namespace Render {

    auto user_interface( ) -> void
    {
        if ( !menu_open ) return;

        ImGui::SetNextWindowSize( ImVec2( 700, 520 ), ImGuiCond_FirstUseEver );
        ImGui::Begin( "IceBox Public", nullptr, ImGuiWindowFlags_NoCollapse );

        if ( ImGui::BeginTabBar( "MainTabs" ) ) {
            if ( ImGui::BeginTabItem( "Visuals" ) ) {
                ImGui::Checkbox( "Trace Lines", &visuals::Tracers );
                ImGui::SameLine( );
                ImGui::SetNextItemWidth( 240.f );
                ImGui::Combo( "##TracerPos", &visuals::TracerSelected, visuals::TracerPos, IM_ARRAYSIZE( visuals::TracerPos ) );
                ImGui::SameLine( );
                ImGui::ColorEdit3( "##TracerColor", &visuals::TracerColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_InputRGB );
                ImGui::Checkbox( "Skeleton", &visuals::Skeleton );
                ImGui::SameLine( );
                ImGui::ColorEdit3( "##SkeletonColor", &visuals::SkeletonColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_InputRGB );
                ImGui::SliderFloat( "Skeleton Thickness", &visuals::SkeletonThickness, 0.5f, 5.0f, "%.1f" );
                ImGui::Checkbox( "Skeleton VisCheck", &visuals::SkeletonVisCheck );
                ImGui::Checkbox( "Aspect ratio (camera +0x128)", &visuals::AspectRatioHook );
                ImGui::SliderFloat( "Aspect ratio", &visuals::AspectRatio, 0.1f, 2.0f, "%.2f" );
                ImGui::Separator( );
                ImGui::Checkbox( "Fov", &visuals::FovEnabled );
                ImGui::BeginDisabled( !visuals::FovEnabled );
                ImGui::SliderFloat( "Eye Fov", &visuals::EyeFovDegrees, 20.f, 120.f, "%.0f deg" );
                ImGui::SliderFloat( "Viewmodel Fov", &visuals::ViewmodelFovDegrees, 20.f, 120.f, "%.0f deg" );
                ImGui::EndDisabled( );
                ImGui::EndTabItem( );
            }

            if ( ImGui::BeginTabItem( "Misc" ) ) {
                ImGui::Checkbox( "Ragebot", &visuals::RageBot );
                ImGui::Checkbox( "Long melee", &visuals::LongMelee );
                ImGui::Checkbox( "Third person", &visuals::ThirdPerson );
                ImGui::Checkbox( "Raycast debug (console)", &visuals::RaycastClosestDebug );
                ImGui::Checkbox( "Run and shoot", &visuals::RunAndShoot );
                if ( visuals::RageBot ) {
                    ImGui::Indent( );
                    ImGui::Checkbox( "Vischeck", &visuals::RageBotVisCheck );
                    ImGui::Checkbox( "Pencheck", &visuals::RageBotPenCheck );
                    ImGui::Unindent( );
                }
                ImGui::Separator( );
                ImGui::EndTabItem( );
            }

            if ( ImGui::BeginTabItem( "Settings" ) ) {
                if ( ImGui::Button( "Uninject", ImVec2( 150, 30 ) ) )
                    d3d11::should_uninject = true;
                ImGui::EndTabItem( );
            }

            ImGui::EndTabBar( );
        }

        ImGui::End( );
    }

    auto Background( ) -> void { }
    auto AddMessage( const std::string& ) -> void { }
    auto Log( ) -> void { }
    void RenderConsole( ) { }

} // namespace Render
