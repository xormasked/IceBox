#pragma once

namespace Render {

	namespace MenuOthers {

		const char* vk_display_label( int vk, char( &buf )[ 64 ] );
		bool any_mouse_button_down( );
		void spawn_dust_at_local_origin( );
		void dump_local_ed_srv_components_to_console( );

	} // namespace MenuOthers

} // namespace Render
