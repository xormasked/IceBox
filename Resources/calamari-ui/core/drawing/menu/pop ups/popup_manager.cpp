#ifndef POPUP_MANAGER_CPP
#define POPUP_MANAGER_CPP

#include <impl/includes.hpp>

using namespace ImGui;

void PopupManager::open_popup( std::function< void( ) > code ) {
	anim_dest = 1.f;
	popup = std::move( code );
	is_valid = true;
}

void PopupManager::close_popup( ) {
	anim_dest = 0.f;
	is_valid = false;
	popup = nullptr;
}

void PopupManager::handle( ) {
    anim = ImLerp( anim, anim_dest, GetIO( ).DeltaTime * 24 );

    if ( anim < 0.05f && anim_dest == 0 ) {
        popup = 0;
    }
    else {
        SetNextWindowPos( GetWindowPos( ) );
        SetNextWindowSize( GetWindowSize( ) );
        PushStyleVar( ImGuiStyleVar_Alpha, anim );
        PushStyleColor( ImGuiCol_WindowBg, GetColorU32( ImGuiCol_WindowBg, 0.96f ) );
        Begin( "popup", 0, ImGuiWindowFlags_NoDecoration );
        {
            BringWindowToFocusFront( GetCurrentWindow( ) );
            BringWindowToDisplayFront( GetCurrentWindow( ) );

            popup( );
        }
        End( );
        PopStyleColor( );
        PopStyleVar( );
    }
}


#endif // !POPUP_MANAGER_CPP
