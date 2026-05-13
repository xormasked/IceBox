#include "../interface.hpp"

#include "../../../../config.hpp"
#include "../../../../../Core/Engine/Anvil/scimitar.hpp"
#include "../../../../../Core/Utils/memory.hpp"
#include "../../d3d11hook.hpp"

#include <Windows.h>

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <string>
#include <unordered_set>

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

        constexpr float k_menu_color_pick_w_rgb = 142.f;
        constexpr float k_menu_color_pick_w_rgba = 168.f;

        constexpr ImGuiTableFlags k_menu_color_row_table_flags =
            ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoSavedSettings;

        /// NoInputs ColorEdit draws the swatch at the left of `CalcItemWidth()` (imgui `ColorEdit4`: `button_offset_x == 0`); offset cursor by `avail - w_button` so it sits on the cell's right edge.
        float menu_color_edit_no_inputs_swatch_span( ImGuiColorEditFlags flags ) noexcept
        {
            const ImGuiStyle& st = ImGui::GetStyle( );
            const float square_sz = ImGui::GetFrameHeight( );
            if ( flags & ImGuiColorEditFlags_NoSmallPreview )
                return square_sz;
            return square_sz + st.ItemInnerSpacing.x;
        }

        void menu_color_edit_begin_right_in_table_cell( ImGuiColorEditFlags flags ) noexcept
        {
            const float w_pick = menu_color_edit_no_inputs_swatch_span( flags );
            const float slack = fmaxf( 0.f, ImGui::GetContentRegionAvail( ).x - w_pick );
            if ( slack > 0.f )
                ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + slack );
            ImGui::PushItemWidth( w_pick );
        }

        constexpr float k_menu_hotkey_btn_w = 76.f;
        constexpr float k_menu_hotkey_btn_h = 22.f;
        constexpr float k_menu_keybind_cell_w = k_menu_hotkey_btn_w + 12.f;

        void menu_keybind_place_button_right_in_cell( float button_w ) noexcept
        {
            const float slack = fmaxf( 0.f, ImGui::GetContentRegionAvail( ).x - button_w );
            if ( slack > 0.f )
                ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + slack );
        }

        /// Thinner footprint + a touch more height than sliders (child uses compact `FramePadding.y == 2`).
        const ImVec2 k_menu_combo_frame_pad( 5.f, 6.f );
        constexpr float k_menu_combo_width_frac = 0.52f;
        constexpr float k_menu_combo_min_width = 100.f;

        inline float menu_combo_desired_width_px( ) noexcept
        {
            return fmaxf( k_menu_combo_min_width, ImGui::GetContentRegionAvail( ).x * k_menu_combo_width_frac );
        }

        /// Matches ImGui `CalcMaxPopupHeightFromItemCount` — reserved below combo so siblings shift like inline expand.
        inline float menu_combo_list_reserve_height_px( int item_count ) noexcept
        {
            if ( item_count <= 0 )
                return 0.f;
            const ImGuiStyle& st = ImGui::GetStyle( );
            return ( ImGui::GetFontSize( ) + st.ItemSpacing.y ) * float( item_count ) - st.ItemSpacing.y + st.WindowPadding.y * 2.f;
        }

        void menu_checkbox_rgb_row_right_pick( const char* row_scope_id, const char* checkbox_label, bool* checked,
                                              float* rgb, ImGuiColorEditFlags flags ) noexcept
        {
            ImGui::PushID( row_scope_id );
            if ( ImGui::BeginTable(
                     "##rgb_row", 2, k_menu_color_row_table_flags, ImVec2( -FLT_MIN, 0.f ), 0.f ) ) {
                ImGui::TableSetupColumn( "l", ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "r", ImGuiTableColumnFlags_WidthFixed, k_menu_color_pick_w_rgb );
                ImGui::TableNextRow( );
                ImGui::TableNextColumn( );
                ImGui::AlignTextToFramePadding( );
                ImGui::Checkbox( checkbox_label, checked );
                ImGui::TableNextColumn( );
                ImGui::AlignTextToFramePadding( );
                menu_color_edit_begin_right_in_table_cell( flags );
                ImGui::ColorEdit3( "##pick_rgb", rgb, flags );
                ImGui::PopItemWidth( );
                ImGui::EndTable( );
            }
            ImGui::PopID( );
        }

        void menu_checkbox_rgba_row_right_pick(
            const char* row_scope_id, const char* checkbox_label, bool* checked, float* rgba, ImGuiColorEditFlags flags ) noexcept
        {
            ImGui::PushID( row_scope_id );
            if ( ImGui::BeginTable(
                     "##rgba_row", 2, k_menu_color_row_table_flags, ImVec2( -FLT_MIN, 0.f ), 0.f ) ) {
                ImGui::TableSetupColumn( "l", ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "r", ImGuiTableColumnFlags_WidthFixed, k_menu_color_pick_w_rgba );
                ImGui::TableNextRow( );
                ImGui::TableNextColumn( );
                ImGui::AlignTextToFramePadding( );
                ImGui::Checkbox( checkbox_label, checked );
                ImGui::TableNextColumn( );
                ImGui::AlignTextToFramePadding( );
                menu_color_edit_begin_right_in_table_cell( flags );
                ImGui::ColorEdit4( "##pick_rgba", rgba, flags );
                ImGui::PopItemWidth( );
                ImGui::EndTable( );
            }
            ImGui::PopID( );
        }

        void menu_text_rgb_row_right_pick( const char* row_scope_id, const char* left_caption_utf8, float* rgb,
                                           ImGuiColorEditFlags flags ) noexcept
        {
            ImGui::PushID( row_scope_id );
            if ( ImGui::BeginTable(
                     "##rgb_lbl_only_row", 2, k_menu_color_row_table_flags, ImVec2( -FLT_MIN, 0.f ), 0.f ) ) {
                ImGui::TableSetupColumn( "l", ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "r", ImGuiTableColumnFlags_WidthFixed, k_menu_color_pick_w_rgb );
                ImGui::TableNextRow( );
                ImGui::TableNextColumn( );
                ImGui::AlignTextToFramePadding( );
                ImGui::TextUnformatted( left_caption_utf8 );
                ImGui::TableNextColumn( );
                ImGui::AlignTextToFramePadding( );
                menu_color_edit_begin_right_in_table_cell( flags );
                ImGui::ColorEdit3( "##pick_rgb_lbl_only", rgb, flags );
                ImGui::PopItemWidth( );
                ImGui::EndTable( );
            }
            ImGui::PopID( );
        }

        void menu_text_rgba_row_right_pick(
            const char* row_scope_id, const char* left_caption_utf8, float* rgba, ImGuiColorEditFlags flags ) noexcept
        {
            ImGui::PushID( row_scope_id );
            if ( ImGui::BeginTable(
                     "##rgba_lbl_row", 2, k_menu_color_row_table_flags, ImVec2( -FLT_MIN, 0.f ), 0.f ) ) {
                ImGui::TableSetupColumn( "l", ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "r", ImGuiTableColumnFlags_WidthFixed, k_menu_color_pick_w_rgba );
                ImGui::TableNextRow( );
                ImGui::TableNextColumn( );
                ImGui::AlignTextToFramePadding( );
                ImGui::TextUnformatted( left_caption_utf8 );
                ImGui::TableNextColumn( );
                ImGui::AlignTextToFramePadding( );
                menu_color_edit_begin_right_in_table_cell( flags );
                ImGui::ColorEdit4( "##pick_rgba_lbl", rgba, flags );
                ImGui::PopItemWidth( );
                ImGui::EndTable( );
            }
            ImGui::PopID( );
        }

        void menu_world_light_row_rgb_pick( bool* enabled, const char* channel_label, float* rgb, ImGuiColorEditFlags flags,
                                            const char* row_id ) noexcept
        {
            ImGui::TableNextRow( );
            ImGui::PushID( row_id );
            ImGui::TableNextColumn( );
            ImGui::AlignTextToFramePadding( );
            ImGui::Checkbox( "##cb", enabled );
            ImGui::SameLine( 0.f, ImGui::GetStyle( ).ItemInnerSpacing.x );
            ImGui::AlignTextToFramePadding( );
            ImGui::TextUnformatted( channel_label );

            ImGui::TableNextColumn( );
            ImGui::AlignTextToFramePadding( );
            if ( *enabled ) {
                menu_color_edit_begin_right_in_table_cell( flags );
                ImGui::ColorEdit3( "##pick", rgb, flags );
                ImGui::PopItemWidth( );
            } else {
                const float w_pick = menu_color_edit_no_inputs_swatch_span( flags );
                const float slack = fmaxf( 0.f, ImGui::GetContentRegionAvail( ).x - w_pick );
                if ( slack > 0.f )
                    ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + slack );
                ImGui::Dummy( ImVec2( w_pick, ImGui::GetFrameHeight( ) ) );
            }
            ImGui::PopID( );
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

        /// Full-width saturated red accent — visually matches minimalist “solid bar under tabs” mocks.
        void menu_draw_tab_strip_accent_band( float thickness_px ) noexcept
        {
            ImGuiWindow* win = ImGui::GetCurrentWindow( );
            if ( !win || win->SkipItems )
                return;
            ImDrawList* dl = win->DrawList;
            const float gap_y = ImGui::GetStyle( ).ItemSpacing.y;
            const ImVec2 c = ImGui::GetCursorScreenPos( );
            const float y0 = c.y - gap_y;
            dl->AddRectFilled( ImVec2( win->InnerRect.Min.x, y0 ), ImVec2( win->InnerRect.Max.x, y0 + thickness_px ),
                               IM_COL32( 237, 20, 32, 255 ) );
        }

        /// Max gradient width for the inline ESP skeleton / tracers separator only (panel titles use full width).
        constexpr float k_menu_esp_section_divider_fade_width_px = 88.f;

        /// Red gradient band matching the line under `menu_panel_heading` titles (no title text).
        /// Pass `max_gradient_width_px > 0` to cap length; `0` spans full `ContentRegionAvail` (default for headings).
        void menu_panel_heading_divider_strip( float max_gradient_width_px = 0.f ) noexcept
        {
            const ImVec2 a = ImGui::GetCursorScreenPos( );
            const float avail = fmaxf( 32.f, ImGui::GetContentRegionAvail( ).x );
            const float w_grad =
                max_gradient_width_px > 0.f ? fminf( avail, max_gradient_width_px ) : avail;
            ImDrawList* dl = ImGui::GetWindowDrawList( );
            const float band_h = 2.f;
            const ImVec2 b( a.x + w_grad, a.y + band_h );
            const ImU32 l = IM_COL32( 255, 38, 48, 255 );
            const ImU32 r = IM_COL32( 255, 38, 48, 0 );
            dl->AddRectFilledMultiColor( a, b, l, r, r, l );
            ImGui::Dummy( ImVec2( 1.f, band_h + ImGui::GetStyle( ).ItemSpacing.y * 0.65f ) );
        }

        /// ALL CAPS label + fading red divider (fade to transparent on the right), BottomFrag-ish section chrome.
        void menu_panel_heading( const char* title_all_caps ) noexcept
        {
            ImGui::TextUnformatted( title_all_caps );
            menu_panel_heading_divider_strip( );
        }

        void apply_menu_theme_once( ) noexcept
        {
            static bool s_applied = false;
            if ( s_applied )
                return;
            s_applied = true;

            ImGui::StyleColorsDark( );

            ImGuiStyle& st = ImGui::GetStyle( );
            /// Flat-ish industrial overlay (minimal rounding, vivid red accents).
            st.WindowRounding = 2.f;
            st.ChildRounding = 0.f;
            st.FrameRounding = 2.f;
            st.PopupRounding = 2.f;
            st.ScrollbarRounding = 2.f;
            st.GrabRounding = 2.f;
            st.TabRounding = 0.f;
            st.ChildBorderSize = 1.f;
            st.ScrollbarSize = 10.f;
            st.WindowPadding = ImVec2( 10.f, 10.f );
            st.FramePadding = ImVec2( 8.f, 4.f );
            st.ItemSpacing = ImVec2( 8.f, 6.f );

            ImVec4* c = st.Colors;
            const ImVec4 black( 0.f, 0.f, 0.f, 1.f );
            const ImVec4 faint_edge( 0.48f, 0.48f, 0.52f, 0.40f );
            const ImVec4 txt( 0.94f, 0.94f, 0.96f, 1.f );
            const ImVec4 txt_dim( 0.43f, 0.43f, 0.46f, 1.f );

            const ImVec4 red_tab( 0.92f, 0.05f, 0.10f, 1.f );         // Selected tab slab
            const ImVec4 red_hi( 0.95f, 0.20f, 0.26f, 1.f );          // Hovers / check / slider grabs
            const ImVec4 red_active( 1.f, 0.38f, 0.42f, 1.f );        // Pulses / active sliders
            const ImVec4 red_muted_btn( 0.14f, 0.06f, 0.06f, 1.f );   // Off-tab hover bleed

            c[ ImGuiCol_Text ] = txt;
            c[ ImGuiCol_TextDisabled ] = txt_dim;
            c[ ImGuiCol_WindowBg ] = black;
            c[ ImGuiCol_ChildBg ] = black;
            c[ ImGuiCol_PopupBg ] = ImVec4( 0.02f, 0.02f, 0.02f, 0.96f );
            c[ ImGuiCol_Border ] = faint_edge;
            c[ ImGuiCol_BorderShadow ] = ImVec4( 0.f, 0.f, 0.f, 0.f );

            const ImVec4 frame_bg( 0.06f, 0.06f, 0.06f, 1.f );
            const ImVec4 frame_bg_hov( 0.16f, 0.06f, 0.06f, 1.f );
            const ImVec4 frame_bg_act( 0.24f, 0.07f, 0.07f, 1.f );

            c[ ImGuiCol_FrameBg ] = frame_bg;
            c[ ImGuiCol_FrameBgHovered ] = frame_bg_hov;
            c[ ImGuiCol_FrameBgActive ] = frame_bg_act;

            c[ ImGuiCol_TitleBg ] = black;
            c[ ImGuiCol_TitleBgActive ] = black;
            c[ ImGuiCol_TitleBgCollapsed ] = black;

            c[ ImGuiCol_MenuBarBg ] = black;
            c[ ImGuiCol_ScrollbarBg ] = black;
            c[ ImGuiCol_ScrollbarGrab ] = ImVec4( 0.22f, 0.22f, 0.24f, 1.f );
            c[ ImGuiCol_ScrollbarGrabHovered ] = red_hi;
            c[ ImGuiCol_ScrollbarGrabActive ] = red_active;

            c[ ImGuiCol_CheckMark ] = red_hi;
            c[ ImGuiCol_SliderGrab ] = red_hi;
            c[ ImGuiCol_SliderGrabActive ] = red_active;

            c[ ImGuiCol_Button ] = ImVec4( 0.12f, 0.12f, 0.14f, 1.f );
            c[ ImGuiCol_ButtonHovered ] = red_hi;
            c[ ImGuiCol_ButtonActive ] = red_active;

            c[ ImGuiCol_Header ] = ImVec4( 0.32f, 0.06f, 0.06f, 0.55f );
            c[ ImGuiCol_HeaderHovered ] = frame_bg_hov;
            c[ ImGuiCol_HeaderActive ] = red_tab;

            c[ ImGuiCol_Separator ] = ImVec4( 0.65f, 0.06f, 0.07f, 0.82f );
            c[ ImGuiCol_SeparatorHovered ] = red_hi;
            c[ ImGuiCol_SeparatorActive ] = red_active;

            c[ ImGuiCol_ResizeGrip ] = ImVec4( 0.5f, 0.08f, 0.09f, 0.38f );
            c[ ImGuiCol_ResizeGripHovered ] = ImVec4( 0.9f, 0.22f, 0.26f, 0.72f );
            c[ ImGuiCol_ResizeGripActive ] = red_active;

            c[ ImGuiCol_Tab ] = black;
            c[ ImGuiCol_TabHovered ] = red_muted_btn;
            c[ ImGuiCol_TabActive ] = red_tab;
            c[ ImGuiCol_TabUnfocused ] = ImVec4( 0.f, 0.f, 0.f, 0.92f );
            c[ ImGuiCol_TabUnfocusedActive ] = ImVec4( 0.55f, 0.10f, 0.13f, 1.f );

            c[ ImGuiCol_PlotLines ] = red_hi;
            c[ ImGuiCol_PlotLinesHovered ] = red_active;
            c[ ImGuiCol_PlotHistogram ] = red_hi;
            c[ ImGuiCol_PlotHistogramHovered ] = red_active;

            c[ ImGuiCol_TableHeaderBg ] = black;
            c[ ImGuiCol_TableBorderStrong ] = ImVec4( 0.35f, 0.07f, 0.08f, 0.92f );
            c[ ImGuiCol_TableBorderLight ] = faint_edge;
            c[ ImGuiCol_TableRowBg ] = ImVec4( 0.f, 0.f, 0.f, 0.f );
            c[ ImGuiCol_TableRowBgAlt ] = ImVec4( 1.f, 1.f, 1.f, 0.02f );

            c[ ImGuiCol_TextSelectedBg ] = ImVec4( 0.55f, 0.08f, 0.09f, 0.72f );
            c[ ImGuiCol_DragDropTarget ] = ImVec4( 0.94f, 0.18f, 0.21f, 0.94f );
            c[ ImGuiCol_NavHighlight ] = ImVec4( 0.9f, 0.26f, 0.31f, 0.92f );
            c[ ImGuiCol_NavWindowingHighlight ] = ImVec4( 1.f, 0.32f, 0.36f, 0.92f );
            c[ ImGuiCol_NavWindowingDimBg ] = ImVec4( 0.f, 0.f, 0.f, 0.58f );
            c[ ImGuiCol_ModalWindowDimBg ] = ImVec4( 0.f, 0.f, 0.f, 0.64f );
        }

        constexpr bool k_child_border = true;

        /// Bottom gap under quad / world-mod panels so they do not sit flush on the tab bar edge.
        constexpr float k_tab_content_bottom_gap = 10.f;

        /// Horizontal gap between left/right quad columns — equals `PushStyleVar ItemSpacing.x` (SameLine spacing).
        constexpr float k_quad_col_gutter = 5.f;
        /// Vertical gap between stacked children in the same column (top panel → bottom panel).
        constexpr float k_quad_row_gutter = 6.f;

        /// Compact child panels: cached height hugs content; no inner scrollbar (avoids size/scroll feedback jitter).
        static auto menu_compact_child_storage_key( const char* panel_id ) noexcept -> ImGuiID
        {
            char buf[ 112 ];
            _snprintf_s( buf, sizeof( buf ), _TRUNCATE, "IceBox_cmp##%s", panel_id );
            return ImHashStr( buf, 0 );
        }
        /// Measured outer height from last `end_menu_child` (target for animation).
        static auto menu_compact_child_target_storage_key( const char* panel_id ) noexcept -> ImGuiID
        {
            char buf[ 112 ];
            _snprintf_s( buf, sizeof( buf ), _TRUNCATE, "IceBox_tgt##%s", panel_id );
            return ImHashStr( buf, 0 );
        }
        /// Same idea as `third_person.cpp` `esmooth` — fast initial move, eases as it approaches target.
        inline float menu_child_height_esmooth( float cur, float tgt, float dt, float tau ) noexcept
        {
            const float a = ( tau > 1e-6f && dt > 0.f ) ? 1.f - expf( -dt / tau ) : 1.f;
            return cur + ( tgt - cur ) * a;
        }
        constexpr float k_compact_child_height_tau = 0.13f;
        constexpr float k_compact_child_height_snap_px = 0.75f;
        constexpr float k_compact_child_max_dt = 0.05f;
        constexpr float k_compact_child_min_outer_h = 24.f;
        /// First-frame height only; must NOT come from GetContentRegionAvail (feedback when window moves / layout reflows).
        constexpr float k_compact_child_cold_outer_h = 220.f;
        constexpr float k_compact_child_slack_below = 5.f;
        constexpr ImGuiWindowFlags k_compact_child_flags =
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

        struct MenuQuadDims {
            ImVec2 cell{};
        };

        auto menu_quad_dims( ) noexcept -> MenuQuadDims
        {
            const ImVec2 avail = ImGui::GetContentRegionAvail( );
            MenuQuadDims d{};
            d.cell.x = ( avail.x - k_quad_col_gutter ) * 0.5f;
            d.cell.y = 0.f; // Unused; compact child heights are measured in end_menu_child, not tied to avail.
            return d;
        }

        void begin_menu_child( const char* id, float width_px, bool border ) noexcept
        {
            ImGuiWindow* parent = ImGui::GetCurrentWindow( );
            const ImGuiID hk_sm = menu_compact_child_storage_key( id );
            const ImGuiID hk_tgt = menu_compact_child_target_storage_key( id );
            const float tgt = parent->StateStorage.GetFloat( hk_tgt, 0.f );
            float sm = parent->StateStorage.GetFloat( hk_sm, 0.f );

            float use_h = k_compact_child_cold_outer_h;
            if ( tgt >= k_compact_child_min_outer_h ) {
                const float dt = fminf( fmaxf( ImGui::GetIO( ).DeltaTime, 0.f ), k_compact_child_max_dt );
                if ( sm < k_compact_child_min_outer_h - 1e-3f ) {
                    /// First time we have a measurement for this child — match immediately (no animate from cold).
                    sm = tgt;
                } else if ( fabsf( tgt - sm ) <= k_compact_child_height_snap_px ) {
                    sm = tgt;
                } else {
                    sm = menu_child_height_esmooth( sm, tgt, dt, k_compact_child_height_tau );
                    if ( fabsf( sm - tgt ) <= k_compact_child_height_snap_px )
                        sm = tgt;
                }
                sm = fmaxf( k_compact_child_min_outer_h, sm );
                parent->StateStorage.SetFloat( hk_sm, sm );
                use_h = ceilf( sm );
            }

            ImGui::PushStyleVar( ImGuiStyleVar_ChildBorderSize, 1.f );
            ImGui::PushStyleColor( ImGuiCol_ChildBg, ImVec4( 0.f, 0.f, 0.f, 1.f ) );
            ImGui::PushStyleColor( ImGuiCol_Border, ImVec4( 0.50f, 0.50f, 0.54f, 0.42f ) );
            ImGui::BeginChild( id, ImVec2( width_px, use_h ), border, k_compact_child_flags );
            const ImGuiStyle& pst = ImGui::GetStyle( );
            ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( pst.FramePadding.x, 2.f ) );
            const float w = ImGui::GetContentRegionAvail( ).x;
            // Keep sliders/combos visually narrow and left-aligned (no full-bleed grab).
            const float item_w = fmaxf( 120.f, fminf( 230.f, w * 0.48f ) );
            ImGui::PushItemWidth( item_w );
        }

        void end_menu_child( const char* id ) noexcept
        {
            ImGuiWindow* cw = ImGui::GetCurrentWindow( );
            ImGuiWindow* parent = cw ? cw->ParentWindow : nullptr;
            if ( parent && cw && ( cw->Flags & ImGuiWindowFlags_ChildWindow ) )
            {
                const ImGuiStyle& ist = ImGui::GetStyle( );
                /// Same basis as CalcWindowScrollValues (CursorMaxPos - CursorStartPos), plus cursor for trailing empty line.
                const float extent_y = cw->DC.CursorMaxPos.y - cw->DC.CursorStartPos.y;
                const float inner_bottom = ceilf( fmaxf( extent_y, ImGui::GetCursorPosY( ) ) );
                float outer_h = inner_bottom + ist.WindowPadding.y * 2.f + k_compact_child_slack_below;
                outer_h = fmaxf( k_compact_child_min_outer_h, ceilf( outer_h ) );
                parent->StateStorage.SetFloat( menu_compact_child_target_storage_key( id ), outer_h );
            }

            ImGui::PopItemWidth( );
            ImGui::PopStyleVar( );
            ImGui::EndChild( );
            ImGui::PopStyleColor( 2 );
            ImGui::PopStyleVar( );
        }

        enum class SliderRevealTab : int { Visuals = 0, Aimbot = 1, World = 2, Misc = 3, Count = 4 };

        constexpr int k_slider_reveal_tab_count = static_cast<int>( SliderRevealTab::Count );

        constexpr float k_slider_reveal_duration_sec = 0.34f;

        inline float slider_reveal_eased_t( float u_linear_clamped01 ) noexcept
        {
            u_linear_clamped01 = fminf( fmaxf( u_linear_clamped01, 0.f ), 1.f );
            return 1.f - powf( 1.f - u_linear_clamped01, 3.f ); // ease-out cubic
        }

        inline std::uintptr_t slider_reveal_ptr_fold( const void* p ) noexcept
        {
            const auto addr = reinterpret_cast<std::uintptr_t>( p );
            return addr ^ static_cast<std::uintptr_t>( addr >> 32 );
        }

        inline std::uint64_t slider_reveal_engaged_storage_key( SliderRevealTab tab, const void* value_address ) noexcept
        {
            constexpr int k_shift = 56;
            const auto tab_bits =
                static_cast<std::uint64_t>( static_cast<int>( tab ) & 0xff ) << k_shift;
            return tab_bits | ( static_cast<std::uint64_t>( slider_reveal_ptr_fold( value_address ) )
                                & ~( ( std::uint64_t{ 0xff } ) << k_shift ) );
        }

        static float g_slider_reveal_start_sec[ k_slider_reveal_tab_count ] = { -1.f, -1.f, -1.f, -1.f };
        static bool g_slider_reveal_prev_selected[ k_slider_reveal_tab_count ] = {};
        static std::unordered_set<std::uint64_t> g_slider_reveal_engaged_keys;

        inline void slider_reveal_clear_engaged_keys_for_tab( SliderRevealTab tab )
        {
            constexpr int k_shift = 56;
            const std::uint64_t tab_sel =
                static_cast<std::uint64_t>( static_cast<int>( tab ) & 0xff ) << k_shift;
            const std::uint64_t selector = tab_sel >> k_shift;
            for ( auto it = g_slider_reveal_engaged_keys.begin( ); it != g_slider_reveal_engaged_keys.end( ); ) {
                if ( ( *it >> k_shift ) == selector )
                    it = g_slider_reveal_engaged_keys.erase( it );
                else
                    ++it;
            }
        }

        struct TabSliderRevealFrame {
            float eased_t { 1.f }; /// eased 0..1 (revealed knob position)
        };

        inline TabSliderRevealFrame slider_reveal_poll_frame( SliderRevealTab tab, bool tab_body_visible_this_frame )
        {
            TabSliderRevealFrame out;
            const int ti = static_cast<int>( tab );

            if ( tab_body_visible_this_frame ) {
                if ( !g_slider_reveal_prev_selected[ ti ] ) {
                    slider_reveal_clear_engaged_keys_for_tab( tab );
                    g_slider_reveal_start_sec[ ti ] = ImGui::GetTime( );
                }
                g_slider_reveal_prev_selected[ ti ] = true;

                const float started = g_slider_reveal_start_sec[ ti ];
                if ( started < 0.f ) {
                    out.eased_t = 1.f;
                    return out;
                }

                const float lin = ( ImGui::GetTime( ) - started ) / k_slider_reveal_duration_sec;
                out.eased_t = slider_reveal_eased_t( fminf( lin, 1.f ) );
            } else {
                g_slider_reveal_prev_selected[ ti ] = false;
                out.eased_t = 1.f;
            }
            return out;
        }

        inline bool menu_slider_float_reveal( SliderRevealTab tab, const TabSliderRevealFrame& fr, const char* label,
                                             float* v, float v_min, float v_max, const char* format ) noexcept
        {
            const std::uint64_t ky = slider_reveal_engaged_storage_key( tab, v );
            constexpr float k_reveal_eps = 0.997f;

            bool sync_live = fr.eased_t >= k_reveal_eps || g_slider_reveal_engaged_keys.count( ky ) > 0;
            float scratch = sync_live ? *v : v_min + ( *v - v_min ) * fr.eased_t;

            const bool changed = ImGui::SliderFloat( label, &scratch, v_min, v_max, format );

            if ( fr.eased_t < k_reveal_eps
                 && ( ImGui::IsItemActive( ) || ImGui::IsItemActivated( ) || ImGui::IsItemEdited( ) ) )
                g_slider_reveal_engaged_keys.insert( ky );

            sync_live = fr.eased_t >= k_reveal_eps || g_slider_reveal_engaged_keys.count( ky ) > 0;
            if ( sync_live )
                *v = scratch;
            return changed;
        }

        inline bool menu_slider_int_reveal( SliderRevealTab tab, const TabSliderRevealFrame& fr, const char* label,
                                           int* v, int v_min, int v_max, const char* format )
        {
            const std::uint64_t ky = slider_reveal_engaged_storage_key( tab, v );
            constexpr float k_reveal_eps = 0.997f;

            bool sync_live = fr.eased_t >= k_reveal_eps || g_slider_reveal_engaged_keys.count( ky ) > 0;
            int scratch = sync_live ?
                *v :
                static_cast<int>( floorf(
                    static_cast<float>( v_min ) + static_cast<float>( *v - v_min ) * fr.eased_t + 0.5f ) );

            scratch = scratch < v_min ? v_min : ( scratch > v_max ? v_max : scratch );

            const bool changed = ImGui::SliderInt( label, &scratch, v_min, v_max, format );

            if ( fr.eased_t < k_reveal_eps
                 && ( ImGui::IsItemActive( ) || ImGui::IsItemActivated( ) || ImGui::IsItemEdited( ) ) )
                g_slider_reveal_engaged_keys.insert( ky );

            sync_live = fr.eased_t >= k_reveal_eps || g_slider_reveal_engaged_keys.count( ky ) > 0;
            if ( sync_live )
                *v = scratch;
            return changed;
        }

        inline void slider_reveal_mark_menu_hidden( ) noexcept
        {
            for ( int i = 0; i < k_slider_reveal_tab_count; ++i )
                g_slider_reveal_prev_selected[ i ] = false;
        }

    } // namespace

    auto user_interface( ) -> void
    {
        if ( !menu_open ) {
            slider_reveal_mark_menu_hidden( );
            return;
        }

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
                    ImGui::Checkbox( "Skeleton vischeck", &visuals::SkeletonVisCheck );
                    if ( visuals::SkeletonVisCheck ) {
                        menu_text_rgb_row_right_pick(
                            "##sk_vis", "visible", &visuals::SkeletonVisVisibleColor.x, kMenuColorRgb );
                        menu_text_rgb_row_right_pick( "##sk_invis", "not visible", &visuals::SkeletonVisInvisColor.x,
                                                      kMenuColorRgb );
                    } else {
                        menu_text_rgb_row_right_pick(
                            "##sk_base_col", "Skeleton color", &visuals::SkeletonColor.x, kMenuColorRgb );
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
            } else {
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
            } else {
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
                } else {
                    ImGui::Spacing( );
                    ImGui::TextDisabled( "Enable World Modulation to edit lighting channels." );
                }
                end_menu_child( "wm_right_channels" );

                ImGui::EndTabItem( );
            } else {
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
                        visuals::ThirdPerson ? jitter_peek_vk_label( visuals::ThirdPersonVk, tp_vk_buf ) : "";
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
                        visuals::JitterPeek ? jitter_peek_vk_label( visuals::JitterPeekVk, jp_vk_buf ) : "";
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
                    spawn_dust_at_local_origin( );
                end_menu_child( "misc_q_br" );

                ImGui::EndGroup( );

                ImGui::PopStyleVar( );
                ImGui::EndTabItem( );
            } else {
                slider_reveal_poll_frame( SliderRevealTab::Misc, false );
            }

            if ( ImGui::BeginTabItem( "SETTINGS" ) ) {
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
