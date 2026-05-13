#ifndef TOOLTIP_MANAGER_HPP
#define TOOLTOP_MANAGER_HPP

#include <impl/includes.hpp>

void TooltipManager::draw( const char* text )
{
	struct s {
		float anim;
	}; auto& obj = anim_obj( text, 2007215, s { } );

	obj.anim = anim( obj.anim, 0.f, 1.f, IsItemHovered( ) );

	if ( obj.anim > 0.01f ) {
		GetForegroundDrawList( )->AddRectFilled( { GImGui->LastItemData.Rect.Min.x, GImGui->LastItemData.Rect.Max.y + 5 }, ImVec2 { GImGui->LastItemData.Rect.Min.x, GImGui->LastItemData.Rect.Max.y + 5 } + CalcTextSize( text ) + ImVec2 { 28, 28 }, GetColorU32( ImGuiCol_WindowBg, obj.anim ), GImGui->Style.FrameRounding );
		GetForegroundDrawList( )->AddRect( { GImGui->LastItemData.Rect.Min.x, GImGui->LastItemData.Rect.Max.y + 5 }, ImVec2 { GImGui->LastItemData.Rect.Min.x, GImGui->LastItemData.Rect.Max.y + 5 } + CalcTextSize( text ) + ImVec2 { 28, 28 }, GetColorU32( ImGuiCol_Border, obj.anim ), GImGui->Style.FrameRounding );
		GetForegroundDrawList( )->AddText( ImVec2 { GImGui->LastItemData.Rect.Min.x, GImGui->LastItemData.Rect.Max.y + 5 } + ImVec2 { 14, 14 }, GetColorU32( ImGuiCol_Text, obj.anim ), text );
	}
}

#endif // !TOOLTIP_MANAGER_HPP
