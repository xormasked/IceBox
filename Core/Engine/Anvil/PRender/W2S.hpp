#pragma once

#include "../scimitar.hpp"

inline bool W2S( ubiVector3 position, ubiVector2& Screen )
{
	auto* view = Scimitar::view_translation::get( );
	if ( !view )
		return false;

	const ubiVector3 temp = position - view->get_view_translation( );
	const float x = temp.Dot( view->get_view_right( ) );
	const float y = temp.Dot( view->get_view_up( ) );
	const float z = temp.Dot( view->get_view_forward( ) * -1.f );

	if ( z < 1.0f )
		return false;

	const int width = GetSystemMetrics( SM_CXSCREEN );
	const int height = GetSystemMetrics( SM_CYSCREEN );

	Screen.x = ( width / 2.f ) * ( 1.f + x / view->get_view_fovX( ) / z );
	Screen.y = ( height / 2.f ) * ( 1.f - y / view->get_view_fovY( ) / z );

	return true;
}
