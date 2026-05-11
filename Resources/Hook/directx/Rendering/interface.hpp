#pragma once
#include <cmath>
#include <vector>
#include <string>



#include "../../../External/ImGui/imgui.h"
#include "../../../External/ImGui/imgui_internal.h"

#include "../../../config.hpp"

#include "chrono"


#define IM_PI 3.14159265358979323846



struct Message {
	std::string text;
	ImVec2 position;
	float timestamp;
};


inline std::vector<Message> messages;
inline const float messageDuration = 2.0f;
inline float currentTime = 0.0f;

namespace Render {
    inline bool menu_open = true;

	void RenderConsole( );
	auto user_interface( ) -> void;
	auto Background( ) -> void;
	auto AddMessage( const std::string& text ) -> void;
	auto Log( ) -> void;

}
