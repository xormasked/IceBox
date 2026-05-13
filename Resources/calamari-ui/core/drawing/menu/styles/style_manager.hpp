#pragma once

class StyleManager {
public:
    void Styles( ) {
        auto& style = GImGui->Style;

        style.WindowRounding = 6;
        style.WindowPadding = ImVec2 { 0, 0 };
        style.WindowBorderSize = 0;

        style.FrameRounding = 3;
        style.FramePadding = ImVec2 { 11, 11 };
        style.FrameBorderSize = 0;

        style.PopupRounding = 3;
        style.PopupBorderSize = 0;

        style.ChildRounding = 4;
        style.ChildBorderSize = 1;

        style.ItemSpacing = ImVec2 { 12, 12 };
        style.ItemInnerSpacing = ImVec2 { 10, 10 };

        style.ScrollbarRounding = 4;
        style.ScrollbarSize = 4;
        style.WindowMinSize = ImVec2 { 1, 1 };
    }

    void Colors( ) {
        ImVec4* colors = GImGui->Style.Colors;
        auto g_brand = monoauth->session->brand;

        colors [ ImGuiCol_Scheme ] = g_brand->get_scheme( );

        colors [ ImGuiCol_Text ] = ImColor( 234, 221, 221 );
        colors [ ImGuiCol_TextHovered ] = ImColor( 115, 110, 110, int( 0.6f * 255 ) );
        colors [ ImGuiCol_TextDisabled ] = ImColor( 115, 110, 110 );
        colors [ ImGuiCol_TextButton ] = ImColor( 19, 20, 26 );

        colors [ ImGuiCol_WindowBg ] = ImColor( 9, 9, 9 );
        colors [ ImGuiCol_ChildBg ] = ImColor( 9, 9, 9 );
        colors [ ImGuiCol_FrameBg ] = ImColor( 16, 16, 16 );
        colors [ ImGuiCol_FrameBgHovered ] = ImColor( 22, 22, 22 );
        colors [ ImGuiCol_FrameBgActive ] = ImColor( 33, 33, 33 );
        colors [ ImGuiCol_PopupBg ] = colors [ ImGuiCol_FrameBg ];
        colors [ ImGuiCol_Border ] = ImColor( 255, 231, 231, 10 );
        colors [ ImGuiCol_CheckMark ] = ImColor( 255, 255, 255 );
        colors [ ImGuiCol_SliderGrab ] = ImColor( 255, 255, 255 );
        colors [ ImGuiCol_TextSelectedBg ] = ImColor( colors [ ImGuiCol_Scheme ].x, colors [ ImGuiCol_Scheme ].y, colors [ ImGuiCol_Scheme ].z, 0.07f );
        colors [ ImGuiCol_Tab ] = ImColor( 17, 17, 23 );

        colors [ ImGuiCol_Button ] = colors [ ImGuiCol_Scheme ];
        colors [ ImGuiCol_ButtonHovered ] = ImColor { colors [ ImGuiCol_Scheme ].x / 1.15f, colors [ ImGuiCol_Scheme ].y / 1.15f, colors [ ImGuiCol_Scheme ].z / 1.15f, 1.f };
        colors [ ImGuiCol_ButtonActive ] = ImColor { colors [ ImGuiCol_Scheme ].x / 1.35f, colors [ ImGuiCol_Scheme ].y / 1.35f, colors [ ImGuiCol_Scheme ].z / 1.35f, 1.f };

        colors [ ImGuiCol_ScrollbarGrab ] = colors [ ImGuiCol_Scheme ];
        colors [ ImGuiCol_ScrollbarGrabHovered ] = colors [ ImGuiCol_Scheme ];
        colors [ ImGuiCol_ScrollbarGrabActive ] = colors [ ImGuiCol_Scheme ];
        colors [ ImGuiCol_BorderShadow ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
        colors [ ImGuiCol_ScrollbarBg ] = ImVec4( 0.02f, 0.02f, 0.02f, 0.0f );
        colors [ ImGuiCol_Separator ] = colors [ ImGuiCol_Border ];
    }

    static StyleManager& get( ) {
        static StyleManager s { };
        return s;
    }
};