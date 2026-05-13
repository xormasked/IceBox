#ifndef OPTION_CPP
#define OPTION_CPP

#include <impl/includes.hpp>

bool option_interface::c_keybind::is_active( ) const
{
	switch ( mode ) {
	case c_keybind_mode::hold:
		return GetAsyncKeyState( key );
		break;
	case c_keybind_mode::toggle:
		return GetKeyState( key );
		break;
	case c_keybind_mode::always:
		return true;
		break;
	}

	return false;
}

bool option_interface::c_keybind::did_press( bool inputs )
{
	bool key_is_down = GetAsyncKeyState( key ) & 0x8000;

	bool result = false;
	if ( key_is_down && !was_down )
		result = true;

	was_down = key_is_down;
	return result;
}

#endif // !OPTION_CPP