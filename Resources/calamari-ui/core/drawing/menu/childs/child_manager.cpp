#ifndef CHILD_MANAGER_CPP
#define CHILD_MANAGER_CPP

#include <impl/includes.hpp>

using namespace ImGui;

void ChildManager::beginchild( std::string label, ImVec2 size ) {
	struct s {
		float h;
	}; auto& obj = anim_obj( label.c_str( ), 443143223, s { } );

	current = label;

	auto window = GetCurrentWindow( );

	PushStyleVar( ImGuiStyleVar_WindowPadding, { 0, 0 } );
	Begin( label.c_str( ), 0, ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground );
	PopStyleVar( );
	SetWindowSize( CalcItemSize( size, window->Size.x / 2 - GImGui->Style.WindowPadding.x - GImGui->Style.ItemSpacing.x / 2, GetCurrentWindow( )->ContentSize.y + 1 ) );

	window->DrawList->AddText( GetWindowPos( ) + ImVec2 { 10, 0 }, GetColorU32( ImGuiCol_TextDisabled ), LangManager::get( ).translate( label ).c_str( ) );

	float anim = 0;

	SetCursorPos( { 20 + CalcTextSize( LangManager::get( ).translate( label ).c_str( ), 0, 1 ).x, 0 } );
	CompBuilder::get( ).OpenButton( label.c_str( ), { 14, 14 }, [ & ] ( CompBuilder::OpenButtonEnv env ) {
		float rad = ImLerp( IM_PI * 1.5f, IM_PI / 2, env.anim.open );
		HelpManager::get( ).rotate_start( );
		GetWindowDrawList( )->AddText( fonts->get( HASH_STR( "Glyphter.tff" ), 14.00f ), 14, env.bb.Min, col_anim( col_anim( GetColorU32( ImGuiCol_TextDisabled ), GetColorU32( ImGuiCol_TextHovered ), env.anim.hover ), GetColorU32( ImGuiCol_Text ), env.anim.held ), I_CHEVRON__DOWN );
		HelpManager::get( ).rotate_end( rad, HelpManager::get( ).rotation_center( ) );

		anim = ImClamp( env.anim.open, 0.f, 1.f );

		}, true );

	window = GetCurrentWindow( );

	SetCursorPos( { 0, 24 } );
	char temp [ 64 ];
	ImFormatString( temp, sizeof( temp ), "child %s", label.c_str( ) );
	PushStyleVar( ImGuiStyleVar_WindowPadding, { 14, 14 } );
	PushStyleVar( ImGuiStyleVar_ItemSpacing, { 12, 12 } );
	BeginChild( temp, { GetWindowWidth( ), size.y == 0 ? int( ( obj.h + GImGui->Style.WindowPadding.y * 2 - 10 ) * anim ) + 10 : size.y - 24 }, ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoScrollbar );
	obj.h = GetCurrentWindow( )->ContentSize.y;
	//SetWindowSize( { window->Size.x, size.y == 0 ? int( ( GetCurrentWindow( )->ContentSize.y + GImGui->Style.WindowPadding.y * 2 - 10 ) * anim ) + 10 : size.y - GetCursorPos( ).y } );
	PushItemWidth( GetWindowWidth( ) - GImGui->Style.WindowPadding.x * 2 );
}

void ChildManager::endchild( ) {
	PopItemWidth( );
	EndChild( );
	PopStyleVar( 2 );
	EndChild( );
}

void ChildManager::smoothscroll( ) {
	struct s {
		float scroll;
		float scroll_anim;
		bool scroll_active = false;
		float old_scroll;
		ImVec2 old_pos;
	}; auto& obj = anim_obj( GetCurrentWindow( )->Name, 23123, s { } );

	if ( GetCurrentWindow( )->ScrollMax.y == 0 ) return;

	obj.scroll = ImClamp( obj.scroll, 0.f, GetCurrentWindow( )->ScrollMax.y );
	obj.scroll_anim = ImClamp( obj.scroll_anim, 0.f, GetCurrentWindow( )->ScrollMax.y );

	ImRect scrollbar_bb { GetWindowPos( ) + ImVec2{ GetWindowWidth( ) - GImGui->Style.WindowPadding.x / 2 - 2, GImGui->Style.WindowPadding.y }, GetWindowPos( ) + ImVec2{ GetWindowWidth( ) - GImGui->Style.WindowPadding.x / 2 + 2, GetWindowHeight( ) - GImGui->Style.WindowPadding.y } };
	float scroll_h = scrollbar_bb.GetHeight( ) * ( GetWindowHeight( ) / ( GetCurrentWindow( )->ContentSize.y + GImGui->Style.WindowPadding.y * 2 ) );

	GetWindowDrawList( )->AddRectFilled( scrollbar_bb.Min, scrollbar_bb.Max, GetColorU32( ImGuiCol_FrameBg ), 2 );
	GetWindowDrawList( )->AddRectFilled( scrollbar_bb.Min + ImVec2 { 0, ( scrollbar_bb.GetHeight( ) - scroll_h ) * ( GetCurrentWindow( )->Scroll.y / GetCurrentWindow( )->ScrollMax.y ) }, scrollbar_bb.Min + ImVec2 { scrollbar_bb.GetWidth( ), scroll_h + ( scrollbar_bb.GetHeight( ) - scroll_h ) * ( GetCurrentWindow( )->Scroll.y / GetCurrentWindow( )->ScrollMax.y ) }, GetColorU32( ImGuiCol_Scheme ), 2 );

	if ( IsMouseClicked( 0 ) && IsMouseHoveringRect( scrollbar_bb.Min, scrollbar_bb.Max ) && !obj.scroll_active ) {
		obj.old_pos = GetIO( ).MousePos;
		obj.scroll_active = true;
		obj.old_scroll = obj.scroll;
	} if ( !IsMouseDown( 0 ) ) obj.scroll_active = false;

	if ( obj.scroll_active ) {
		GetCurrentWindow( )->Flags |= ImGuiWindowFlags_NoMove;
		obj.scroll = ImClamp( GetCurrentWindow( )->ScrollMax.y * ( ( GetIO( ).MousePos.y - scrollbar_bb.Min.y ) / scrollbar_bb.GetHeight( ) ), 0.f, GetCurrentWindow( )->ScrollMax.y );
	}

	GetCurrentWindow( )->Scroll.y = ImLerp( GetCurrentWindow( )->Scroll.y, obj.scroll, GetIO( ).DeltaTime * 40 );

	ImGuiWindow* wheeling_window = nullptr;
	if ( GImGui->HoveredWindow ) {
		for ( ImGuiWindow* window = GImGui->HoveredWindow; window->Flags & ImGuiWindowFlags_ChildWindow; window = window->ParentWindow ) {
			if ( window->ScrollMax [ ImGuiAxis_Y ] == 0 )
				continue;

			wheeling_window = window;
		}
	}

	if ( !wheeling_window ) return;

	if ( wheeling_window == GetCurrentWindow( ) )
		obj.scroll = ImClamp( obj.scroll - 0 * 50, 0.f, GetCurrentWindow( )->ScrollMax.y );
}

#endif // !CHILD_MANAGER_CPP
