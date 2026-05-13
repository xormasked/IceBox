#pragma once


struct HookVars {

	void load( );

	struct world_channel {
		bool edit = true;
		ImVec4 rgb { 1.f , 1.f , 1.f , 1.f };
	};

	struct world_modulation_vars {
		bool enabled = false;

		world_channel lighting;
		world_channel reflection;
		world_channel highlight;
		world_channel top_bottom;
		world_channel global_illumination;
		world_channel magic;
		world_channel top_bottom_fog;
		world_channel sky;
		world_channel highlight_secondary;

		struct player_glow_vars {
			bool enabled = false;
			int style = 0;
			ImVec4 rgb { 0.f , 0.f , 0.f , 1.f };
		} glow;
	};

	struct world_edit_vars {
		bool enabled = false;
		float clarity = 1.10f;
		float saturation = 1.10f;
		float brightness = 1.10f;
		float illumination = 4.f;
	};

	struct world_vars {
		world_modulation_vars modulation;
		world_edit_vars edit;
	} world;

	struct esp_skeleton_settings {
		bool enabled = true;
		float thickness = 1.5f;
		c_color color { 255 , 255 , 255 };
		bool visibility_check = false;
		c_color visible_color { 0 , 255 , 0 };
		c_color invisible_color { 255 , 0 , 0 };
	};

	struct esp_box_settings {
		bool enabled = false;
		float thickness = 0.5f;
		c_color color { 255 , 255 , 255 };
	};

	struct esp_settings {
		esp_skeleton_settings skeleton;
		esp_box_settings box;
	} esp;

	struct aim_settings {
		c_option silent_aim;
		float silent_aim_fov = 360.f;
		c_option fov_circle;
		bool fov_circle_filled = false;
		float fov_circle_opacity = 1.f;
		c_option bullet_tracers;
	} aim;

	struct misc_settings {
		bool no_recoil = false;
		bool no_weapon_spread = false;
		bool run_and_shoot = false;
		c_option long_melee;

		c_option aspect_ratio_changer;
		float aspect_ratio = 1.5f;

		c_option fov_changer;
		int camera_fov = 90;

		c_option instant_revive;
	} misc;

	struct rage_bot_settings {
		bool on = false;
		bool vischeck = false;
		bool pencheck = false;
		int delay_ms = 0;
	};

	struct combat_vars {
		rage_bot_settings rage_bot;
	} combat;

	struct hook_visual_runtime {
		bool rage_bot = false;
		bool rage_bot_vis_check = true;
		bool rage_bot_pen_check = false;

		bool long_melee = false;

		bool third_person = false;
		int third_person_vk = 'V';

		bool raycast_closest_debug = false;
		bool run_and_shoot = false;
		bool unlock_all_mid_hook = false;
		bool no_spread = false;
		bool no_recoil = false;

		bool self_revive = false;
		int self_revive_vk = 'R';

		bool silent_aim = false;
		bool silent_aim_vis_check = true;
		float silent_aim_fov_degrees = 10.f;
		bool silent_aim_show_fov_circle = true;
		bool silent_aim_circle_filled = false;
		ImVec4 silent_aim_circle_color = { 1.f , 0.35f , 0.35f , 0.6f };
		float silent_aim_circle_opacity = 1.f;

		float dust_spawn_radius = 3.f;
		ImVec4 dust_spawn_color = { 1.f , 1.f , 1.f , 1.f };

		bool jitter_peek = false;
		int jitter_peek_vk = 'G';
		int jitter_peek_delay_ms = 80;

		bool chat_spam_enabled = false;
		bool chat_spam_team = false;
		bool chat_spam_all = false;
		int chat_spam_interval_ms = 250;
		char chat_spam_message[ 256 ]{ };

		bool aspect_ratio_hook = false;
		float aspect_ratio = 1.5f;

		bool better_light = false;

		bool tracers = false;
		ImVec4 tracer_color = { 1.f , 1.f , 1.f , 1.f };
		int tracer_selected = 0;

		bool skeleton = true;
		ImVec4 skeleton_color = { 1.f , 1.f , 1.f , 1.f };
		ImVec4 skeleton_vis_visible_color = { 0.f , 1.f , 0.f , 1.f };
		ImVec4 skeleton_vis_invis_color = { 1.f , 0.f , 0.f , 1.f };
		float skeleton_thickness = 1.5f;
		bool skeleton_vis_check = false;

		bool box_esp = false;
		float box_esp_thickness = 0.5f;

		bool enemy_outline = false;
		ImVec4 enemy_outline_color = { 1.f , 0.25f , 0.25f , 1.f };

		bool fov_enabled = false;
		float eye_fov_degrees = 90.f;
		float viewmodel_fov_degrees = 50.f;
	} visual;

	struct {
		c_option menu;
		float font_size = 15.00f;
		c_option framerate;
	} ui;
};

inline auto vars = std::make_shared< HookVars >( );
