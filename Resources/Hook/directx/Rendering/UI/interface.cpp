#include "../interface.hpp"

#include "../../../../config.hpp"
#include "../../../../../Core/Engine/Anvil/scimitar.hpp"
#include "../../../../../Core/Utils/memory.hpp"
#include "../../d3d11hook.hpp"

#include <Windows.h>

#include <cstdio>

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

        constexpr ImGuiColorEditFlags kMenuColorRgb =
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_InputRGB |
            ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaPreviewHalf |
            ImGuiColorEditFlags_PickerHueBar;

        constexpr ImGuiColorEditFlags kMenuColorRgba =
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_InputRGB |
            ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf |
            ImGuiColorEditFlags_PickerHueBar;

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

        void apply_menu_theme_once( ) noexcept
        {
            static bool s_applied = false;
            if ( s_applied )
                return;
            s_applied = true;

            ImGui::StyleColorsDark( );

            ImGuiStyle& st = ImGui::GetStyle( );
            st.WindowRounding = 5.f;
            st.FrameRounding = 4.f;
            st.GrabRounding = 3.f;
            st.TabRounding = 4.f;
            st.ScrollbarSize = 12.f;
            st.WindowPadding = ImVec2( 12.f, 12.f );
            st.FramePadding = ImVec2( 8.f, 4.f );
            st.ItemSpacing = ImVec2( 8.f, 6.f );

            ImVec4* c = st.Colors;
            const ImVec4 txt( 0.90f, 0.90f, 0.90f, 1.f );
            const ImVec4 txt_dim( 0.45f, 0.45f, 0.45f, 1.f );
            const ImVec4 blk( 0.06f, 0.06f, 0.06f, 0.98f );
            const ImVec4 gray_bg( 0.14f, 0.14f, 0.14f, 1.f );
            const ImVec4 gray_frame( 0.18f, 0.18f, 0.18f, 1.f );
            const ImVec4 gray_hi( 0.28f, 0.28f, 0.28f, 1.f );
            const ImVec4 red( 0.72f, 0.14f, 0.14f, 1.f );
            const ImVec4 red_hi( 0.88f, 0.22f, 0.22f, 1.f );
            const ImVec4 red_active( 1.f, 0.32f, 0.28f, 1.f );
            const ImVec4 red_tabs( 0.42f, 0.10f, 0.10f, 1.f );

            c[ ImGuiCol_Text ] = txt;
            c[ ImGuiCol_TextDisabled ] = txt_dim;
            c[ ImGuiCol_WindowBg ] = blk;
            c[ ImGuiCol_ChildBg ] = gray_bg;
            c[ ImGuiCol_PopupBg ] = ImVec4( 0.10f, 0.10f, 0.10f, 0.98f );
            c[ ImGuiCol_Border ] = ImVec4( 0.38f, 0.22f, 0.22f, 0.55f );
            c[ ImGuiCol_BorderShadow ] = ImVec4( 0.f, 0.f, 0.f, 0.f );
            c[ ImGuiCol_FrameBg ] = gray_frame;
            c[ ImGuiCol_FrameBgHovered ] = ImVec4( 0.32f, 0.18f, 0.18f, 1.f );
            c[ ImGuiCol_FrameBgActive ] = ImVec4( 0.40f, 0.14f, 0.14f, 1.f );
            c[ ImGuiCol_TitleBg ] = ImVec4( 0.08f, 0.08f, 0.08f, 1.f );
            c[ ImGuiCol_TitleBgActive ] = red_tabs;
            c[ ImGuiCol_TitleBgCollapsed ] = ImVec4( 0.08f, 0.08f, 0.08f, 0.85f );
            c[ ImGuiCol_MenuBarBg ] = gray_bg;
            c[ ImGuiCol_ScrollbarBg ] = blk;
            c[ ImGuiCol_ScrollbarGrab ] = gray_hi;
            c[ ImGuiCol_ScrollbarGrabHovered ] = red_hi;
            c[ ImGuiCol_ScrollbarGrabActive ] = red_active;
            c[ ImGuiCol_CheckMark ] = red_hi;
            c[ ImGuiCol_SliderGrab ] = red;
            c[ ImGuiCol_SliderGrabActive ] = red_active;
            c[ ImGuiCol_Button ] = gray_hi;
            c[ ImGuiCol_ButtonHovered ] = red;
            c[ ImGuiCol_ButtonActive ] = red_active;
            c[ ImGuiCol_Header ] = ImVec4( 0.35f, 0.14f, 0.14f, 0.65f );
            c[ ImGuiCol_HeaderHovered ] = ImVec4( 0.55f, 0.16f, 0.16f, 1.f );
            c[ ImGuiCol_HeaderActive ] = red_tabs;
            c[ ImGuiCol_Separator ] = ImVec4( 0.35f, 0.35f, 0.35f, 0.55f );
            c[ ImGuiCol_SeparatorHovered ] = red_hi;
            c[ ImGuiCol_SeparatorActive ] = red_active;
            c[ ImGuiCol_ResizeGrip ] = ImVec4( 0.5f, 0.15f, 0.15f, 0.35f );
            c[ ImGuiCol_ResizeGripHovered ] = ImVec4( 0.75f, 0.2f, 0.2f, 0.65f );
            c[ ImGuiCol_ResizeGripActive ] = red_active;
            c[ ImGuiCol_Tab ] = ImVec4( 0.16f, 0.16f, 0.16f, 1.f );
            c[ ImGuiCol_TabHovered ] = ImVec4( 0.55f, 0.18f, 0.18f, 1.f );
            c[ ImGuiCol_TabActive ] = red_tabs;
            c[ ImGuiCol_TabUnfocused ] = ImVec4( 0.12f, 0.12f, 0.12f, 1.f );
            c[ ImGuiCol_TabUnfocusedActive ] = ImVec4( 0.32f, 0.12f, 0.12f, 1.f );
            c[ ImGuiCol_PlotLines ] = red_hi;
            c[ ImGuiCol_PlotLinesHovered ] = red_active;
            c[ ImGuiCol_PlotHistogram ] = red;
            c[ ImGuiCol_PlotHistogramHovered ] = red_hi;
            c[ ImGuiCol_TableHeaderBg ] = ImVec4( 0.12f, 0.12f, 0.12f, 1.f );
            c[ ImGuiCol_TableBorderStrong ] = ImVec4( 0.35f, 0.22f, 0.22f, 1.f );
            c[ ImGuiCol_TableBorderLight ] = ImVec4( 0.28f, 0.28f, 0.28f, 0.55f );
            c[ ImGuiCol_TableRowBg ] = ImVec4( 0.f, 0.f, 0.f, 0.f );
            c[ ImGuiCol_TableRowBgAlt ] = ImVec4( 1.f, 1.f, 1.f, 0.03f );
            c[ ImGuiCol_TextSelectedBg ] = ImVec4( 0.65f, 0.15f, 0.15f, 0.55f );
            c[ ImGuiCol_DragDropTarget ] = ImVec4( 0.9f, 0.25f, 0.25f, 0.95f );
            c[ ImGuiCol_NavHighlight ] = ImVec4( 0.85f, 0.25f, 0.25f, 0.8f );
            c[ ImGuiCol_NavWindowingHighlight ] = ImVec4( 1.f, 0.35f, 0.35f, 0.75f );
            c[ ImGuiCol_NavWindowingDimBg ] = ImVec4( 0.f, 0.f, 0.f, 0.55f );
            c[ ImGuiCol_ModalWindowDimBg ] = ImVec4( 0.f, 0.f, 0.f, 0.62f );
        }

    } // namespace

    auto user_interface( ) -> void
    {
        if ( !menu_open ) return;

        apply_menu_theme_once( );

        ImGui::SetNextWindowSize( ImVec2( 480.f, 680.f ), ImGuiCond_FirstUseEver );
        ImGui::Begin( "IceBox Public", nullptr, ImGuiWindowFlags_NoCollapse );

        if ( ImGui::BeginTabBar( "MainTabs" ) ) {
            if ( ImGui::BeginTabItem( "Visuals" ) ) {
                ImGui::Checkbox( "Trace Lines", &visuals::Tracers );
                ImGui::SameLine( );
                ImGui::SetNextItemWidth( 240.f );
                ImGui::Combo( "##TracerPos", &visuals::TracerSelected, visuals::TracerPos, IM_ARRAYSIZE( visuals::TracerPos ) );
                ImGui::SameLine( );
                ImGui::ColorEdit3( "##TracerColor", &visuals::TracerColor.x, kMenuColorRgb );
                ImGui::Checkbox( "Skeleton", &visuals::Skeleton );
                ImGui::SameLine( );
                ImGui::ColorEdit3( "##SkeletonColor", &visuals::SkeletonColor.x, kMenuColorRgb );
                ImGui::SliderFloat( "Skeleton Thickness", &visuals::SkeletonThickness, 0.5f, 5.0f, "%.1f" );
                ImGui::Checkbox( "Skeleton VisCheck", &visuals::SkeletonVisCheck );
                ImGui::Checkbox( "Enemy outlines", &visuals::EnemyOutline );
                ImGui::SameLine( );
                ImGui::ColorEdit4( "##EnemyOutlineCol", &visuals::EnemyOutlineColor.x, kMenuColorRgba );
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

            if ( ImGui::BeginTabItem( "Aimbot" ) ) {
                ImGui::Checkbox( "Silent aim", &visuals::SilentAim );
                ImGui::BeginDisabled( !visuals::SilentAim );
                ImGui::Checkbox( "Silent aim vis check", &visuals::SilentAimVisCheck );
                ImGui::SliderFloat( "Silent aim FOV", &visuals::SilentAimFovDegrees, 1.f, 60.f, "%.1f deg" );
                ImGui::Checkbox( "Silent aim FOV circle", &visuals::SilentAimShowFovCircle );
                ImGui::Checkbox( "Fill FOV circle", &visuals::SilentAimCircleFilled );
                ImGui::SliderFloat( "FOV circle opacity", &visuals::SilentAimCircleOpacity, 0.f, 1.f, "%.2f" );
                ImGui::ColorEdit4( "FOV circle color", &visuals::SilentAimCircleColor.x, kMenuColorRgba );
                ImGui::EndDisabled( );

                ImGui::Separator( );
                ImGui::Checkbox( "Ragebot", &visuals::RageBot );
                if ( visuals::RageBot ) {
                    ImGui::Indent( );
                    if ( !visuals::RageBotVisCheck && !visuals::RageBotPenCheck )
                        ImGui::TextWrapped( "Enable Vischeck and/or Pencheck for Ragebot to fire." );
                    ImGui::Checkbox( "Ragebot Vischeck", &visuals::RageBotVisCheck );
                    ImGui::Checkbox( "Ragebot Pencheck", &visuals::RageBotPenCheck );
                    ImGui::Unindent( );
                }

                ImGui::EndTabItem( );
            }

            if ( ImGui::BeginTabItem( "world modulation" ) ) {
                ImGui::Checkbox( "World Modulation", &world_modulation::enabled );
                if ( world_modulation::enabled && IceBox::world_modulation_values_differ_from_frozen_snapshot( ) ) {
                    ImGui::SameLine( );
                    if ( ImGui::SmallButton( "Reset to game##wm_rst" ) )
                        IceBox::world_modulation_reset_all_to_frozen_snapshot( );
                }

                ImGui::Checkbox( "Better light", &visuals::BetterLight );

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
                            ImGui::ColorEdit3( "##pick", &world_modulation::light_rgb.x, kMenuColorRgb );
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
                            ImGui::ColorEdit3( "##pick", &world_modulation::reflection_rgb.x, kMenuColorRgb );
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
                            ImGui::ColorEdit3( "##pick", &world_modulation::highlight_rgb.x, kMenuColorRgb );
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
                            ImGui::ColorEdit3( "##pick", &world_modulation::top_bottom_rgb.x, kMenuColorRgb );
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
                            ImGui::ColorEdit3( "##pick", &world_modulation::global_illum_rgb.x, kMenuColorRgb );
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
                            ImGui::ColorEdit3( "##pick", &world_modulation::magic_rgb.x, kMenuColorRgb );
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
                            ImGui::ColorEdit3( "##pick", &world_modulation::top_bottom_fog_rgb.x, kMenuColorRgb );
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
                            ImGui::ColorEdit3( "##pick", &world_modulation::sky_rgb.x, kMenuColorRgb );
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
                            ImGui::ColorEdit3( "##pick", &world_modulation::highlight2_rgb.x, kMenuColorRgb );
                        }
                        ImGui::PopID( );

                        ImGui::EndTable( );
                    }
                }

                ImGui::Separator( );
                ImGui::TextUnformatted( "World edit" );
                ImGui::Checkbox( "Patch world XMM vector##world_edit_hook", &world_edit::enabled );
                ImGui::BeginDisabled( !world_edit::enabled );
                ImGui::SliderFloat( "Clarity##world_edit", &world_edit::clarity, 0.f, 5.f, "%.2f" );
                ImGui::SliderFloat( "Saturation##world_edit", &world_edit::saturation, 0.f, 5.f, "%.2f" );
                ImGui::SliderFloat( "Brightness##world_edit", &world_edit::brightness, 0.f, 5.f, "%.2f" );
                ImGui::SliderFloat( "Illumination##world_edit", &world_edit::illumination, 0.f, 16.f, "%.2f" );
                ImGui::EndDisabled( );

                ImGui::Separator( );
                ImGui::TextUnformatted( "Player glow modulation" );
                ImGui::Checkbox( "Enable##player_glow", &world_modulation::player_glow_enabled );
                if ( world_modulation::player_glow_enabled ) {
                    const char* player_glow_type_labels[ ] = { "Solid", "Outline" };
                    ImGui::SetNextItemWidth( 200.f );
                    ImGui::Combo( "Glow type",
                                  &world_modulation::player_glow_type,
                                  player_glow_type_labels,
                                  IM_ARRAYSIZE( player_glow_type_labels ) );
                    ImGui::SetNextItemWidth( -FLT_MIN );
                    ImGui::ColorEdit3( "Color##player_glow_rgb", &world_modulation::player_glow_rgb.x, kMenuColorRgb );
                }

                ImGui::EndTabItem( );
            }

            if ( ImGui::BeginTabItem( "Misc" ) ) {
                ImGui::Checkbox( "Long melee", &visuals::LongMelee );
                ImGui::Checkbox( "Third person", &visuals::ThirdPerson );

                static bool s_third_person_listen = false;
                static double s_third_person_listen_after = 0.0;
                static bool s_third_person_wait_mouse_release = false;
                if ( !visuals::ThirdPerson ) {
                    s_third_person_listen = false;
                    s_third_person_wait_mouse_release = false;
                }

                if ( visuals::ThirdPerson ) {
                    ImGui::Indent( );

                    char tp_vk_buf[ 64 ];
                    const char* tp_vk_text = jitter_peek_vk_label( visuals::ThirdPersonVk, tp_vk_buf );

                    const char* tp_btn_label = s_third_person_listen ? "" : tp_vk_text;
                    const ImVec2 tp_hotkey_btn_size( 76.f, 22.f );

                    ImGui::PushID( "ThirdPersonHotkey" );
                    ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.40f, 0.40f, 0.43f, 1.00f ) );
                    ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.48f, 0.48f, 0.51f, 1.00f ) );
                    ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.34f, 0.34f, 0.37f, 1.00f ) );
                    ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.90f, 0.90f, 0.92f, 1.00f ) );
                    if ( ImGui::Button( tp_btn_label, tp_hotkey_btn_size ) ) {
                        s_third_person_listen = true;
                        s_third_person_listen_after = ImGui::GetTime( ) + 0.18;
                        s_third_person_wait_mouse_release = true;
                    }
                    ImGui::PopStyleColor( 4 );
                    ImGui::PopID( );

                    if ( s_third_person_listen ) {
                        const double now_tp = ImGui::GetTime( );
                        if ( now_tp >= s_third_person_listen_after ) {
                            if ( ( GetAsyncKeyState( VK_ESCAPE ) & 0x8000 ) != 0 ) {
                                s_third_person_listen = false;
                                s_third_person_wait_mouse_release = false;
                            } else {
                                if ( s_third_person_wait_mouse_release ) {
                                    if ( !jitter_peek_any_mouse_down( ) )
                                        s_third_person_wait_mouse_release = false;
                                }

                                if ( !s_third_person_wait_mouse_release ) {
                                    for ( int vk = 1; vk < 256; ++vk ) {
                                        if ( ( GetAsyncKeyState( vk ) & 0x8000 ) != 0 ) {
                                            visuals::ThirdPersonVk = vk;
                                            s_third_person_listen = false;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    ImGui::Unindent( );
                }

                ImGui::Checkbox( "Raycast debug (console)", &visuals::RaycastClosestDebug );
                ImGui::Checkbox( "Run and shoot", &visuals::RunAndShoot );
                ImGui::Checkbox( "Unlock all", &visuals::UnlockAllMidHook );
                ImGui::Checkbox( "No spread", &visuals::NoSpread );
                ImGui::Checkbox( "No recoil", &visuals::NoRecoil );
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

                ImGui::Separator( );
                ImGui::TextUnformatted( "Dust" );
                ImGui::SliderFloat( "Dust radius", &visuals::DustSpawnRadius, 0.25f, 15.f, "%.2f" );
                ImGui::ColorEdit4( "Dust color", &visuals::DustSpawnColor.x, kMenuColorRgba );
                if ( ImGui::Button( "Spawn dust (local origin)", ImVec2( 220.f, 0.f ) ) )
                    spawn_dust_at_local_origin( );

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
