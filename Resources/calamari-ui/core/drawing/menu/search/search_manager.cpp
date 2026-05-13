#ifndef SEARCH_MANAGER_CPP
#define SEARCH_MANAGER_CPP

#include <impl/includes.hpp>

void SearchManager::additem( std::string label, std::function< void( ) > code ) {
	if ( std::find_if( items.begin( ), items.end( ), [ & ] ( const SearchItem& it ) { return it.label == label; } ) != items.end( ) || ( GImGui->CurrentItemFlags & ImGuiItemFlags_NoNav ) )
		return;

	items.push_back( SearchItem { label, TabsManager::get( ).current, TabsManager::get( ).get_tab( ).cur, ChildManager::get( ).get_current( ), code } );
}

void SearchManager::draw( ) {
	std::vector< SearchItem* > res;

	static std::string buf;

	for ( auto& item : items ) {
		if ( compare( LangManager::get( ).translate( item.label ).c_str( ), buf.c_str( ) ) ) {
			res.push_back( &item );
		}
	}

	anim = ImLerp( anim, anim_dest, GetIO( ).DeltaTime * 28 );

	if ( anim < 0.05f ) {
		if ( strlen( search_buf ) > 0 ) anim_dest = 1.f;
		buf = search_buf;
	}

	if ( strlen( search_buf ) == 0 ) anim_dest = 0.f;

	TabsManager::get( ).get_tabs_anim( ) = 0.f;

	PushStyleVar( ImGuiStyleVar_Alpha, anim * GImGui->Style.Alpha );
	BeginGroup( );
	{
		PushItemWidth( GetWindowWidth( ) - 28 );

		for ( int i = 0; i < res.size( ); ++i ) {
			auto& item = *res [ i ];

			TextDisabled( LangManager::get( ).translate( TabsManager::get( ).get_tab( item.tab ).label ).c_str( ) );
			SameLine( 0, 10 );
			PushFont( fonts->get( HASH_STR( "Glyphter.tff" ), 14.00f ) );
			TextDisabled( I_CHEVRON__RIGHT );
			PopFont( );
			SameLine( 0, 24 );
			if ( TabsManager::get( ).get_tab( item.tab ).subtabs.size( ) > 0 ) {
				TextDisabled( TabsManager::get( ).get_tab( item.tab ).subtabs [ item.subtab ].c_str( ) );
				SameLine( 0, 10 );
				PushFont( fonts->get( HASH_STR( "Glyphter.tff" ), 14.00f ) );
				TextDisabled( I_CHEVRON__RIGHT );
				PopFont( );
				SameLine( 0, 24 );
			}
			TextEx( item.child.c_str( ), FindRenderedTextEnd( item.child.c_str( ) ) );

			item.code( );

			if ( i < res.size( ) - 1 )
				WidgetsManager::get( ).Separator( );
		}

		PopItemWidth( );
	}
	EndGroup( );
	PopStyleVar( );
}

#endif // !SEARCH_MANAGER_CPP
