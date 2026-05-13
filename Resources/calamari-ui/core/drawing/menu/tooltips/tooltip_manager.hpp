#pragma once

class TooltipManager {
public:
	void draw( const char* text );

	static TooltipManager& get( ) {
		static TooltipManager s;
		return s;
	}
};