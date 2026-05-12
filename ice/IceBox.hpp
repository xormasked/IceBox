#pragma once

#include "../Core/Maths/havok_math.hpp"

struct ImDrawList;

namespace IceBox {

	auto long_melee( bool enable ) -> void;

	auto rage_bot_run( bool enable ) -> void;

	auto camera_fx_apply_fov( ) -> void;

	bool aspect_ratio_install( );
	void aspect_ratio_uninstall( );
	bool aspect_ratio_installed( );

	bool run_and_shoot_install( );
	void run_and_shoot_uninstall( );
	bool run_and_shoot_installed( );

	bool unlock_all_install( );
	void unlock_all_uninstall( );
	bool unlock_all_installed( );

	auto third_person( bool enable, float delta_seconds ) -> void;

	void third_person_reset( );

	void raycast_debug_hit_marker_decay( float delta_seconds );

	bool raycast_debug_hit_marker_world( ubiVector3& out_world ) noexcept;

	void raycast_closest_player_debug_tick( float delta_seconds );

	void jitter_peek_tick( float delta_seconds );

	void render_raycast_debug_overlay( float delta_seconds, ImDrawList* draw_list );

}
