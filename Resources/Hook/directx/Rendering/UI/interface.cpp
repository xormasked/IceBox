#include "../interface.hpp"

#include "../../../../config.hpp"
#include "../../d3d11hook.hpp"
#include "menu_design_functions.hpp"
#include "menu_others.hpp"

#include <Windows.h>

#include <cmath>
#include <string>

#include "../../../../../ice/IceBox.hpp"

namespace Render {

    auto user_interface( ) -> void
    {
        if ( !menu_open ) {
            MenuDesign::slider_reveal_mark_menu_hidden( );
            return;
        }

        using namespace MenuDesign;

        apply_menu_theme_once( );

        ImGui::SetNextWindowSize( ImVec2( 760.f, 740.f ), ImGuiCond_FirstUseEver );
        ImGui::Begin( "IceBox Public", nullptr, ImGuiWindowFlags_NoCollapse );

        if ( ImGui::BeginTabBar( "MainTabs" ) ) {
            menu_draw_tab_strip_accent_band( 2.f );

            if ( ImGui::BeginTabItem( "VISUALS" ) ) {
                const TabSliderRevealFrame slid_vis =
                    slider_reveal_poll_frame( SliderRevealTab::Visuals, true );
                ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( k_quad_col_gutter, k_quad_row_gutter ) );
                const MenuQuadDims vs_q = menu_quad_dims( );

                ImGui::BeginGroup( );
                begin_menu_child( "vis_q_tl", vs_q.cell.x, k_child_border );
                menu_panel_heading( "ESP" );
                ImGui::Checkbox( "Skeleton ESP", &visuals::Skeleton );
                if ( visuals::Skeleton ) {
                    menu_slider_float_reveal(
                        SliderRevealTab::Visuals,
                        slid_vis,
                        "Skeleton Thickness",
                        &visuals::SkeletonThickness,
                        0.5f,
                        5.0f,
                        "%.1f" );
                }
                ImGui::Checkbox( "Box ESP", &visuals::BoxEsp );
                if ( visuals::BoxEsp ) {
                    menu_slider_float_reveal(
                        SliderRevealTab::Visuals,
                        slid_vis,
                        "Box thickness",
                        &visuals::BoxEspThickness,
                        0.5f,
                        5.0f,
                        "%.1f" );
                }
                if ( visuals::Skeleton || visuals::BoxEsp ) {
                    ImGui::Checkbox( "ESP vischeck", &visuals::SkeletonVisCheck );
                    if ( visuals::SkeletonVisCheck ) {
                        menu_text_rgb_row_right_pick(
                            "##sk_vis", "visible", &visuals::SkeletonVisVisibleColor.x, kMenuColorRgb );
                        menu_text_rgb_row_right_pick( "##sk_invis", "not visible", &visuals::SkeletonVisInvisColor.x,
                            kMenuColorRgb );
                    }
                    else {
                        menu_text_rgb_row_right_pick(
                            "##sk_base_col", "ESP color", &visuals::SkeletonColor.x, kMenuColorRgb );
                    }
                }
                ImGui::Spacing( );
                menu_panel_heading_divider_strip( k_menu_esp_section_divider_fade_width_px );
                ImGui::Spacing( );
                menu_checkbox_rgb_row_right_pick(
                    "##trace_lines_pick", "Tracers", &visuals::Tracers, &visuals::TracerColor.x, kMenuColorRgb );
                ImGui::AlignTextToFramePadding( );
                ImGui::TextUnformatted( "Tracer source" );
                ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, k_menu_combo_frame_pad );
                ImGui::PushItemWidth( menu_combo_desired_width_px( ) );
                ImGui::SetNextComboDropdownLayoutHeight(
                    menu_combo_list_reserve_height_px( IM_ARRAYSIZE( visuals::TracerPos ) ) );
                ImGui::Combo( "##TracerPos", &visuals::TracerSelected, visuals::TracerPos, IM_ARRAYSIZE( visuals::TracerPos ) );
                ImGui::PopItemWidth( );
                ImGui::PopStyleVar( );
                ImGui::Spacing( );
                menu_checkbox_rgba_row_right_pick(
                    "##outline_pick", "Outlines", &visuals::EnemyOutline, &visuals::EnemyOutlineColor.x, kMenuColorRgba );
                end_menu_child( "vis_q_tl" );
                begin_menu_child( "vis_q_bl", vs_q.cell.x, k_child_border );
                menu_panel_heading( "CAMERA" );
                ImGui::Checkbox( "Aspect ratio (camera +0x128)", &visuals::AspectRatioHook );
                menu_slider_float_reveal(
                    SliderRevealTab::Visuals, slid_vis, "Aspect ratio", &visuals::AspectRatio, 0.1f, 2.0f, "%.2f" );
                end_menu_child( "vis_q_bl" );
                ImGui::EndGroup( );

                ImGui::SameLine( 0.f, k_quad_col_gutter );

                ImGui::BeginGroup( );
                begin_menu_child( "vis_q_tr", vs_q.cell.x, k_child_border );
                menu_panel_heading( "FOV" );
                ImGui::Checkbox( "Fov", &visuals::FovEnabled );
                menu_slider_float_reveal(
                    SliderRevealTab::Visuals,
                    slid_vis,
                    "Eye Fov",
                    &visuals::EyeFovDegrees,
                    20.f,
                    120.f,
                    "%.0f deg" );
                menu_slider_float_reveal(
                    SliderRevealTab::Visuals,
                    slid_vis,
                    "Viewmodel Fov",
                    &visuals::ViewmodelFovDegrees,
                    20.f,
                    120.f,
                    "%.0f deg" );
                end_menu_child( "vis_q_tr" );
                ImGui::EndGroup( );

                ImGui::PopStyleVar( );
                ImGui::EndTabItem( );
            }
            else {
                slider_reveal_poll_frame( SliderRevealTab::Visuals, false );
            }

            if ( ImGui::BeginTabItem( "AIMBOT" ) ) {
                const TabSliderRevealFrame slid_aim = slider_reveal_poll_frame( SliderRevealTab::Aimbot, true );
                ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( k_quad_col_gutter, k_quad_row_gutter ) );
                const MenuQuadDims aim_q = menu_quad_dims( );

                ImGui::BeginGroup( );
                begin_menu_child( "aim_q_tl", aim_q.cell.x, k_child_border );
                menu_panel_heading( "SILENT AIM" );
                ImGui::Checkbox( "Silent aim", &visuals::SilentAim );
                ImGui::Checkbox( "Silent aim vis check", &visuals::SilentAimVisCheck );
                menu_slider_float_reveal(
                    SliderRevealTab::Aimbot,
                    slid_aim,
                    "Silent aim FOV",
                    &visuals::SilentAimFovDegrees,
                    1.f,
                    60.f,
                    "%.1f deg" );
                end_menu_child( "aim_q_tl" );
                begin_menu_child( "aim_q_bl", aim_q.cell.x, k_child_border );
                menu_panel_heading( "RAGEBOT" );
                ImGui::Checkbox( "Ragebot", &visuals::RageBot );
                if ( visuals::RageBot ) {
                    if ( !visuals::RageBotVisCheck && !visuals::RageBotPenCheck )
                        ImGui::TextWrapped( "Enable Vischeck and/or Pencheck for Ragebot to fire." );
                    ImGui::Checkbox( "Ragebot Vischeck", &visuals::RageBotVisCheck );
                    ImGui::Checkbox( "Ragebot Pencheck", &visuals::RageBotPenCheck );
                }
                end_menu_child( "aim_q_bl" );
                ImGui::EndGroup( );

                ImGui::SameLine( 0.f, k_quad_col_gutter );

                ImGui::BeginGroup( );
                begin_menu_child( "aim_q_tr", aim_q.cell.x, k_child_border );
                menu_panel_heading( "FOV CIRCLE" );
                menu_checkbox_rgba_row_right_pick(
                    "##fovcircle_pick",
                    "Silent aim FOV circle",
                    &visuals::SilentAimShowFovCircle,
                    &visuals::SilentAimCircleColor.x,
                    kMenuColorRgba );
                ImGui::Checkbox( "Fill FOV circle", &visuals::SilentAimCircleFilled );
                menu_slider_float_reveal(
                    SliderRevealTab::Aimbot,
                    slid_aim,
                    "FOV circle opacity",
                    &visuals::SilentAimCircleOpacity,
                    0.f,
                    1.f,
                    "%.2f" );
                end_menu_child( "aim_q_tr" );
                begin_menu_child( "aim_q_br", aim_q.cell.x, k_child_border );
                menu_panel_heading( "INFO" );
                end_menu_child( "aim_q_br" );
                ImGui::EndGroup( );

                ImGui::PopStyleVar( );
                ImGui::EndTabItem( );
            }
            else {
                slider_reveal_poll_frame( SliderRevealTab::Aimbot, false );
            }

            if ( ImGui::BeginTabItem( "WORLD MOD" ) ) {
                const TabSliderRevealFrame slid_world = slider_reveal_poll_frame( SliderRevealTab::World, true );
                const ImGuiStyle& wm_st = ImGui::GetStyle( );
                const float wm_gap_x = wm_st.ItemSpacing.x;
                const ImVec2 wm_avail_tab = ImGui::GetContentRegionAvail( );

                const float wm_col_w = fmaxf( 1.f, ( wm_avail_tab.x - wm_gap_x ) * 0.5f );
                const float wm_left_w = wm_col_w;
                const float wm_right_w = fmaxf( 1.f, wm_avail_tab.x - wm_left_w - wm_gap_x );

                ImGui::BeginGroup( );
                begin_menu_child( "wm_world_edit", wm_left_w, k_child_border );
                menu_panel_heading( "WORLD" );
                ImGui::Checkbox( "Patch world XMM vector##world_edit_hook", &world_edit::enabled );
                menu_slider_float_reveal(
                    SliderRevealTab::World, slid_world, "Clarity##world_edit", &world_edit::clarity, 0.f, 5.f, "%.2f" );
                menu_slider_float_reveal(
                    SliderRevealTab::World,
                    slid_world,
                    "Saturation##world_edit",
                    &world_edit::saturation,
                    0.f,
                    5.f,
                    "%.2f" );
                menu_slider_float_reveal(
                    SliderRevealTab::World,
                    slid_world,
                    "Brightness##world_edit",
                    &world_edit::brightness,
                    0.f,
                    5.f,
                    "%.2f" );
                menu_slider_float_reveal(
                    SliderRevealTab::World,
                    slid_world,
                    "Illumination##world_edit",
                    &world_edit::illumination,
                    0.f,
                    16.f,
                    "%.2f" );
                end_menu_child( "wm_world_edit" );

                begin_menu_child( "wm_player_glow", wm_left_w, k_child_border );
                menu_panel_heading( "PLAYER GLOW" );
                menu_checkbox_rgb_row_right_pick(
                    "##player_glow_pick",
                    "Enable##player_glow",
                    &world_modulation::player_glow_enabled,
                    &world_modulation::player_glow_rgb.x,
                    kMenuColorRgb );
                if ( world_modulation::player_glow_enabled ) {
                    const char* player_glow_type_labels[ ] = { "Solid", "Outline" };
                    ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, k_menu_combo_frame_pad );
                    ImGui::PushItemWidth( menu_combo_desired_width_px( ) );
                    ImGui::SetNextComboDropdownLayoutHeight(
                        menu_combo_list_reserve_height_px( IM_ARRAYSIZE( player_glow_type_labels ) ) );
                    ImGui::Combo(
                        "Glow type", &world_modulation::player_glow_type, player_glow_type_labels, IM_ARRAYSIZE( player_glow_type_labels ) );
                    ImGui::PopItemWidth( );
                    ImGui::PopStyleVar( );
                }
                end_menu_child( "wm_player_glow" );
                ImGui::EndGroup( );

                ImGui::SameLine( 0.f, wm_gap_x );
                begin_menu_child( "wm_right_channels", wm_right_w, k_child_border );
                menu_panel_heading( "LIGHTING" );
                ImGui::Checkbox( "World Modulation", &world_modulation::enabled );
                if ( world_modulation::enabled && IceBox::world_modulation_values_differ_from_frozen_snapshot( ) ) {
                    ImGui::SameLine( );
                    if ( ImGui::SmallButton( "Reset to game##wm_rst" ) )
                        IceBox::world_modulation_reset_all_to_frozen_snapshot( );
                }
                ImGui::Checkbox( "Better light", &visuals::BetterLight );

                ImGui::Spacing( );
                ImGui::Separator( );
                ImGui::Spacing( );

                if ( world_modulation::enabled ) {
                    if ( ImGui::BeginTable(
                        "wm_channels",
                        2,
                        ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoPadOuterX |
                        ImGuiTableFlags_BordersInnerV,
                        ImVec2( -FLT_MIN, 0.f ),
                        0.f ) ) {
                        ImGui::TableSetupColumn( "opt", ImGuiTableColumnFlags_WidthStretch, 1.f );
                        ImGui::TableSetupColumn( "clr", ImGuiTableColumnFlags_WidthFixed, k_menu_color_pick_w_rgb );

                        menu_world_light_row_rgb_pick(
                            &world_modulation::edit_light, "Light", &world_modulation::light_rgb.x, kMenuColorRgb, "wm_l" );
                        menu_world_light_row_rgb_pick( &world_modulation::edit_reflection,
                            "Reflection",
                            &world_modulation::reflection_rgb.x,
                            kMenuColorRgb,
                            "wm_r" );
                        menu_world_light_row_rgb_pick( &world_modulation::edit_highlight,
                            "Highlight",
                            &world_modulation::highlight_rgb.x,
                            kMenuColorRgb,
                            "wm_h" );
                        menu_world_light_row_rgb_pick( &world_modulation::edit_top_bottom,
                            "Top / bottom",
                            &world_modulation::top_bottom_rgb.x,
                            kMenuColorRgb,
                            "wm_tb" );
                        menu_world_light_row_rgb_pick( &world_modulation::edit_global_illum,
                            "Global illumination",
                            &world_modulation::global_illum_rgb.x,
                            kMenuColorRgb,
                            "wm_gi" );
                        menu_world_light_row_rgb_pick( &world_modulation::edit_magic,
                            "Magic (RRB / GGP / BYB)",
                            &world_modulation::magic_rgb.x,
                            kMenuColorRgb,
                            "wm_m" );
                        menu_world_light_row_rgb_pick( &world_modulation::edit_top_bottom_fog,
                            "Top / bottom fog",
                            &world_modulation::top_bottom_fog_rgb.x,
                            kMenuColorRgb,
                            "wm_tbf" );
                        menu_world_light_row_rgb_pick(
                            &world_modulation::edit_sky, "Sky color", &world_modulation::sky_rgb.x, kMenuColorRgb, "wm_sky" );
                        menu_world_light_row_rgb_pick( &world_modulation::edit_highlight2,
                            "Highlight 2",
                            &world_modulation::highlight2_rgb.x,
                            kMenuColorRgb,
                            "wm_h2" );

                        ImGui::EndTable( );
                    }
                }
                else {
                    ImGui::Spacing( );
                    ImGui::TextDisabled( "Enable World Modulation to edit lighting channels." );
                }
                end_menu_child( "wm_right_channels" );

                ImGui::EndTabItem( );
            }
            else {
                slider_reveal_poll_frame( SliderRevealTab::World, false );
            }

            if ( ImGui::BeginTabItem( "MISC" ) ) {
                const TabSliderRevealFrame slid_misc = slider_reveal_poll_frame( SliderRevealTab::Misc, true );
                ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( k_quad_col_gutter, k_quad_row_gutter ) );
                const MenuQuadDims misc_q = menu_quad_dims( );

                static bool s_third_person_listen = false;
                static double s_third_person_listen_after = 0.0;
                static bool s_third_person_wait_mouse_release = false;
                static bool s_jitter_peek_listen = false;
                static double s_jitter_peek_listen_after = 0.0;
                static bool s_jitter_peek_wait_mouse_release = false;
                static bool s_self_revive_listen = false;
                static double s_self_revive_listen_after = 0.0;
                static bool s_self_revive_wait_mouse_release = false;

                ImGui::BeginGroup( );
                begin_menu_child( "misc_q_tl", misc_q.cell.x, k_child_border );
                menu_panel_heading( "MOVEMENT" );
                ImGui::Checkbox( "Long melee", &visuals::LongMelee );
                if ( !visuals::ThirdPerson ) {
                    s_third_person_listen = false;
                    s_third_person_wait_mouse_release = false;
                }
                ImGui::PushID( "ThirdPersonHotkey" );
                {
                    char tp_vk_buf[ 64 ];
                    const char* tp_vk_text =
                        visuals::ThirdPerson ? MenuOthers::vk_display_label( visuals::ThirdPersonVk, tp_vk_buf ) : "";
                    const char* tp_btn_label = s_third_person_listen ? "" : tp_vk_text;
                    if ( ImGui::BeginTable(
                        "##tp_row", 2, k_menu_color_row_table_flags, ImVec2( -FLT_MIN, 0.f ), 0.f ) ) {
                        ImGui::TableSetupColumn( "l", ImGuiTableColumnFlags_WidthStretch );
                        ImGui::TableSetupColumn( "r", ImGuiTableColumnFlags_WidthFixed, k_menu_keybind_cell_w );
                        ImGui::TableNextRow( );
                        ImGui::TableNextColumn( );
                        ImGui::AlignTextToFramePadding( );
                        ImGui::Checkbox( "Third person", &visuals::ThirdPerson );
                        ImGui::TableNextColumn( );
                        ImGui::AlignTextToFramePadding( );
                        if ( visuals::ThirdPerson ) {
                            menu_keybind_place_button_right_in_cell( k_menu_hotkey_btn_w );
                            ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.07f, 0.07f, 0.08f, 1.00f ) );
                            ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.24f, 0.06f, 0.07f, 1.00f ) );
                            ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.40f, 0.08f, 0.10f, 1.00f ) );
                            ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.92f, 0.92f, 0.94f, 1.00f ) );
                            if ( ImGui::Button(
                                tp_btn_label, ImVec2( k_menu_hotkey_btn_w, k_menu_hotkey_btn_h ) ) ) {
                                s_third_person_listen = true;
                                s_third_person_listen_after = ImGui::GetTime( ) + 0.18;
                                s_third_person_wait_mouse_release = true;
                            }
                            ImGui::PopStyleColor( 4 );
                        }
                        ImGui::EndTable( );
                    }
                }
                ImGui::PopID( );
                if ( visuals::ThirdPerson && s_third_person_listen ) {
                    const double now_tp = ImGui::GetTime( );
                    if ( now_tp >= s_third_person_listen_after ) {
                        if ( ( GetAsyncKeyState( VK_ESCAPE ) & 0x8000 ) != 0 ) {
                            s_third_person_listen = false;
                            s_third_person_wait_mouse_release = false;
                        }
                        else {
                            if ( s_third_person_wait_mouse_release ) {
                                if ( !MenuOthers::any_mouse_button_down( ) )
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
                end_menu_child( "misc_q_tl" );

                begin_menu_child( "misc_q_bl", misc_q.cell.x, k_child_border );
                menu_panel_heading( "JITTER PEEK" );
                if ( !visuals::JitterPeek ) {
                    s_jitter_peek_listen = false;
                    s_jitter_peek_wait_mouse_release = false;
                }
                ImGui::PushID( "JitterPeekHotkey" );
                {
                    char jp_vk_buf[ 64 ];
                    const char* jp_vk_text =
                        visuals::JitterPeek ? MenuOthers::vk_display_label( visuals::JitterPeekVk, jp_vk_buf ) : "";
                    const char* jp_btn_label = s_jitter_peek_listen ? "" : jp_vk_text;
                    if ( ImGui::BeginTable(
                        "##jp_row", 2, k_menu_color_row_table_flags, ImVec2( -FLT_MIN, 0.f ), 0.f ) ) {
                        ImGui::TableSetupColumn( "l", ImGuiTableColumnFlags_WidthStretch );
                        ImGui::TableSetupColumn( "r", ImGuiTableColumnFlags_WidthFixed, k_menu_keybind_cell_w );
                        ImGui::TableNextRow( );
                        ImGui::TableNextColumn( );
                        ImGui::AlignTextToFramePadding( );
                        ImGui::Checkbox( "Jitter peek", &visuals::JitterPeek );
                        ImGui::TableNextColumn( );
                        ImGui::AlignTextToFramePadding( );
                        if ( visuals::JitterPeek ) {
                            menu_keybind_place_button_right_in_cell( k_menu_hotkey_btn_w );
                            ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.07f, 0.07f, 0.08f, 1.00f ) );
                            ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.24f, 0.06f, 0.07f, 1.00f ) );
                            ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.40f, 0.08f, 0.10f, 1.00f ) );
                            ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.92f, 0.92f, 0.94f, 1.00f ) );
                            if ( ImGui::Button(
                                jp_btn_label, ImVec2( k_menu_hotkey_btn_w, k_menu_hotkey_btn_h ) ) ) {
                                s_jitter_peek_listen = true;
                                s_jitter_peek_listen_after = ImGui::GetTime( ) + 0.18;
                                s_jitter_peek_wait_mouse_release = true;
                            }
                            ImGui::PopStyleColor( 4 );
                        }
                        ImGui::EndTable( );
                    }
                }
                ImGui::PopID( );
                if ( visuals::JitterPeek && s_jitter_peek_listen ) {
                    const double now = ImGui::GetTime( );
                    if ( now >= s_jitter_peek_listen_after ) {
                        if ( ( GetAsyncKeyState( VK_ESCAPE ) & 0x8000 ) != 0 ) {
                            s_jitter_peek_listen = false;
                            s_jitter_peek_wait_mouse_release = false;
                        }
                        else {
                            if ( s_jitter_peek_wait_mouse_release ) {
                                if ( !MenuOthers::any_mouse_button_down( ) )
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
                if ( visuals::JitterPeek ) {
                    menu_slider_int_reveal(
                        SliderRevealTab::Misc,
                        slid_misc,
                        "Delay (ms)",
                        &visuals::JitterPeekDelayMs,
                        10,
                        150,
                        "%d" );
                }
                end_menu_child( "misc_q_bl" );

                ImGui::EndGroup( );

                ImGui::SameLine( 0.f, k_quad_col_gutter );

                ImGui::BeginGroup( );
                begin_menu_child( "misc_q_tr", misc_q.cell.x, k_child_border );
                menu_panel_heading( "COMBAT" );
                ImGui::Checkbox( "Raycast debug (console)", &visuals::RaycastClosestDebug );
                ImGui::Checkbox( "Run and shoot", &visuals::RunAndShoot );
                if ( !visuals::SelfRevive ) {
                    s_self_revive_listen = false;
                    s_self_revive_wait_mouse_release = false;
                }
                ImGui::PushID( "SelfReviveHotkey" );
                {
                    char sr_vk_buf[ 64 ];
                    const char* sr_vk_text =
                        visuals::SelfRevive ? MenuOthers::vk_display_label( visuals::SelfReviveVk, sr_vk_buf ) : "";
                    const char* sr_btn_label = s_self_revive_listen ? "" : sr_vk_text;
                    if ( ImGui::BeginTable(
                        "##sr_row", 2, k_menu_color_row_table_flags, ImVec2( -FLT_MIN, 0.f ), 0.f ) ) {
                        ImGui::TableSetupColumn( "l", ImGuiTableColumnFlags_WidthStretch );
                        ImGui::TableSetupColumn( "r", ImGuiTableColumnFlags_WidthFixed, k_menu_keybind_cell_w );
                        ImGui::TableNextRow( );
                        ImGui::TableNextColumn( );
                        ImGui::AlignTextToFramePadding( );
                        ImGui::Checkbox( "Self revive", &visuals::SelfRevive );
                        ImGui::TableNextColumn( );
                        ImGui::AlignTextToFramePadding( );
                        if ( visuals::SelfRevive ) {
                            menu_keybind_place_button_right_in_cell( k_menu_hotkey_btn_w );
                            ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.07f, 0.07f, 0.08f, 1.00f ) );
                            ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.24f, 0.06f, 0.07f, 1.00f ) );
                            ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.40f, 0.08f, 0.10f, 1.00f ) );
                            ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.92f, 0.92f, 0.94f, 1.00f ) );
                            if ( ImGui::Button(
                                sr_btn_label, ImVec2( k_menu_hotkey_btn_w, k_menu_hotkey_btn_h ) ) ) {
                                s_self_revive_listen = true;
                                s_self_revive_listen_after = ImGui::GetTime( ) + 0.18;
                                s_self_revive_wait_mouse_release = true;
                            }
                            ImGui::PopStyleColor( 4 );
                        }
                        ImGui::EndTable( );
                    }
                }
                ImGui::PopID( );
                if ( visuals::SelfRevive && s_self_revive_listen ) {
                    const double now_sr = ImGui::GetTime( );
                    if ( now_sr >= s_self_revive_listen_after ) {
                        if ( ( GetAsyncKeyState( VK_ESCAPE ) & 0x8000 ) != 0 ) {
                            s_self_revive_listen = false;
                            s_self_revive_wait_mouse_release = false;
                        }
                        else {
                            if ( s_self_revive_wait_mouse_release ) {
                                if ( !MenuOthers::any_mouse_button_down( ) )
                                    s_self_revive_wait_mouse_release = false;
                            }
                            if ( !s_self_revive_wait_mouse_release ) {
                                for ( int vk = 1; vk < 256; ++vk ) {
                                    if ( ( GetAsyncKeyState( vk ) & 0x8000 ) != 0 ) {
                                        visuals::SelfReviveVk = vk;
                                        s_self_revive_listen = false;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                ImGui::Checkbox( "Unlock all", &visuals::UnlockAllMidHook );
                ImGui::Checkbox( "No spread", &visuals::NoSpread );
                ImGui::Checkbox( "No recoil", &visuals::NoRecoil );
                end_menu_child( "misc_q_tr" );

                begin_menu_child( "misc_q_br", misc_q.cell.x, k_child_border );
                menu_panel_heading( "DUST" );
                menu_slider_float_reveal(
                    SliderRevealTab::Misc,
                    slid_misc,
                    "Dust radius",
                    &visuals::DustSpawnRadius,
                    0.25f,
                    15.f,
                    "%.2f" );
                menu_text_rgba_row_right_pick(
                    "##dust_tint_pick", "Dust color", &visuals::DustSpawnColor.x, kMenuColorRgba );
                if ( ImGui::Button( "Spawn dust (local)", ImVec2( -FLT_MIN, 0.f ) ) )
                    MenuOthers::spawn_dust_at_local_origin( );
                end_menu_child( "misc_q_br" );

                ImGui::EndGroup( );

                ImGui::PopStyleVar( );
                ImGui::EndTabItem( );
            }
            else {
                slider_reveal_poll_frame( SliderRevealTab::Misc, false );
            }

            if ( ImGui::BeginTabItem( "SETTINGS" ) ) {
                if ( ImGui::Button( "Dump EDESrvComponents (console)", ImVec2( -FLT_MIN, 0.f ) ) )
                    MenuOthers::dump_local_ed_srv_components_to_console( );

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
