#include "menu_design_functions.hpp"

#include <cmath>
#include <cstdio>
#include <unordered_set>

namespace {

    using Render::MenuDesign::SliderRevealTab;

    constexpr int k_slider_reveal_tab_count = static_cast< int >( SliderRevealTab::Count );
    constexpr float k_slider_reveal_duration_sec = 0.34f;

    constexpr float k_compact_child_height_tau = 0.13f;
    constexpr float k_compact_child_height_snap_px = 0.75f;
    constexpr float k_compact_child_max_dt = 0.05f;
    constexpr float k_compact_child_min_outer_h = 24.f;
    constexpr float k_compact_child_cold_outer_h = 220.f;
    constexpr float k_compact_child_slack_below = 5.f;
    constexpr ImGuiWindowFlags k_compact_child_flags =
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    float g_slider_reveal_start_sec[ k_slider_reveal_tab_count ] = { -1.f, -1.f, -1.f, -1.f };
    bool g_slider_reveal_prev_selected[ k_slider_reveal_tab_count ] = {};
    std::unordered_set<std::uint64_t> g_slider_reveal_engaged_keys;

    float slider_reveal_eased_t( float u_linear_clamped01 ) noexcept
    {
        u_linear_clamped01 = fminf( fmaxf( u_linear_clamped01, 0.f ), 1.f );
        return 1.f - powf( 1.f - u_linear_clamped01, 3.f );
    }

    inline std::uintptr_t slider_reveal_ptr_fold( const void* p ) noexcept
    {
        const auto addr = reinterpret_cast< std::uintptr_t >( p );
        return addr ^ static_cast< std::uintptr_t >( addr >> 32 );
    }

    inline std::uint64_t slider_reveal_engaged_storage_key( SliderRevealTab tab, const void* value_address ) noexcept
    {
        constexpr int k_shift = 56;
        const auto tab_bits =
            static_cast< std::uint64_t >( static_cast< int >( tab ) & 0xff ) << k_shift;
        return tab_bits | ( static_cast< std::uint64_t >( slider_reveal_ptr_fold( value_address ) )
            & ~( ( std::uint64_t{ 0xff } ) << k_shift ) );
    }

    void slider_reveal_clear_engaged_keys_for_tab( SliderRevealTab tab )
    {
        constexpr int k_shift = 56;
        const std::uint64_t tab_sel =
            static_cast< std::uint64_t >( static_cast< int >( tab ) & 0xff ) << k_shift;
        const std::uint64_t selector = tab_sel >> k_shift;
        for ( auto it = g_slider_reveal_engaged_keys.begin( ); it != g_slider_reveal_engaged_keys.end( ); ) {
            if ( ( *it >> k_shift ) == selector )
                it = g_slider_reveal_engaged_keys.erase( it );
            else
                ++it;
        }
    }

    ImGuiID menu_compact_child_storage_key( const char* panel_id ) noexcept
    {
        char buf[ 112 ];
        _snprintf_s( buf, sizeof( buf ), _TRUNCATE, "IceBox_cmp##%s", panel_id );
        return ImHashStr( buf, 0 );
    }

    ImGuiID menu_compact_child_target_storage_key( const char* panel_id ) noexcept
    {
        char buf[ 112 ];
        _snprintf_s( buf, sizeof( buf ), _TRUNCATE, "IceBox_tgt##%s", panel_id );
        return ImHashStr( buf, 0 );
    }

    inline float menu_child_height_esmooth( float cur, float tgt, float dt, float tau ) noexcept
    {
        const float a = ( tau > 1e-6f && dt > 0.f ) ? 1.f - expf( -dt / tau ) : 1.f;
        return cur + ( tgt - cur ) * a;
    }

} // namespace

namespace Render {

    namespace MenuDesign {

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

        float menu_combo_desired_width_px( ) noexcept
        {
            return fmaxf( k_menu_combo_min_width, ImGui::GetContentRegionAvail( ).x * k_menu_combo_width_frac );
        }

        float menu_combo_list_reserve_height_px( int item_count ) noexcept
        {
            if ( item_count <= 0 )
                return 0.f;
            const ImGuiStyle& st = ImGui::GetStyle( );
            return ( ImGui::GetFontSize( ) + st.ItemSpacing.y ) * float( item_count ) - st.ItemSpacing.y + st.WindowPadding.y * 2.f;
        }

        void menu_keybind_place_button_right_in_cell( float button_w ) noexcept
        {
            const float slack = fmaxf( 0.f, ImGui::GetContentRegionAvail( ).x - button_w );
            if ( slack > 0.f )
                ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + slack );
        }

        void menu_checkbox_rgb_row_right_pick(
            const char* row_scope_id,
            const char* checkbox_label,
            bool* checked,
            float* rgb,
            ImGuiColorEditFlags flags ) noexcept
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
            const char* row_scope_id,
            const char* checkbox_label,
            bool* checked,
            float* rgba,
            ImGuiColorEditFlags flags ) noexcept
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

        void menu_text_rgb_row_right_pick(
            const char* row_scope_id,
            const char* left_caption_utf8,
            float* rgb,
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
            const char* row_scope_id,
            const char* left_caption_utf8,
            float* rgba,
            ImGuiColorEditFlags flags ) noexcept
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

        void menu_world_light_row_rgb_pick(
            bool* enabled,
            const char* channel_label,
            float* rgb,
            ImGuiColorEditFlags flags,
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
            }
            else {
                const float w_pick = menu_color_edit_no_inputs_swatch_span( flags );
                const float slack = fmaxf( 0.f, ImGui::GetContentRegionAvail( ).x - w_pick );
                if ( slack > 0.f )
                    ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + slack );
                ImGui::Dummy( ImVec2( w_pick, ImGui::GetFrameHeight( ) ) );
            }
            ImGui::PopID( );
        }

        void menu_draw_tab_strip_accent_band( float thickness_px ) noexcept
        {
            ImGuiWindow* win = ImGui::GetCurrentWindow( );
            if ( !win || win->SkipItems )
                return;
            ImDrawList* dl = win->DrawList;
            const float gap_y = ImGui::GetStyle( ).ItemSpacing.y;
            const ImVec2 c = ImGui::GetCursorScreenPos( );
            const float y0 = c.y - gap_y;
            dl->AddRectFilled(
                ImVec2( win->InnerRect.Min.x, y0 ),
                ImVec2( win->InnerRect.Max.x, y0 + thickness_px ),
                IM_COL32( 237, 20, 32, 255 ) );
        }

        void menu_panel_heading_divider_strip( float max_gradient_width_px ) noexcept
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

            const ImVec4 red_tab( 0.92f, 0.05f, 0.10f, 1.f );
            const ImVec4 red_hi( 0.95f, 0.20f, 0.26f, 1.f );
            const ImVec4 red_active( 1.f, 0.38f, 0.42f, 1.f );
            const ImVec4 red_muted_btn( 0.14f, 0.06f, 0.06f, 1.f );

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

        MenuQuadDims menu_quad_dims( ) noexcept
        {
            const ImVec2 avail = ImGui::GetContentRegionAvail( );
            MenuQuadDims d{};
            d.cell.x = ( avail.x - k_quad_col_gutter ) * 0.5f;
            d.cell.y = 0.f;
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
                    sm = tgt;
                }
                else if ( fabsf( tgt - sm ) <= k_compact_child_height_snap_px ) {
                    sm = tgt;
                }
                else {
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
            const float item_w = fmaxf( 120.f, fminf( 230.f, w * 0.48f ) );
            ImGui::PushItemWidth( item_w );
        }

        void end_menu_child( const char* id ) noexcept
        {
            ImGuiWindow* cw = ImGui::GetCurrentWindow( );
            ImGuiWindow* parent = cw ? cw->ParentWindow : nullptr;
            if ( parent && cw && ( cw->Flags & ImGuiWindowFlags_ChildWindow ) ) {
                const ImGuiStyle& ist = ImGui::GetStyle( );
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

        TabSliderRevealFrame slider_reveal_poll_frame( SliderRevealTab tab, bool tab_body_visible_this_frame )
        {
            TabSliderRevealFrame out;
            const int ti = static_cast< int >( tab );

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
            }
            else {
                g_slider_reveal_prev_selected[ ti ] = false;
                out.eased_t = 1.f;
            }
            return out;
        }

        void slider_reveal_mark_menu_hidden( ) noexcept
        {
            for ( int i = 0; i < k_slider_reveal_tab_count; ++i )
                g_slider_reveal_prev_selected[ i ] = false;
        }

        bool menu_slider_float_reveal(
            SliderRevealTab tab,
            const TabSliderRevealFrame& fr,
            const char* label,
            float* v,
            float v_min,
            float v_max,
            const char* format ) noexcept
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

        bool menu_slider_int_reveal(
            SliderRevealTab tab,
            const TabSliderRevealFrame& fr,
            const char* label,
            int* v,
            int v_min,
            int v_max,
            const char* format )
        {
            const std::uint64_t ky = slider_reveal_engaged_storage_key( tab, v );
            constexpr float k_reveal_eps = 0.997f;

            bool sync_live = fr.eased_t >= k_reveal_eps || g_slider_reveal_engaged_keys.count( ky ) > 0;
            int scratch = sync_live ?
                *v :
                static_cast< int >(
                    floorf( static_cast< float >( v_min ) + static_cast< float >( *v - v_min ) * fr.eased_t + 0.5f ) );

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

    } // namespace MenuDesign

} // namespace Render
