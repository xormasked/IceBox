#ifndef VARS_CPP
#define VARS_CPP

#include <impl/includes.hpp>

void HookVars::load( )
{
	this->misc.aspect_ratio = render->game_width / render->game_height;

	this->ui.framerate = { .state = true };
	this->ui.menu = { .state = true , .keybind = { .key = VK_INSERT , .mode = c_keybind_mode::hold } };
}

#endif // !VARS_CPP
