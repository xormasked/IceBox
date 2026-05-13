#pragma once

#include <impl/includes.hpp>

class Watermark
{
public:
	static void Draw( float menuAlpha )
	{
		if ( menuAlpha <= 0.01f )
			return;

		constexpr float W     = 341.f;
		constexpr float H     = 29.f;
		constexpr float FONT_SZ = 12.f;
		constexpr float SEP_W   = 1.f;
		constexpr float SEP_GAP = 8.f;
		constexpr float ICON_SZ = 14.f;

		ImVec2 displaySize = ImGui::GetIO( ).DisplaySize;
		ImGui::SetNextWindowPos( ImVec2( ( displaySize.x - W ) * 0.5f , 10.f ) , ImGuiCond_Always );
		ImGui::SetNextWindowSize( ImVec2( W , H ) );

		ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding , 6.f );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding , ImVec2( 0.f , 0.f ) );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize , 1.f );
		ImGui::PushStyleVar( ImGuiStyleVar_Alpha , menuAlpha );
		ImGui::PushStyleColor( ImGuiCol_WindowBg , ImVec4( 12.f / 255.f , 12.f / 255.f , 12.f / 255.f , 1.f ) );
		ImGui::PushStyleColor( ImGuiCol_Border , ImColor( 255 , 231 , 231 , 20 ).Value );

		if ( ImGui::Begin( "##watermark" , nullptr ,
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoScrollWithMouse |
				ImGuiWindowFlags_NoSavedSettings ) )
		{
			ImVec2      pos = ImGui::GetWindowPos( );
			ImDrawList* dl  = ImGui::GetWindowDrawList( );

			ImFont* iconFont = fonts->get( HASH_STR( "Glyphter.tff" ) , ICON_SZ );
			ImFont* boldFont = fonts->get( HASH_STR( "Geist-SemiBold.ttf" ) , FONT_SZ );
			ImFont* regFont  = fonts->get( HASH_STR( "Geist-Regular.ttf" ) , FONT_SZ );
			if ( !boldFont )
				boldFont = ImGui::GetFont( );
			if ( !regFont )
				regFont = ImGui::GetFont( );

			struct Segment {
				std::string text;
				ImFont*     font;
				ImU32       col;
			};
			std::vector< Segment > segs = {
				{ "Calamari Rust" , boldFont , ImColor( 215 , 215 , 215 ) },
				{ "UID : 1" , regFont , ImColor( 115 , 115 , 115 ) },
			};

			float iconW = 0.f;
			if ( iconFont )
				iconW = iconFont->CalcTextSizeA( ICON_SZ , FLT_MAX , -1.f , I_APPS ).x + 6.f;

			float totalW = iconW;
			for ( auto& s : segs )
				totalW += ( SEP_W + SEP_GAP * 2.f ) + s.font->CalcTextSizeA( FONT_SZ , FLT_MAX , -1.f , s.text.c_str( ) ).x;

			float startX = pos.x + ( W - totalW ) * 0.5f;
			if ( startX < pos.x + 6.f )
				startX = pos.x + 6.f;

			float cy = pos.y + H * 0.5f - FONT_SZ * 0.5f;
			float x  = startX;

			if ( iconFont )
			{
				float icY = pos.y + H * 0.5f - ICON_SZ * 0.5f;
				dl->AddText( iconFont , ICON_SZ , ImVec2( x , icY ) , ImColor( 170 , 170 , 170 ) , I_APPS );
				x += iconW;
			}

			auto DrawSep = [ & ] ( ) {
				x += SEP_GAP;
				dl->AddRectFilled( ImVec2( x , pos.y + 8.f ) , ImVec2( x + SEP_W , pos.y + H - 8.f ) ,
					ImColor( 255 , 231 , 231 , 35 ) );
				x += SEP_W + SEP_GAP;
			};

			for ( auto& s : segs )
			{
				DrawSep( );
				dl->AddText( s.font , FONT_SZ , ImVec2( x , cy ) , s.col , s.text.c_str( ) );
				x += s.font->CalcTextSizeA( FONT_SZ , FLT_MAX , -1.f , s.text.c_str( ) ).x;
			}
		}
		ImGui::End( );

		ImGui::PopStyleColor( 2 );
		ImGui::PopStyleVar( 4 );
	}
};
