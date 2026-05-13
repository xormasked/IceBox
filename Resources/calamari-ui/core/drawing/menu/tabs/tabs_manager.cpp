#ifndef TABS_MANAGER_CPP
#define TABS_MANAGER_CPP

#include <impl/includes.hpp>

using namespace ImGui;

bool TabsManager::tab( int i ) 
{
	ImRect bb { GetCurrentWindow( )->DC.CursorPos, GetCurrentWindow( )->DC.CursorPos + ImVec2{ CalcTextSize( LangManager::get( ).translate( tabs [ i ].label ).c_str( ), 0, 1 ).x + 24, GetWindowHeight( ) } };
	CompBuilder::get( ).Selectable( tabs [ i ].label.c_str( ), i == next, bb, [ & ] ( const CompBuilder::SelectableEnv& env ) {
		if ( env.pressed && i != next ) {
			next = i;
			anim_dest = 0.f;
			SearchManager::get( ).clear( );
		}

		GetWindowDrawList( )->AddRectFilledMultiColor( { bb.Min.x, bb.Max.y - 20 }, bb.Max, GetColorU32( ImGuiCol_Scheme, 0 ), GetColorU32( ImGuiCol_Scheme, 0 ), GetColorU32( ImGuiCol_Scheme, env.anim.selected * 0.1f ), GetColorU32( ImGuiCol_Scheme, env.anim.selected * 0.1f ) );
		GetWindowDrawList( )->AddRectFilled( { bb.Min.x, bb.Max.y - 1 }, bb.Max, GetColorU32( ImGuiCol_Scheme, env.anim.selected ) );

		auto col = col_anim( col_anim( GetColorU32( ImGuiCol_TextDisabled ), GetColorU32( ImGuiCol_TextDisabled, 0.6f ), env.anim.hover ), GetColorU32( ImGuiCol_Text ), env.anim.selected );
		auto icon_col = col_anim( col_anim( GetColorU32( ImGuiCol_TextDisabled ), GetColorU32( ImGuiCol_TextDisabled, 0.6f ), env.anim.hover ), GetColorU32( ImGuiCol_Scheme ), env.anim.selected );

		GetWindowDrawList( )->AddText( fonts->get( HASH_STR( "Glyphter.tff" ), 14.00f ), 14, bb.Min + ImVec2 { 0, bb.GetHeight( ) / 2 - 7 }, icon_col, tabs [ i ].icon );
		GetWindowDrawList( )->AddText( bb.Min + ImVec2 { 24, bb.GetHeight( ) / 2 - GImGui->FontSize / 2 }, col, env.label, FindRenderedTextEnd( env.label ) );
		} );

	return false;
}

bool TabsManager::subtab( int i ) 
{
	ImRect bb { GetCurrentWindow( )->DC.CursorPos, GetCurrentWindow( )->DC.CursorPos + CalcTextSize( LangManager::get( ).translate( tabs [ current ].subtabs [ i ] ).c_str( ), 0, 1 ) + ImVec2{ 20, 16 } };
	CompBuilder::get( ).Selectable( tabs [ current ].subtabs [ i ].c_str( ), i == tabs[ current ].next, bb, [ & ] ( const CompBuilder::SelectableEnv& env ) {
		if ( env.pressed && i != tabs [ current ].next ) {
			tabs [ current ].next = i;
			subtabs_anim_dest = 0.f;
		}

		GetWindowDrawList( )->AddRect( bb.Min, bb.Max, GetColorU32( ImGuiCol_Border, env.anim.selected ), GImGui->Style.FrameRounding );

		auto col = col_anim( col_anim( GetColorU32( ImGuiCol_TextDisabled ), GetColorU32( ImGuiCol_TextDisabled, 0.6f ), env.anim.hover ), GetColorU32( ImGuiCol_Text ), env.anim.selected );
		GetWindowDrawList( )->AddText( bb.Min + ImVec2 { 10, 8 }, col, env.label, FindRenderedTextEnd( env.label ) );
		} );

	return false;
}

void TabsManager::render_tabs( float spacing, bool line ) 
{
	BeginGroup( );
	{
		PushStyleVar( ImGuiStyleVar_ItemSpacing, { spacing, spacing } );

		for ( int i = 0; i < tabs.size( ); ++i ) {
			tab( i );

			if ( line ) SameLine( );
		}

		PopStyleVar( );
	}
	EndGroup( );

	handle( );
}

void TabsManager::render_subtabs( float spacing, bool line )
{
	if ( tabs [ current ].subtabs.empty( ) )
		return;

	SetCursorPos( { 14, 14 } );
	BeginGroup( );
	{
		PushStyleVar( ImGuiStyleVar_ItemSpacing, { spacing, spacing } );
		PushStyleVar( ImGuiStyleVar_Alpha, anim * GImGui->Style.Alpha );

		for ( int i = 0; i < tabs [ current ].subtabs.size( ); ++i ) {
			subtab( i );

			if ( line && i < ( tabs [ current ].subtabs.size( ) - 1 ) ) {
				if ( GetCurrentWindow( )->DC.CursorPos.x + CalcTextSize( tabs [ current ].subtabs [ i + 1 ].c_str( ), 0, 1 ).x + 34 < GetCurrentWindow( )->Rect( ).Max.x ) {
					SameLine( );
				}
			}
		}

		PopStyleVar( 2 );
	}
	EndGroup( );

	SetCursorPos( { 0, GetCursorPos( ).y - GImGui->Style.ItemSpacing.y } );
}

void TabsManager::handle( ) 
{
	anim = ImClamp( ImLerp( anim, anim_dest, GetIO( ).DeltaTime * 20.f ), 0.0f, 1.0f );
	subtabs_anim = ImClamp( ImLerp( subtabs_anim, subtabs_anim_dest, GetIO( ).DeltaTime * 20.f ), 0.0f, 1.0f );

	if ( anim < 0.05f ) {
		current = next;
		anim_dest = 1.f;
	}

	if ( subtabs_anim < 0.05f ) {
		tabs [ current ].cur = tabs [ current ].next;
		subtabs_anim_dest = 1.f;
	}
}

void TabsManager::draw_page( ImGuiWindow* window )
{
	if ( tabs [ current ].pages.size( ) <= tabs [ current ].cur )
		return;

	ImGuiWindow* content_window = GetCurrentWindow( );

	ChildManager::get( ).smoothscroll( );

	tabs [ current ].pages [ tabs [ current ].cur ]( );

	SetNextWindowPos( window->Pos );
	SetNextWindowSize( window->Size );
	PushStyleVar( ImGuiStyleVar_WindowPadding, { 0, 0 } );
	Begin( HASH_STR( "glow" ), 0, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration );
	{
		PopStyleVar( );

		BringWindowToFocusFront( GetCurrentWindow( ) );
		BringWindowToDisplayFront( GetCurrentWindow( ) );

		static float anim1 = 0.f, anim2 = 0.f;
		anim1 = ImLerp( anim1, float( content_window->Scroll.y > 0 ), GetIO( ).DeltaTime * 24 );
		anim2 = ImLerp( anim2, float( content_window->Scroll.y < ( content_window->ScrollMax.y - 1 ) ), GetIO( ).DeltaTime * 24 );

		PushStyleColor( ImGuiCol_WindowBg, ImColor { 0, 0, 0 }.Value );
		GetWindowDrawList( )->AddRectFilledMultiColor( content_window->Pos, content_window->Pos + ImVec2 { content_window->Size.x, GImGui->Style.WindowPadding.y }, GetColorU32( ImGuiCol_WindowBg, anim1 ), GetColorU32( ImGuiCol_WindowBg, anim1 ), GetColorU32( ImGuiCol_WindowBg, 0.f ), GetColorU32( ImGuiCol_WindowBg, 0.f ) );
		PopStyleColor( );
		//GetWindowDrawList( )->AddRectFilledMultiColor( content_window->Pos + ImVec2{ 0, content_window->Size.y - GImGui->Style.WindowPadding.y }, content_window->Pos + content_window->Size, GetColorU32( ImGuiCol_WindowBg, 0.f ), GetColorU32( ImGuiCol_WindowBg, 0.f ), GetColorU32( ImGuiCol_WindowBg, anim2 ), GetColorU32( ImGuiCol_WindowBg, anim2 ) );
	}
	End( );
}

void TabsManager::add_page( int tab, std::function< void( ) > code ) 
{
	tabs [ tab ].pages.push_back( code );
}

#endif // !TABS_MANAGER_CPP
