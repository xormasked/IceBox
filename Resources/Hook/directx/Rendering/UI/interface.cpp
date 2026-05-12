#include "../interface.hpp"

#include "../../../../config.hpp"
#include "../../d3d11hook.hpp"

#include <Windows.h>

#include <cstdio>

namespace Render {

    namespace {

        auto jitter_peek_vk_label( int vk, char ( &buf )[ 64 ] ) -> const char*
        {
            if ( vk >= '0' && vk <= '9' ) {
                std::snprintf( buf, sizeof( buf ), "%c", vk );
                return buf;
            }
            if ( vk >= 'A' && vk <= 'Z' ) {
                std::snprintf( buf, sizeof( buf ), "%c", vk );
                return buf;
            }
            if ( vk >= 'a' && vk <= 'z' ) {
                std::snprintf( buf, sizeof( buf ), "%c", vk - 32 );
                return buf;
            }

            switch ( vk ) {
            case VK_SPACE:
                return "Space";
            case VK_TAB:
                return "Tab";
            case VK_RETURN:
                return "Enter";
            case VK_SHIFT:
                return "Shift";
            case VK_CONTROL:
                return "Ctrl";
            case VK_MENU:
                return "Alt";
            case VK_ESCAPE:
                return "Escape";
            case VK_BACK:
                return "Backspace";
            case VK_DELETE:
                return "Delete";
            case VK_INSERT:
                return "Insert";
            case VK_HOME:
                return "Home";
            case VK_END:
                return "End";
            case VK_PRIOR:
                return "Page Up";
            case VK_NEXT:
                return "Page Down";
            case VK_LEFT:
                return "Left";
            case VK_RIGHT:
                return "Right";
            case VK_UP:
                return "Up";
            case VK_DOWN:
                return "Down";
            case VK_LBUTTON:
                return "Mouse L";
            case VK_RBUTTON:
                return "Mouse R";
            case VK_MBUTTON:
                return "Mouse M";
            case VK_XBUTTON1:
                return "Mouse 4";
            case VK_XBUTTON2:
                return "Mouse 5";
            default:
                std::snprintf( buf, sizeof( buf ), "VK 0x%02X", vk );
                return buf;
            }
        }

        bool jitter_peek_any_mouse_down( )
        {
            return ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 ) != 0 || ( GetAsyncKeyState( VK_RBUTTON ) & 0x8000 ) != 0 ||
                   ( GetAsyncKeyState( VK_MBUTTON ) & 0x8000 ) != 0 || ( GetAsyncKeyState( VK_XBUTTON1 ) & 0x8000 ) != 0 ||
                   ( GetAsyncKeyState( VK_XBUTTON2 ) & 0x8000 ) != 0;
        }

    } // namespace

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
                ImGui::Checkbox( "Unlock all", &visuals::UnlockAllMidHook );
                ImGui::Checkbox( "Jitter peek", &visuals::JitterPeek );

                static bool s_jitter_peek_listen = false;
                static double s_jitter_peek_listen_after = 0.0;
                static bool s_jitter_peek_wait_mouse_release = false;
                if ( !visuals::JitterPeek ) {
                    s_jitter_peek_listen = false;
                    s_jitter_peek_wait_mouse_release = false;
                }

                if ( visuals::JitterPeek ) {
                    ImGui::Indent( );

                    char vk_buf[ 64 ];
                    const char* vk_text = jitter_peek_vk_label( visuals::JitterPeekVk, vk_buf );

                    const char* btn_label = s_jitter_peek_listen ? "" : vk_text;
                    const ImVec2 hotkey_btn_size( 76.f, 22.f );

                    ImGui::PushID( "JitterPeekHotkey" );
                    ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.40f, 0.40f, 0.43f, 1.00f ) );
                    ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.48f, 0.48f, 0.51f, 1.00f ) );
                    ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.34f, 0.34f, 0.37f, 1.00f ) );
                    ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.90f, 0.90f, 0.92f, 1.00f ) );
                    if ( ImGui::Button( btn_label, hotkey_btn_size ) ) {
                        s_jitter_peek_listen = true;
                        s_jitter_peek_listen_after = ImGui::GetTime( ) + 0.18;
                        s_jitter_peek_wait_mouse_release = true;
                    }
                    ImGui::PopStyleColor( 4 );
                    ImGui::PopID( );

                    if ( s_jitter_peek_listen ) {
                        const double now = ImGui::GetTime( );
                        if ( now >= s_jitter_peek_listen_after ) {
                            if ( ( GetAsyncKeyState( VK_ESCAPE ) & 0x8000 ) != 0 ) {
                                s_jitter_peek_listen = false;
                                s_jitter_peek_wait_mouse_release = false;
                            } else {
                                if ( s_jitter_peek_wait_mouse_release ) {
                                    if ( !jitter_peek_any_mouse_down( ) )
                                        s_jitter_peek_wait_mouse_release = false;
                                }

                                if ( !s_jitter_peek_wait_mouse_release ) {
                                    for ( int vk = 1; vk < 256; ++vk ) {
                                        if ( ( GetAsyncKeyState( vk ) & 0x8000 ) != 0 ) {
                                            visuals::JitterPeekVk = vk;
                                            s_jitter_peek_listen = false;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    ImGui::SliderInt( "Delay (ms)", &visuals::JitterPeekDelayMs, 10, 150 );

                    ImGui::Unindent( );
                }
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
