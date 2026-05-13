#pragma once

class HelpManager {
public:
	void rotate_start( );
	ImVec2 rotation_center( );
	void rotate_end( float rad, ImVec2 center );

	static HelpManager& get( ) {
		static HelpManager s;
		return s;
	}
};