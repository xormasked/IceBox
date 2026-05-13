#pragma once

class PopupManager {
private:
	std::function<void( )> popup;
	float anim;
	float anim_dest;
	bool is_valid;

public:
	PopupManager( ) : anim( 0.f ), anim_dest( 0.f ), is_valid( false ) { }

	void open_popup( std::function< void( ) > code );
	void close_popup( );
	void handle( );

	static PopupManager& get( ) {
		static PopupManager s { };
		return s;
	}
};