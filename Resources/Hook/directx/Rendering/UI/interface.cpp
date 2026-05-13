#include "../interface.hpp"

#include "../../../../config.hpp"
#include "../../../../../Core/Engine/Anvil/scimitar.hpp"
#include "../../../../../Core/Utils/memory.hpp"
#include "../../d3d11hook.hpp"

#include <Windows.h>

#include <cstdio>
#include <iostream>

#include "../../../../../Core/Engine/Anvil/AnvilNext LightingEngine/R6LightEngine.hpp"
#include "../../../../../ice/IceBox.hpp"

namespace Render {

    namespace {

        auto jitter_peek_vk_label( int vk, char ( &buf )[ 64 ] ) -> const char*
        {
            if ( vk >= '0' && vk <= '9' ) {
                sprintf_s( buf, sizeof( buf ), "%c", vk );
                return buf;
            }
            if ( vk >= 'A' && vk <= 'Z' ) {
                sprintf_s( buf, sizeof( buf ), "%c", vk );
                return buf;
            }
            if ( vk >= 'a' && vk <= 'z' ) {
                sprintf_s( buf, sizeof( buf ), "%c", vk - 32 );
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
                sprintf_s( buf, sizeof( buf ), "VK 0x%02X", vk );
                return buf;
            }
        }

        bool jitter_peek_any_mouse_down( )
        {
            return ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 ) != 0 || ( GetAsyncKeyState( VK_RBUTTON ) & 0x8000 ) != 0 ||
                   ( GetAsyncKeyState( VK_MBUTTON ) & 0x8000 ) != 0 || ( GetAsyncKeyState( VK_XBUTTON1 ) & 0x8000 ) != 0 ||
                   ( GetAsyncKeyState( VK_XBUTTON2 ) & 0x8000 ) != 0;
        }

        void spawn_dust_at_local_origin( )
        {
            auto* const gm = Scimitar::game_manager::get( );
            if ( !Memory::valid_pointer( gm ) )
                return;
            auto* const lc = gm->get_local_controller( );
            if ( !Memory::valid_pointer( lc ) )
                return;
            auto* const pawn = lc->pawn_decrypt( );
            if ( !Memory::valid_pointer( pawn ) )
                return;
            auto* const ent = pawn->entity_decrypt( );
            if ( !Memory::valid_pointer( ent ) )
                return;

            const ubiVector4 pos = ent->Origin4( );
            const ubiVector4 col(
                visuals::DustSpawnColor.x,
                visuals::DustSpawnColor.y,
                visuals::DustSpawnColor.z,
                visuals::DustSpawnColor.w );

            Scimitar::add_dust( pos, visuals::DustSpawnRadius, col );
        }

        struct LightingCmpRow {
            const char* name;
            AnvilNextLightingEngine::LightingComponent id;
        };

        void print_all_lighting_addresses( )
        {
            namespace ANLE = AnvilNextLightingEngine;
            static const LightingCmpRow k_rows[] = {
                { "LCLightBlue", ANLE::LCLightBlue },
                { "LCLightGreen", ANLE::LCLightGreen },
                { "LCLightRed", ANLE::LCLightRed },
                { "LCLightRotation", ANLE::LCLightRotation },
                { "LCLightPower", ANLE::LCLightPower },
                { "LCReflectionRed", ANLE::LCReflectionRed },
                { "LCReflectionGreen", ANLE::LCReflectionGreen },
                { "LCReflectionBlue", ANLE::LCReflectionBlue },
                { "LCHighlightRed", ANLE::LCHighlightRed },
                { "LCHighlightGreen", ANLE::LCHighlightGreen },
                { "LCHighlightBlue", ANLE::LCHighlightBlue },
                { "LCTopBottomRed", ANLE::LCTopBottomRed },
                { "LCTopBottomGreen", ANLE::LCTopBottomGreen },
                { "LCTopBottomBlue", ANLE::LCTopBottomBlue },
                { "LCGlobalEluminationRed", ANLE::LCGlobalEluminationRed },
                { "LCGlobalEluminationGreen", ANLE::LCGlobalEluminationGreen },
                { "LCGlobalEluminationBlue", ANLE::LCGlobalEluminationBlue },
                { "LCMagicRRB", ANLE::LCMagicRRB },
                { "LCMagicGGP", ANLE::LCMagicGGP },
                { "LCMagicBYB", ANLE::LCMagicBYB },
                { "LCTopBottomFogRed", ANLE::LCTopBottomFogRed },
                { "LCTopBottomFogGreen", ANLE::LCTopBottomFogGreen },
                { "LCTopBottomFogBlue", ANLE::LCTopBottomFogBlue },
                { "LCSkyColorRed", ANLE::LCSkyColorRed },
                { "LCSkyColorGreen", ANLE::LCSkyColorGreen },
                { "LCSkyColorBlue", ANLE::LCSkyColorBlue },
                { "LCHighlight2Red", ANLE::LCHighlight2Red },
                { "LCHighlight2Green", ANLE::LCHighlight2Green },
                { "LCHighlight2Blue", ANLE::LCHighlight2Blue },
            };

            std::cout << "[Lighting] all get_lighting_component addresses:\n";
            for ( const auto& row : k_rows ) {
                const uintptr_t addr = ANLE::get_lighting_component( row.id );
                std::cout << "  " << row.name << " (off 0x" << std::hex
                          << static_cast< uint32_t >( row.id ) << ") -> 0x" << addr << std::dec << '\n';
            }
            std::cout << std::flush;
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

            if ( ImGui::BeginTabItem( "world modulation" ) ) {
                constexpr ImGuiColorEditFlags wf_pick =
                    ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_DisplayRGB;

                ImGui::Checkbox( "World Modulation", &world_modulation::enabled );
                if ( world_modulation::enabled && IceBox::world_modulation_values_differ_from_frozen_snapshot( ) ) {
                    ImGui::SameLine( );
                    if ( ImGui::SmallButton( "Reset to game##wm_rst" ) )
                        IceBox::world_modulation_reset_all_to_frozen_snapshot( );
                }

                if ( world_modulation::enabled ) {
                    ImGui::Spacing( );

                    if ( ImGui::BeginTable(
                             "wm_channels",
                             3,
                             ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV ) ) {
                        ImGui::TableSetupColumn( "##cb", ImGuiTableColumnFlags_WidthFixed, 28.f );
                        ImGui::TableSetupColumn( "label", ImGuiTableColumnFlags_WidthFixed, 248.f );
                        ImGui::TableSetupColumn( "##rgb", ImGuiTableColumnFlags_WidthFixed, 168.f );

                        ImGui::TableNextRow( );
                        ImGui::PushID( "wm_l" );
                        ImGui::TableSetColumnIndex( 0 );
                        ImGui::Checkbox( "##cb", &world_modulation::edit_light );
                        ImGui::TableSetColumnIndex( 1 );
                        ImGui::AlignTextToFramePadding( );
                        ImGui::TextUnformatted( "Light" );
                        ImGui::TableSetColumnIndex( 2 );
                        if ( world_modulation::edit_light ) {
                            ImGui::SetNextItemWidth( -FLT_MIN );
                            ImGui::ColorEdit3( "##pick", &world_modulation::light_rgb.x, wf_pick );
                        }
                        ImGui::PopID( );

                        ImGui::TableNextRow( );
                        ImGui::PushID( "wm_r" );
                        ImGui::TableSetColumnIndex( 0 );
                        ImGui::Checkbox( "##cb", &world_modulation::edit_reflection );
                        ImGui::TableSetColumnIndex( 1 );
                        ImGui::AlignTextToFramePadding( );
                        ImGui::TextUnformatted( "Reflection" );
                        ImGui::TableSetColumnIndex( 2 );
                        if ( world_modulation::edit_reflection ) {
                            ImGui::SetNextItemWidth( -FLT_MIN );
                            ImGui::ColorEdit3( "##pick", &world_modulation::reflection_rgb.x, wf_pick );
                        }
                        ImGui::PopID( );

                        ImGui::TableNextRow( );
                        ImGui::PushID( "wm_h" );
                        ImGui::TableSetColumnIndex( 0 );
                        ImGui::Checkbox( "##cb", &world_modulation::edit_highlight );
                        ImGui::TableSetColumnIndex( 1 );
                        ImGui::AlignTextToFramePadding( );
                        ImGui::TextUnformatted( "Highlight" );
                        ImGui::TableSetColumnIndex( 2 );
                        if ( world_modulation::edit_highlight ) {
                            ImGui::SetNextItemWidth( -FLT_MIN );
                            ImGui::ColorEdit3( "##pick", &world_modulation::highlight_rgb.x, wf_pick );
                        }
                        ImGui::PopID( );

                        ImGui::TableNextRow( );
                        ImGui::PushID( "wm_tb" );
                        ImGui::TableSetColumnIndex( 0 );
                        ImGui::Checkbox( "##cb", &world_modulation::edit_top_bottom );
                        ImGui::TableSetColumnIndex( 1 );
                        ImGui::AlignTextToFramePadding( );
                        ImGui::TextUnformatted( "Top / bottom" );
                        ImGui::TableSetColumnIndex( 2 );
                        if ( world_modulation::edit_top_bottom ) {
                            ImGui::SetNextItemWidth( -FLT_MIN );
                            ImGui::ColorEdit3( "##pick", &world_modulation::top_bottom_rgb.x, wf_pick );
                        }
                        ImGui::PopID( );

                        ImGui::TableNextRow( );
                        ImGui::PushID( "wm_gi" );
                        ImGui::TableSetColumnIndex( 0 );
                        ImGui::Checkbox( "##cb", &world_modulation::edit_global_illum );
                        ImGui::TableSetColumnIndex( 1 );
                        ImGui::AlignTextToFramePadding( );
                        ImGui::TextUnformatted( "Global illumination" );
                        ImGui::TableSetColumnIndex( 2 );
                        if ( world_modulation::edit_global_illum ) {
                            ImGui::SetNextItemWidth( -FLT_MIN );
                            ImGui::ColorEdit3( "##pick", &world_modulation::global_illum_rgb.x, wf_pick );
                        }
                        ImGui::PopID( );

                        ImGui::TableNextRow( );
                        ImGui::PushID( "wm_m" );
                        ImGui::TableSetColumnIndex( 0 );
                        ImGui::Checkbox( "##cb", &world_modulation::edit_magic );
                        ImGui::TableSetColumnIndex( 1 );
                        ImGui::AlignTextToFramePadding( );
                        ImGui::TextUnformatted( "Magic (RRB / GGP / BYB)" );
                        ImGui::TableSetColumnIndex( 2 );
                        if ( world_modulation::edit_magic ) {
                            ImGui::SetNextItemWidth( -FLT_MIN );
                            ImGui::ColorEdit3( "##pick", &world_modulation::magic_rgb.x, wf_pick );
                        }
                        ImGui::PopID( );

                        ImGui::TableNextRow( );
                        ImGui::PushID( "wm_tbf" );
                        ImGui::TableSetColumnIndex( 0 );
                        ImGui::Checkbox( "##cb", &world_modulation::edit_top_bottom_fog );
                        ImGui::TableSetColumnIndex( 1 );
                        ImGui::AlignTextToFramePadding( );
                        ImGui::TextUnformatted( "Top / bottom fog" );
                        ImGui::TableSetColumnIndex( 2 );
                        if ( world_modulation::edit_top_bottom_fog ) {
                            ImGui::SetNextItemWidth( -FLT_MIN );
                            ImGui::ColorEdit3( "##pick", &world_modulation::top_bottom_fog_rgb.x, wf_pick );
                        }
                        ImGui::PopID( );

                        ImGui::TableNextRow( );
                        ImGui::PushID( "wm_sky" );
                        ImGui::TableSetColumnIndex( 0 );
                        ImGui::Checkbox( "##cb", &world_modulation::edit_sky );
                        ImGui::TableSetColumnIndex( 1 );
                        ImGui::AlignTextToFramePadding( );
                        ImGui::TextUnformatted( "Sky color" );
                        ImGui::TableSetColumnIndex( 2 );
                        if ( world_modulation::edit_sky ) {
                            ImGui::SetNextItemWidth( -FLT_MIN );
                            ImGui::ColorEdit3( "##pick", &world_modulation::sky_rgb.x, wf_pick );
                        }
                        ImGui::PopID( );

                        ImGui::TableNextRow( );
                        ImGui::PushID( "wm_h2" );
                        ImGui::TableSetColumnIndex( 0 );
                        ImGui::Checkbox( "##cb", &world_modulation::edit_highlight2 );
                        ImGui::TableSetColumnIndex( 1 );
                        ImGui::AlignTextToFramePadding( );
                        ImGui::TextUnformatted( "Highlight 2" );
                        ImGui::TableSetColumnIndex( 2 );
                        if ( world_modulation::edit_highlight2 ) {
                            ImGui::SetNextItemWidth( -FLT_MIN );
                            ImGui::ColorEdit3( "##pick", &world_modulation::highlight2_rgb.x, wf_pick );
                        }
                        ImGui::PopID( );

                        ImGui::EndTable( );
                    }
                }

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
                ImGui::TextUnformatted( "Dust" );
                ImGui::SliderFloat( "Dust radius", &visuals::DustSpawnRadius, 0.25f, 15.f, "%.2f" );
                ImGui::ColorEdit4( "Dust color", &visuals::DustSpawnColor.x,
                                     ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB );
                if ( ImGui::Button( "Spawn dust (local origin)", ImVec2( 220.f, 0.f ) ) )
                    spawn_dust_at_local_origin( );

                ImGui::Separator( );
                if ( ImGui::Button( "Print lighting component address", ImVec2( 220.f, 0.f ) ) ) {
                    const uintptr_t addr =
                        AnvilNextLightingEngine::get_lighting_component( AnvilNextLightingEngine::LCMagicRRB );
                    std::cout << "[Lighting] get_lighting_component -> 0x" << std::hex << addr << std::dec << std::endl;
                }

                if ( ImGui::Button( "Print all lighting component addresses", ImVec2( 220.f, 0.f ) ) )
                    print_all_lighting_addresses( );

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
