#pragma once

using namespace ImGui;

struct SearchItem {
	std::string label;
	int tab;
	int subtab;
	std::string child;
	std::function< void( ) > code;
};

class SearchManager {
	float anim = 0.f, anim_dest = 1.f;
	std::vector< SearchItem > items;
public:
	char search_buf [ 16 ];

	void additem( std::string label, std::function< void( ) > code );

	std::vector< SearchItem >& get_items( ) {
		return items;
	}

	std::string to_lower( const char* str ) {
		std::string result;

		for ( int i = 0; str [ i ]; ++i ) {
			result += std::tolower( str [ i ] );
		}

		return result;
	}

	bool compare( const char* str, const char* substr ) {
		return strstr( to_lower( str ).c_str( ), to_lower( substr ).c_str( ) );
	}

	void update( ) {
		anim_dest = 0.f;
	}

	float& get_anim( ) {
		return anim;
	}

	void clear( ) {
		memset( search_buf, 0, sizeof( search_buf ) );
		search_buf [ 0 ] = '\0';
	}

	void draw( );

	static SearchManager& get( ) {
		static SearchManager s;
		return s;
	}
};