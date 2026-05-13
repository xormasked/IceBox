#include "../../IceBox.hpp"
#include "../../../Core/Engine/Anvil/AnvilNext LightingEngine/R6LightEngine.hpp"
#include "../../../Core/Engine/Anvil/scimitar.hpp"
#include "../../../Core/Utils/memory.hpp"
#include "../../../Resources/config.hpp"

#include <cmath>

namespace {

	namespace ANLE = AnvilNextLightingEngine;

	struct LightingRgbPack {
		ImVec4 light;
		ImVec4 reflection;
		ImVec4 highlight;
		ImVec4 top_bottom;
		ImVec4 global_illum;
		ImVec4 magic;
		ImVec4 top_bottom_fog;
		ImVec4 sky;
		ImVec4 highlight2;
	};

	struct CheatStashState {
		LightingRgbPack rgb{};
		bool have_light = false;
		bool have_reflection = false;
		bool have_highlight = false;
		bool have_top_bottom = false;
		bool have_global_illum = false;
		bool have_magic = false;
		bool have_top_bottom_fog = false;
		bool have_sky = false;
		bool have_highlight2 = false;

		void clear_all( ) noexcept
		{
			have_light = have_reflection = have_highlight = have_top_bottom = false;
			have_global_illum = have_magic = have_top_bottom_fog = have_sky = have_highlight2 = false;
		}
	};

	bool rgb_different( const ImVec4& a, const ImVec4& b ) noexcept
	{
		const float e = 1e-5f;
		return std::fabs( a.x - b.x ) > e || std::fabs( a.y - b.y ) > e || std::fabs( a.z - b.z ) > e;
	}

	void read_rgb( ImVec4& c,
	               ANLE::LightingComponent red,
	               ANLE::LightingComponent green,
	               ANLE::LightingComponent blue ) noexcept
	{
		c.x = Memory::Read<float>( ANLE::get_lighting_component( red ) );
		c.y = Memory::Read<float>( ANLE::get_lighting_component( green ) );
		c.z = Memory::Read<float>( ANLE::get_lighting_component( blue ) );
	}

	void write_rgb( const ImVec4& c,
	                ANLE::LightingComponent red,
	                ANLE::LightingComponent green,
	                ANLE::LightingComponent blue ) noexcept
	{
		Memory::Write<float>( ANLE::get_lighting_component( red ), c.x );
		Memory::Write<float>( ANLE::get_lighting_component( green ), c.y );
		Memory::Write<float>( ANLE::get_lighting_component( blue ), c.z );
	}

	struct Channel {
		bool& edit;
		ImVec4& wm_rgb;
		ImVec4 LightingRgbPack::* pk;
		bool ( CheatStashState::* have_m );
		bool& prev_edit;
		ANLE::LightingComponent r;
		ANLE::LightingComponent g;
		ANLE::LightingComponent b;
	};

	static LightingRgbPack s_game_last{};
	static LightingRgbPack s_frozen_game_restore{};
	static bool s_have_game_restore_snapshot = false;

	static LightingRgbPack s_resume_after_disable{};
	static bool s_have_resume_after_disable = false;

	static LightingRgbPack s_our_last{};

	static LightingRgbPack s_last_game_overwrite{};

	static bool s_prev_edit_light = true;
	static bool s_prev_edit_reflection = true;
	static bool s_prev_edit_highlight = true;
	static bool s_prev_edit_top_bottom = true;
	static bool s_prev_edit_global_illum = true;
	static bool s_prev_edit_magic = true;
	static bool s_prev_edit_top_bottom_fog = true;
	static bool s_prev_edit_sky = true;
	static bool s_prev_edit_highlight2 = true;

	static CheatStashState s_cheat_stash{};

	static Channel g_channels[ ] = {
	    { world_modulation::edit_light,
	      world_modulation::light_rgb,
	      &LightingRgbPack::light,
	      &CheatStashState::have_light,
	      s_prev_edit_light,
	      ANLE::LCLightRed,
	      ANLE::LCLightGreen,
	      ANLE::LCLightBlue },
	    { world_modulation::edit_reflection,
	      world_modulation::reflection_rgb,
	      &LightingRgbPack::reflection,
	      &CheatStashState::have_reflection,
	      s_prev_edit_reflection,
	      ANLE::LCReflectionRed,
	      ANLE::LCReflectionGreen,
	      ANLE::LCReflectionBlue },
	    { world_modulation::edit_highlight,
	      world_modulation::highlight_rgb,
	      &LightingRgbPack::highlight,
	      &CheatStashState::have_highlight,
	      s_prev_edit_highlight,
	      ANLE::LCHighlightRed,
	      ANLE::LCHighlightGreen,
	      ANLE::LCHighlightBlue },
	    { world_modulation::edit_top_bottom,
	      world_modulation::top_bottom_rgb,
	      &LightingRgbPack::top_bottom,
	      &CheatStashState::have_top_bottom,
	      s_prev_edit_top_bottom,
	      ANLE::LCTopBottomRed,
	      ANLE::LCTopBottomGreen,
	      ANLE::LCTopBottomBlue },
	    { world_modulation::edit_global_illum,
	      world_modulation::global_illum_rgb,
	      &LightingRgbPack::global_illum,
	      &CheatStashState::have_global_illum,
	      s_prev_edit_global_illum,
	      ANLE::LCGlobalEluminationRed,
	      ANLE::LCGlobalEluminationGreen,
	      ANLE::LCGlobalEluminationBlue },
	    { world_modulation::edit_magic,
	      world_modulation::magic_rgb,
	      &LightingRgbPack::magic,
	      &CheatStashState::have_magic,
	      s_prev_edit_magic,
	      ANLE::LCMagicRRB,
	      ANLE::LCMagicGGP,
	      ANLE::LCMagicBYB },
	    { world_modulation::edit_top_bottom_fog,
	      world_modulation::top_bottom_fog_rgb,
	      &LightingRgbPack::top_bottom_fog,
	      &CheatStashState::have_top_bottom_fog,
	      s_prev_edit_top_bottom_fog,
	      ANLE::LCTopBottomFogRed,
	      ANLE::LCTopBottomFogGreen,
	      ANLE::LCTopBottomFogBlue },
	    { world_modulation::edit_sky,
	      world_modulation::sky_rgb,
	      &LightingRgbPack::sky,
	      &CheatStashState::have_sky,
	      s_prev_edit_sky,
	      ANLE::LCSkyColorRed,
	      ANLE::LCSkyColorGreen,
	      ANLE::LCSkyColorBlue },
	    { world_modulation::edit_highlight2,
	      world_modulation::highlight2_rgb,
	      &LightingRgbPack::highlight2,
	      &CheatStashState::have_highlight2,
	      s_prev_edit_highlight2,
	      ANLE::LCHighlight2Red,
	      ANLE::LCHighlight2Green,
	      ANLE::LCHighlight2Blue },
	};

	static constexpr size_t g_n_channels = sizeof( g_channels ) / sizeof( g_channels[ 0 ] );

	void read_pack( LightingRgbPack& p ) noexcept
	{
		for ( size_t i = 0; i < g_n_channels; ++i )
			read_rgb( p.*( g_channels[ i ].pk ), g_channels[ i ].r, g_channels[ i ].g, g_channels[ i ].b );
	}

	void write_pack( const LightingRgbPack& p ) noexcept
	{
		for ( size_t i = 0; i < g_n_channels; ++i )
			write_rgb( p.*( g_channels[ i ].pk ), g_channels[ i ].r, g_channels[ i ].g, g_channels[ i ].b );
	}

	void write_pack_enabled_channels_only( const LightingRgbPack& p ) noexcept
	{
		for ( size_t i = 0; i < g_n_channels; ++i ) {
			const Channel& ch = g_channels[ i ];
			if ( ch.edit )
				write_rgb( p.*( ch.pk ), ch.r, ch.g, ch.b );
		}
	}

	void pull_lighting_from_game_into_ui( ) noexcept
	{
		for ( size_t i = 0; i < g_n_channels; ++i ) {
			const Channel& ch = g_channels[ i ];
			read_rgb( ch.wm_rgb, ch.r, ch.g, ch.b );
		}
	}

	void wm_to_pack( LightingRgbPack& p ) noexcept
	{
		for ( size_t i = 0; i < g_n_channels; ++i ) {
			const Channel& ch = g_channels[ i ];
			p.*( ch.pk ) = ch.wm_rgb;
		}
	}

	void pack_to_wm( const LightingRgbPack& p ) noexcept
	{
		for ( size_t i = 0; i < g_n_channels; ++i ) {
			const Channel& ch = g_channels[ i ];
			ch.wm_rgb = p.*( ch.pk );
		}
	}

	void maybe_push_rgb( bool channel_enabled,
	                     ImVec4& current,
	                     ImVec4& shadow_our,
	                     ANLE::LightingComponent red,
	                     ANLE::LightingComponent green,
	                     ANLE::LightingComponent blue ) noexcept
	{
		if ( !channel_enabled )
			return;
		if ( !rgb_different( current, shadow_our ) )
			return;
		write_rgb( current, red, green, blue );
		shadow_our = current;
	}

	void recover_if_game_overwrote(
	    bool channel_enabled,
	    ImVec4& ui,
	    const ImVec4& shadow_our,
	    ImVec4& store_game_here,
	    ANLE::LightingComponent red,
	    ANLE::LightingComponent green,
	    ANLE::LightingComponent blue ) noexcept
	{
		if ( !channel_enabled )
			return;
		ImVec4 mem{};
		read_rgb( mem, red, green, blue );
		if ( !rgb_different( mem, shadow_our ) )
			return;
		if ( rgb_different( ui, shadow_our ) )
			return;
		store_game_here = mem;
		write_rgb( ui, red, green, blue );
	}

	static bool round_in_play( ) noexcept
	{
		return Scimitar::round_state::CurrentState( Scimitar::round_state::Prep ) ||
		       Scimitar::round_state::CurrentState( Scimitar::round_state::Action );
	}

	void restore_frozen_game_to_memory( ) noexcept
	{
		if ( !s_have_game_restore_snapshot )
			return;
		write_pack( s_frozen_game_restore );
	}

	void restore_cheat_from_stash_if_edit_turned_on(
	    bool was_editing,
	    bool now_editing,
	    ImVec4& wm_rgb,
	    ImVec4& shadow_rgb,
	    ImVec4& stash_slot,
	    bool have_stash,
	    ANLE::LightingComponent r,
	    ANLE::LightingComponent g,
	    ANLE::LightingComponent b ) noexcept
	{
		if ( !now_editing || was_editing )
			return;
		if ( !have_stash )
			return;
		wm_rgb = stash_slot;
		shadow_rgb = stash_slot;
		write_rgb( stash_slot, r, g, b );
	}

	void restore_single_channel_to_frozen_if_edit_turned_off(
	    bool was_editing,
	    bool now_editing,
	    ImVec4& wm_rgb,
	    ImVec4& shadow_rgb,
	    const ImVec4& frozen_rgb,
	    ImVec4& stash_slot,
	    bool& have_stash,
	    ANLE::LightingComponent r,
	    ANLE::LightingComponent g,
	    ANLE::LightingComponent b ) noexcept
	{
		if ( !s_have_game_restore_snapshot )
			return;
		if ( !was_editing || now_editing )
			return;
		stash_slot = shadow_rgb;
		have_stash = true;
		wm_rgb = frozen_rgb;
		shadow_rgb = frozen_rgb;
		write_rgb( frozen_rgb, r, g, b );
	}

	bool wm_differs_from_frozen_snapshot( ) noexcept
	{
		if ( !s_have_game_restore_snapshot )
			return false;
		for ( size_t i = 0; i < g_n_channels; ++i ) {
			const Channel& ch = g_channels[ i ];
			if ( rgb_different( ch.wm_rgb, s_frozen_game_restore.*( ch.pk ) ) )
				return true;
		}
		return false;
	}

	void reset_wm_ui_and_memory_to_frozen_snapshot( ) noexcept
	{
		if ( !s_have_game_restore_snapshot )
			return;
		pack_to_wm( s_frozen_game_restore );
		s_our_last = s_frozen_game_restore;
		write_pack( s_frozen_game_restore );
		s_cheat_stash.clear_all( );
	}

} // namespace

bool IceBox::world_modulation_values_differ_from_frozen_snapshot( ) noexcept
{
	if ( !world_modulation::enabled )
		return false;
	return wm_differs_from_frozen_snapshot( );
}

void IceBox::world_modulation_reset_all_to_frozen_snapshot( ) noexcept
{
	reset_wm_ui_and_memory_to_frozen_snapshot( );
}

auto IceBox::world_modulation_monitor_game( ) -> void
{
	if ( !round_in_play( ) )
		return;
	if ( world_modulation::enabled )
		return;

	read_pack( s_game_last );
}

auto IceBox::world_modulation_apply( ) -> void
{
	static bool s_was_enabled = false;

	const bool on = world_modulation::enabled;

	if ( s_was_enabled && !on ) {
		wm_to_pack( s_resume_after_disable );
		s_have_resume_after_disable = true;
		s_cheat_stash.clear_all( );
		restore_frozen_game_to_memory( );
		s_was_enabled = false;
		return;
	}

	if ( !on ) {
		s_was_enabled = false;
		return;
	}

	namespace wm = world_modulation;

	if ( !s_was_enabled ) {
		if ( s_have_resume_after_disable ) {
			pack_to_wm( s_resume_after_disable );
			s_our_last = s_resume_after_disable;
			write_pack_enabled_channels_only( s_our_last );
		} else {
			pull_lighting_from_game_into_ui( );
			wm_to_pack( s_our_last );
			s_frozen_game_restore = s_our_last;
			s_have_game_restore_snapshot = true;
		}
		for ( size_t i = 0; i < g_n_channels; ++i )
			g_channels[ i ].prev_edit = g_channels[ i ].edit;
		s_was_enabled = true;
		return;
	}

	for ( size_t i = 0; i < g_n_channels; ++i ) {
		const Channel& ch = g_channels[ i ];
		restore_cheat_from_stash_if_edit_turned_on(
		    ch.prev_edit,
		    ch.edit,
		    ch.wm_rgb,
		    s_our_last.*( ch.pk ),
		    s_cheat_stash.rgb.*( ch.pk ),
		    s_cheat_stash.*( ch.have_m ),
		    ch.r,
		    ch.g,
		    ch.b );
	}

	for ( size_t i = 0; i < g_n_channels; ++i ) {
		const Channel& ch = g_channels[ i ];
		restore_single_channel_to_frozen_if_edit_turned_off(
		    ch.prev_edit,
		    ch.edit,
		    ch.wm_rgb,
		    s_our_last.*( ch.pk ),
		    s_frozen_game_restore.*( ch.pk ),
		    s_cheat_stash.rgb.*( ch.pk ),
		    s_cheat_stash.*( ch.have_m ),
		    ch.r,
		    ch.g,
		    ch.b );
	}

	for ( size_t i = 0; i < g_n_channels; ++i ) {
		const Channel& ch = g_channels[ i ];
		recover_if_game_overwrote(
		    ch.edit,
		    ch.wm_rgb,
		    s_our_last.*( ch.pk ),
		    s_last_game_overwrite.*( ch.pk ),
		    ch.r,
		    ch.g,
		    ch.b );
	}

	for ( size_t i = 0; i < g_n_channels; ++i ) {
		const Channel& ch = g_channels[ i ];
		maybe_push_rgb( ch.edit, ch.wm_rgb, s_our_last.*( ch.pk ), ch.r, ch.g, ch.b );
	}

	for ( size_t i = 0; i < g_n_channels; ++i )
		g_channels[ i ].prev_edit = g_channels[ i ].edit;
}

auto IceBox::world_modulation_prepare_uninject( ) -> void
{
	restore_frozen_game_to_memory( );
}
