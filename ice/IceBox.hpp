#pragma once

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

}
