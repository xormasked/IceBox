#pragma once

#include "../Core/Maths/havok_math.hpp"
#include "../Core/Engine/Anvil/scimitar.hpp"

struct ImDrawList;

namespace IceBox {

	auto long_melee( bool enable ) -> void;

	void long_melee_prepare_uninject( ) noexcept;

	auto rage_bot_run( bool enable ) -> void;

	auto camera_fx_apply_fov( ) -> void;

	auto camera_fx_prepare_uninject( ) -> void;

	auto world_modulation_monitor_game( ) -> void;

	auto world_modulation_apply( ) -> void;

	auto world_modulation_prepare_uninject( ) -> void;

	bool world_modulation_values_differ_from_frozen_snapshot( ) noexcept;

	void world_modulation_reset_all_to_frozen_snapshot( ) noexcept;

	bool world_edit_install( );
	void world_edit_uninstall( );
	bool world_edit_installed( );
	void world_edit_tick( ) noexcept;
	void world_edit_prepare_uninject( ) noexcept;

	auto world_glow_apply( ) -> void;

	auto world_glow_prepare_uninject( ) -> void;

	bool aspect_ratio_install( );
	void aspect_ratio_uninstall( );
	bool aspect_ratio_installed( );

	bool better_light_install( );
	void better_light_uninstall( );
	bool better_light_installed( );
	void better_light_prepare_uninject( ) noexcept;

	bool run_and_shoot_install( );
	void run_and_shoot_uninstall( );
	bool run_and_shoot_installed( );

	bool unlock_all_install( );
	void unlock_all_uninstall( );
	bool unlock_all_installed( );

	bool no_spread_install( );
	void no_spread_uninstall( );
	bool no_spread_installed( );

	bool no_recoil_install( );
	void no_recoil_uninstall( );
	bool no_recoil_installed( );

	void no_recoil_prepare_uninject( ) noexcept;

	void silent_aim_tick( );

	void silent_aim_prepare_uninject( ) noexcept;

	void render_silent_aim_fov_overlay( ImDrawList* draw_list ) noexcept;

	auto third_person( bool enable, float delta_seconds ) -> void;

	void third_person_reset( );

	void raycast_debug_hit_marker_decay( float delta_seconds );

	bool raycast_debug_hit_marker_world( ubiVector3& out_world ) noexcept;

	void raycast_closest_player_debug_tick( float delta_seconds );

	void jitter_peek_tick( float delta_seconds );

	void chat_spammer_tick( );

	void render_raycast_debug_overlay( float delta_seconds, ImDrawList* draw_list );

	void enemy_outlines_tick( );

	void enemy_outlines_prepare_uninject( ) noexcept;

	auto self_revive( bool enable ) -> void;

}
