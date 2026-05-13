#pragma once
#include <drawing/unicodes.hpp>

class TabsManager {
	float anim = 1.f, subtabs_anim = 1.f;
	float anim_dest = 1.f, subtabs_anim_dest = 1.f;

	struct Tab {
		const char* icon;
		std::string label;

		std::vector< std::string > subtabs;

		std::vector< std::function< void( ) > > pages { };

		int next = 0;
		int cur = 0;
	};

	std::vector< Tab > tabs {
		{ I_TARGET, HASH_STR( "Aimbot" ), { HASH_STR( "General" ), HASH_STR( "Weapon Modifications" ) } },
		{ I_EYE, HASH_STR( "Visuals" ), { HASH_STR( "General" ) } },
		{ I_WORLD, HASH_STR( "World" ), { HASH_STR( "World Modulation" ), HASH_STR( "Player Glow" ) } },
		{ I_APPS, HASH_STR( "Misc" ), { HASH_STR( "General" ), HASH_STR( "Viewmodel" ), HASH_STR( "Overlays" ), HASH_STR( "Skybox" ), HASH_STR( "Rust+" ) } },
		{ I_SETTINGS, HASH_STR( "Settings" ), { HASH_STR( "General" ), HASH_STR( "Configs" ) } },
	};
public:
	int current = 0, next = 0;

	bool tab( int i );
	bool subtab( int i );

	void render_tabs( float spacing, bool line = false );
	void render_subtabs( float spacing, bool line = true );
	void handle( );

	void add_page( int tab, std::function< void( ) > code );
	void draw_page( ImGuiWindow* window );

	std::vector< std::function< void( ) > > get_pages( ) {
		std::vector< std::function< void( ) > > res;

		for ( auto& tab : tabs ) {
			for ( auto& page : tab.pages ) {
				res.emplace_back( page );
			}
		}

		return res;
	}

	float& get_subtabs_anim( ) {
		return subtabs_anim;
	}

	float& get_tabs_anim( ) {
		return anim;
	}

	float get_anim( ) {
		return subtabs_anim * anim;
	}

	Tab& get_tab( ) {
		return tabs [ current ];
	}

	Tab& get_tab( int i ) {
		return tabs [ i ];
	}

	std::vector< Tab >& get_tabs( ) {
		return tabs;
	}

	void set_tab( int i ) { current = i; };
	void set_subtab( int i ) { tabs [ current ].cur = i; };

	std::vector< std::string >& get_subtabs( int i ) {
		return tabs [ i ].subtabs;
	}

	static TabsManager& get( ) {
		static TabsManager s { };
		return s;
	}
};