#pragma once

#include "../../../../External/ImGui/imgui.h"
#include "../../../../External/ImGui/imgui_internal.h"

namespace Render {

    namespace MenuDesign {

        inline constexpr ImGuiColorEditFlags kMenuColorRgb =
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_InputRGB |
            ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaPreviewHalf |
            ImGuiColorEditFlags_PickerHueBar;

        inline constexpr ImGuiColorEditFlags kMenuColorRgba =
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_InputRGB |
            ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf |
            ImGuiColorEditFlags_PickerHueBar;

        inline constexpr float k_menu_color_pick_w_rgb = 142.f;
        inline constexpr float k_menu_color_pick_w_rgba = 168.f;

        inline constexpr ImGuiTableFlags k_menu_color_row_table_flags =
            ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoSavedSettings;

        inline constexpr float k_menu_hotkey_btn_w = 76.f;
        inline constexpr float k_menu_hotkey_btn_h = 22.f;
        inline constexpr float k_menu_keybind_cell_w = k_menu_hotkey_btn_w + 12.f;

        inline const ImVec2 k_menu_combo_frame_pad( 5.f, 6.f );
        inline constexpr float k_menu_combo_width_frac = 0.52f;
        inline constexpr float k_menu_combo_min_width = 100.f;

        inline constexpr float k_menu_esp_section_divider_fade_width_px = 88.f;

        inline constexpr bool k_child_border = true;
        inline constexpr float k_tab_content_bottom_gap = 10.f;
        inline constexpr float k_quad_col_gutter = 5.f;
        inline constexpr float k_quad_row_gutter = 6.f;

        enum class SliderRevealTab : int { Visuals = 0, Aimbot = 1, World = 2, Misc = 3, Count = 4 };

        struct TabSliderRevealFrame {
            float eased_t{ 1.f };
        };

        struct MenuQuadDims {
            ImVec2 cell{};
        };

        float menu_color_edit_no_inputs_swatch_span( ImGuiColorEditFlags flags ) noexcept;
        void menu_color_edit_begin_right_in_table_cell( ImGuiColorEditFlags flags ) noexcept;

        float menu_combo_desired_width_px( ) noexcept;
        float menu_combo_list_reserve_height_px( int item_count ) noexcept;

        void menu_keybind_place_button_right_in_cell( float button_w ) noexcept;

        void menu_checkbox_rgb_row_right_pick(
            const char* row_scope_id, const char* checkbox_label, bool* checked, float* rgb, ImGuiColorEditFlags flags ) noexcept;
        void menu_checkbox_rgba_row_right_pick(
            const char* row_scope_id, const char* checkbox_label, bool* checked, float* rgba, ImGuiColorEditFlags flags ) noexcept;
        void menu_text_rgb_row_right_pick(
            const char* row_scope_id, const char* left_caption_utf8, float* rgb, ImGuiColorEditFlags flags ) noexcept;
        void menu_text_rgba_row_right_pick(
            const char* row_scope_id, const char* left_caption_utf8, float* rgba, ImGuiColorEditFlags flags ) noexcept;
        void menu_world_light_row_rgb_pick(
            bool* enabled,
            const char* channel_label,
            float* rgb,
            ImGuiColorEditFlags flags,
            const char* row_id ) noexcept;

        void menu_draw_tab_strip_accent_band( float thickness_px ) noexcept;
        void menu_panel_heading_divider_strip( float max_gradient_width_px = 0.f ) noexcept;
        void menu_panel_heading( const char* title_all_caps ) noexcept;

        void apply_menu_theme_once( ) noexcept;

        MenuQuadDims menu_quad_dims( ) noexcept;
        void begin_menu_child( const char* id, float width_px, bool border ) noexcept;
        void end_menu_child( const char* id ) noexcept;

        TabSliderRevealFrame slider_reveal_poll_frame( SliderRevealTab tab, bool tab_body_visible_this_frame );
        void slider_reveal_mark_menu_hidden( ) noexcept;

        bool menu_slider_float_reveal(
            SliderRevealTab tab,
            const TabSliderRevealFrame& fr,
            const char* label,
            float* v,
            float v_min,
            float v_max,
            const char* format ) noexcept;
        bool menu_slider_int_reveal(
            SliderRevealTab tab,
            const TabSliderRevealFrame& fr,
            const char* label,
            int* v,
            int v_min,
            int v_max,
            const char* format );

    } // namespace MenuDesign

} // namespace Render
