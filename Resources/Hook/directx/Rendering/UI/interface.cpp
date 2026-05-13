#include "../interface.hpp"

#include "../../../../config.hpp"
#include "../../d3d11hook.hpp"

#include <impl/includes.hpp>
#include <impl/hook_bridge.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>

namespace Render {
	void reset_menu_binding_after_imgui_teardown( );
}

namespace hook_bridge {

	void request_uninject( )
	{
		d3d11::should_uninject = true;
	}

	void clear_hook_font_refs_after_imgui_destroy( )
	{
		if ( fonts )
			fonts->clear_cached_fonts( );
		Render::reset_menu_binding_after_imgui_teardown( );
	}

}

namespace {

	void color_to_imvec4( ImVec4& out, const c_color& c )
	{
		out.x = c.r;
		out.y = c.g;
		out.z = c.b;
		out.w = c.a;
	}

	void imvec4_to_color( c_color& out, const ImVec4& v )
	{
		out.r = v.x;
		out.g = v.y;
		out.b = v.z;
		out.a = v.w;
	}

	void pull_visual_runtime_from_game( HookVars::hook_visual_runtime& v )
	{
		v.rage_bot = visuals::RageBot;
		v.rage_bot_vis_check = visuals::RageBotVisCheck;
		v.rage_bot_pen_check = visuals::RageBotPenCheck;

		v.long_melee = visuals::LongMelee;

		v.third_person = visuals::ThirdPerson;
		v.third_person_vk = visuals::ThirdPersonVk;

		v.raycast_closest_debug = visuals::RaycastClosestDebug;
		v.run_and_shoot = visuals::RunAndShoot;
		v.unlock_all_mid_hook = visuals::UnlockAllMidHook;
		v.no_spread = visuals::NoSpread;
		v.no_recoil = visuals::NoRecoil;

		v.self_revive = visuals::SelfRevive;
		v.self_revive_vk = visuals::SelfReviveVk;

		v.silent_aim = visuals::SilentAim;
		v.silent_aim_vis_check = visuals::SilentAimVisCheck;
		v.silent_aim_fov_degrees = visuals::SilentAimFovDegrees;
		v.silent_aim_show_fov_circle = visuals::SilentAimShowFovCircle;
		v.silent_aim_circle_filled = visuals::SilentAimCircleFilled;
		v.silent_aim_circle_color = visuals::SilentAimCircleColor;
		v.silent_aim_circle_opacity = visuals::SilentAimCircleOpacity;

		v.dust_spawn_radius = visuals::DustSpawnRadius;
		v.dust_spawn_color = visuals::DustSpawnColor;

		v.jitter_peek = visuals::JitterPeek;
		v.jitter_peek_vk = visuals::JitterPeekVk;
		v.jitter_peek_delay_ms = visuals::JitterPeekDelayMs;

		v.chat_spam_enabled = visuals::ChatSpamEnabled;
		v.chat_spam_team = visuals::ChatSpamTeam;
		v.chat_spam_all = visuals::ChatSpamAll;
		v.chat_spam_interval_ms = visuals::ChatSpamIntervalMs;
		std::memcpy( v.chat_spam_message, visuals::ChatSpamMessage, sizeof( v.chat_spam_message ) );

		v.aspect_ratio_hook = visuals::AspectRatioHook;
		v.aspect_ratio = visuals::AspectRatio;

		v.better_light = visuals::BetterLight;

		v.tracers = visuals::Tracers;
		v.tracer_color = visuals::TracerColor;
		v.tracer_selected = visuals::TracerSelected;

		v.skeleton = visuals::Skeleton;
		v.skeleton_color = visuals::SkeletonColor;
		v.skeleton_vis_visible_color = visuals::SkeletonVisVisibleColor;
		v.skeleton_vis_invis_color = visuals::SkeletonVisInvisColor;
		v.skeleton_thickness = visuals::SkeletonThickness;
		v.skeleton_vis_check = visuals::SkeletonVisCheck;

		v.box_esp = visuals::BoxEsp;
		v.box_esp_thickness = visuals::BoxEspThickness;

		v.enemy_outline = visuals::EnemyOutline;
		v.enemy_outline_color = visuals::EnemyOutlineColor;

		v.fov_enabled = visuals::FovEnabled;
		v.eye_fov_degrees = visuals::EyeFovDegrees;
		v.viewmodel_fov_degrees = visuals::ViewmodelFovDegrees;
	}

	void sync_menu_from_visual_runtime( HookVars& h )
	{
		auto& v = h.visual;

		auto& e = h.esp;
		e.skeleton.enabled = v.skeleton;
		e.skeleton.thickness = v.skeleton_thickness;
		imvec4_to_color( e.skeleton.color, v.skeleton_color );
		e.skeleton.visibility_check = v.skeleton_vis_check;
		imvec4_to_color( e.skeleton.visible_color, v.skeleton_vis_visible_color );
		imvec4_to_color( e.skeleton.invisible_color, v.skeleton_vis_invis_color );

		e.box.enabled = v.box_esp;
		e.box.thickness = v.box_esp_thickness;
		imvec4_to_color( e.box.color, v.skeleton_color );

		h.aim.silent_aim.state = v.silent_aim;
		h.aim.silent_aim_fov = v.silent_aim_fov_degrees;

		h.aim.fov_circle.state = v.silent_aim_show_fov_circle;
		imvec4_to_color( h.aim.fov_circle.color, v.silent_aim_circle_color );
		h.aim.fov_circle_filled = v.silent_aim_circle_filled;
		h.aim.fov_circle_opacity = v.silent_aim_circle_opacity;

		h.aim.bullet_tracers.state = v.tracers;
		imvec4_to_color( h.aim.bullet_tracers.color, v.tracer_color );

		h.misc.no_recoil = v.no_recoil;
		h.misc.no_weapon_spread = v.no_spread;
		h.misc.run_and_shoot = v.run_and_shoot;
		h.misc.long_melee.state = v.long_melee;

		h.misc.aspect_ratio_changer.state = v.aspect_ratio_hook;
		h.misc.aspect_ratio = v.aspect_ratio;

		h.misc.camera.fov_editor_enabled = v.fov_enabled;
		h.misc.camera.eye_fov_degrees = v.eye_fov_degrees;
		h.misc.camera.viewmodel_fov_degrees = v.viewmodel_fov_degrees;

		h.misc.movement.third_person = v.third_person;
		h.misc.movement.third_person_vk = v.third_person_vk;

		h.misc.jitter_peek.enabled = v.jitter_peek;
		h.misc.jitter_peek.vk = v.jitter_peek_vk;
		h.misc.jitter_peek.delay_ms = v.jitter_peek_delay_ms;

		h.misc.cheats.unlock_all = v.unlock_all_mid_hook;
		h.misc.cheats.better_light = v.better_light;

		h.misc.instant_revive.state = v.self_revive;
		h.misc.instant_revive.keybind.key = v.self_revive_vk;
	}

	void merge_menu_into_visual_runtime( HookVars& h )
	{
		auto& v = h.visual;
		const auto& e = h.esp;

		v.skeleton = e.skeleton.enabled;
		v.skeleton_thickness = e.skeleton.thickness;
		color_to_imvec4( v.skeleton_color, e.skeleton.color );
		v.skeleton_vis_check = e.skeleton.visibility_check;
		color_to_imvec4( v.skeleton_vis_visible_color, e.skeleton.visible_color );
		color_to_imvec4( v.skeleton_vis_invis_color, e.skeleton.invisible_color );

		v.box_esp = e.box.enabled;
		v.box_esp_thickness = e.box.thickness;
		color_to_imvec4( v.skeleton_color, e.box.color );

		v.silent_aim = h.aim.silent_aim.state;
		v.silent_aim_fov_degrees = h.aim.silent_aim_fov;

		v.silent_aim_show_fov_circle = h.aim.fov_circle.state;
		color_to_imvec4( v.silent_aim_circle_color, h.aim.fov_circle.color );
		v.silent_aim_circle_filled = h.aim.fov_circle_filled;
		v.silent_aim_circle_opacity = h.aim.fov_circle_opacity;

		v.tracers = h.aim.bullet_tracers.state;
		color_to_imvec4( v.tracer_color, h.aim.bullet_tracers.color );

		v.no_recoil = h.misc.no_recoil;
		v.no_spread = h.misc.no_weapon_spread;
		v.run_and_shoot = h.misc.run_and_shoot;
		v.long_melee = h.misc.long_melee.state;

		v.aspect_ratio_hook = h.misc.aspect_ratio_changer.state;
		v.aspect_ratio = h.misc.aspect_ratio;

		v.third_person = h.misc.movement.third_person;
		v.third_person_vk = h.misc.movement.third_person_vk;

		v.jitter_peek = h.misc.jitter_peek.enabled;
		v.jitter_peek_vk = h.misc.jitter_peek.vk;
		v.jitter_peek_delay_ms = h.misc.jitter_peek.delay_ms;

		v.fov_enabled = h.misc.camera.fov_editor_enabled;
		v.eye_fov_degrees = std::clamp( h.misc.camera.eye_fov_degrees, 70.f, 160.f );
		v.viewmodel_fov_degrees = std::clamp( h.misc.camera.viewmodel_fov_degrees, 1.f, 120.f );

		v.unlock_all_mid_hook = h.misc.cheats.unlock_all;
		v.better_light = h.misc.cheats.better_light;

		v.self_revive = h.misc.instant_revive.state;
		v.self_revive_vk = h.misc.instant_revive.keybind.key;

		v.rage_bot = h.combat.rage_bot.on;
		v.rage_bot_vis_check = h.combat.rage_bot.vischeck;
		v.rage_bot_pen_check = h.combat.rage_bot.pencheck;

		RageBot::DelayMs = h.combat.rage_bot.delay_ms;
	}

	void push_visual_runtime_to_game( const HookVars::hook_visual_runtime& v )
	{
		visuals::RageBot = v.rage_bot;
		visuals::RageBotVisCheck = v.rage_bot_vis_check;
		visuals::RageBotPenCheck = v.rage_bot_pen_check;

		visuals::LongMelee = v.long_melee;

		visuals::ThirdPerson = v.third_person;
		visuals::ThirdPersonVk = v.third_person_vk;

		visuals::RaycastClosestDebug = v.raycast_closest_debug;
		visuals::RunAndShoot = v.run_and_shoot;
		visuals::UnlockAllMidHook = v.unlock_all_mid_hook;
		visuals::NoSpread = v.no_spread;
		visuals::NoRecoil = v.no_recoil;

		visuals::SelfRevive = v.self_revive;
		visuals::SelfReviveVk = v.self_revive_vk;

		visuals::SilentAim = v.silent_aim;
		visuals::SilentAimVisCheck = v.silent_aim_vis_check;
		visuals::SilentAimFovDegrees = v.silent_aim_fov_degrees;
		visuals::SilentAimShowFovCircle = v.silent_aim_show_fov_circle;
		visuals::SilentAimCircleFilled = v.silent_aim_circle_filled;
		visuals::SilentAimCircleColor = v.silent_aim_circle_color;
		visuals::SilentAimCircleOpacity = v.silent_aim_circle_opacity;

		visuals::DustSpawnRadius = v.dust_spawn_radius;
		visuals::DustSpawnColor = v.dust_spawn_color;

		visuals::JitterPeek = v.jitter_peek;
		visuals::JitterPeekVk = v.jitter_peek_vk;
		visuals::JitterPeekDelayMs = v.jitter_peek_delay_ms;

		visuals::ChatSpamEnabled = v.chat_spam_enabled;
		visuals::ChatSpamTeam = v.chat_spam_team;
		visuals::ChatSpamAll = v.chat_spam_all;
		visuals::ChatSpamIntervalMs = v.chat_spam_interval_ms;
		std::memcpy( visuals::ChatSpamMessage, v.chat_spam_message, sizeof( visuals::ChatSpamMessage ) );

		visuals::AspectRatioHook = v.aspect_ratio_hook;
		visuals::AspectRatio = v.aspect_ratio;

		visuals::BetterLight = v.better_light;

		visuals::Tracers = v.tracers;
		visuals::TracerColor = v.tracer_color;
		visuals::TracerSelected = v.tracer_selected;

		visuals::Skeleton = v.skeleton;
		visuals::SkeletonColor = v.skeleton_color;
		visuals::SkeletonVisVisibleColor = v.skeleton_vis_visible_color;
		visuals::SkeletonVisInvisColor = v.skeleton_vis_invis_color;
		visuals::SkeletonThickness = v.skeleton_thickness;
		visuals::SkeletonVisCheck = v.skeleton_vis_check;

		visuals::BoxEsp = v.box_esp;
		visuals::BoxEspThickness = v.box_esp_thickness;

		visuals::EnemyOutline = v.enemy_outline;
		visuals::EnemyOutlineColor = v.enemy_outline_color;

		visuals::FovEnabled = v.fov_enabled;
		visuals::EyeFovDegrees = v.eye_fov_degrees;
		visuals::ViewmodelFovDegrees = v.viewmodel_fov_degrees;
	}

	void pull_world_mod_settings_into_vars( HookVars::world_modulation_vars& wm_dst )
	{
		namespace wm = world_modulation;

		wm_dst.enabled = wm::enabled;

		wm_dst.glow.enabled = wm::player_glow_enabled;
		wm_dst.glow.style = wm::player_glow_type;
		wm_dst.glow.rgb = wm::player_glow_rgb;

		wm_dst.lighting.edit = wm::edit_light;
		wm_dst.lighting.rgb = wm::light_rgb;

		wm_dst.reflection.edit = wm::edit_reflection;
		wm_dst.reflection.rgb = wm::reflection_rgb;

		wm_dst.highlight.edit = wm::edit_highlight;
		wm_dst.highlight.rgb = wm::highlight_rgb;

		wm_dst.top_bottom.edit = wm::edit_top_bottom;
		wm_dst.top_bottom.rgb = wm::top_bottom_rgb;

		wm_dst.global_illumination.edit = wm::edit_global_illum;
		wm_dst.global_illumination.rgb = wm::global_illum_rgb;

		wm_dst.magic.edit = wm::edit_magic;
		wm_dst.magic.rgb = wm::magic_rgb;

		wm_dst.top_bottom_fog.edit = wm::edit_top_bottom_fog;
		wm_dst.top_bottom_fog.rgb = wm::top_bottom_fog_rgb;

		wm_dst.sky.edit = wm::edit_sky;
		wm_dst.sky.rgb = wm::sky_rgb;

		wm_dst.highlight_secondary.edit = wm::edit_highlight2;
		wm_dst.highlight_secondary.rgb = wm::highlight2_rgb;
	}

	void push_world_modulation_from_vars( const HookVars::world_modulation_vars& wm_src )
	{
		namespace wm = world_modulation;

		wm::enabled = wm_src.enabled;

		wm::player_glow_enabled = wm_src.glow.enabled;
		wm::player_glow_type = wm_src.glow.style;
		wm::player_glow_rgb = wm_src.glow.rgb;

		wm::edit_light = wm_src.lighting.edit;
		wm::light_rgb = wm_src.lighting.rgb;

		wm::edit_reflection = wm_src.reflection.edit;
		wm::reflection_rgb = wm_src.reflection.rgb;

		wm::edit_highlight = wm_src.highlight.edit;
		wm::highlight_rgb = wm_src.highlight.rgb;

		wm::edit_top_bottom = wm_src.top_bottom.edit;
		wm::top_bottom_rgb = wm_src.top_bottom.rgb;

		wm::edit_global_illum = wm_src.global_illumination.edit;
		wm::global_illum_rgb = wm_src.global_illumination.rgb;

		wm::edit_magic = wm_src.magic.edit;
		wm::magic_rgb = wm_src.magic.rgb;

		wm::edit_top_bottom_fog = wm_src.top_bottom_fog.edit;
		wm::top_bottom_fog_rgb = wm_src.top_bottom_fog.rgb;

		wm::edit_sky = wm_src.sky.edit;
		wm::sky_rgb = wm_src.sky.rgb;

		wm::edit_highlight2 = wm_src.highlight_secondary.edit;
		wm::highlight2_rgb = wm_src.highlight_secondary.rgb;
	}

	void pull_world_edit_into_vars( HookVars::world_edit_vars& we_dst )
	{
		we_dst.enabled = world_edit::enabled;
		we_dst.clarity = world_edit::clarity;
		we_dst.saturation = world_edit::saturation;
		we_dst.brightness = world_edit::brightness;
		we_dst.illumination = world_edit::illumination;
	}

	void push_world_edit_from_vars( const HookVars::world_edit_vars& we_src )
	{
		world_edit::enabled = we_src.enabled;
		world_edit::clarity = we_src.clarity;
		world_edit::saturation = we_src.saturation;
		world_edit::brightness = we_src.brightness;
		world_edit::illumination = we_src.illumination;
	}

	void sync_combat_rage_from_visual_mirror( HookVars& h )
	{
		h.combat.rage_bot.on = h.visual.rage_bot;
		h.combat.rage_bot.vischeck = h.visual.rage_bot_vis_check;
		h.combat.rage_bot.pencheck = h.visual.rage_bot_pen_check;
		h.combat.rage_bot.delay_ms = RageBot::DelayMs;
	}

	void refresh_hook_only_mirrors_from_globals( HookVars& h )
	{
		pull_world_mod_settings_into_vars( h.world.modulation );
		pull_world_edit_into_vars( h.world.edit );
		sync_combat_rage_from_visual_mirror( h );
	}

	void sync_hook_vars_from_game( HookVars& h )
	{
		pull_visual_runtime_from_game( h.visual );
		refresh_hook_only_mirrors_from_globals( h );
		sync_menu_from_visual_runtime( h );
	}

	void sync_game_from_hook_vars( HookVars& h )
	{
		merge_menu_into_visual_runtime( h );
		push_visual_runtime_to_game( h.visual );
		push_world_modulation_from_vars( h.world.modulation );
		push_world_edit_from_vars( h.world.edit );
		refresh_hook_only_mirrors_from_globals( h );
	}

} // namespace

namespace Render {

	namespace {
		ImGuiContext* g_menu_bound_imgui_ctx = nullptr;
	}

	void reset_menu_binding_after_imgui_teardown( )
	{
		g_menu_bound_imgui_ctx = nullptr;
	}

	auto user_interface( ) -> void
	{
		if ( !menu_open )
			return;

		ImGuiContext* const ctx = ImGui::GetCurrentContext( );
		if ( !ctx )
			return;

		render->g_pd3dDevice = d3d11::p_device;
		const ImGuiIO& io = ImGui::GetIO( );
		render->game_width = io.DisplaySize.x;
		render->game_height = io.DisplaySize.y;

		if ( ctx != g_menu_bound_imgui_ctx ) {
			g_menu_bound_imgui_ctx = ctx;
			vars->load( );
			menu->initialize( );
		}

		sync_hook_vars_from_game( *vars );
		menu->draw( );
		sync_game_from_hook_vars( *vars );
	}

} // namespace Render
