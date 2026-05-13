#include "../../IceBox.hpp"
#include "../../../Core/Engine/Anvil/AnvilNext LightingEngine/R6LightEngine.hpp"
#include "../../../Core/Utils/memory.hpp"
#include "../../../Resources/config.hpp"

namespace {

	namespace ANLE = AnvilNextLightingEngine;

	constexpr float k_restore_end = 8.205523109e-19f;
	constexpr float k_restore_start = -0.0462269783f;
	constexpr float k_restore_density = 4.f;

	static void write_restore_values( ) noexcept
	{
		Memory::Write<float>( ANLE::get_world_glow_component( ANLE::WCGlowEndDistance ), k_restore_end );
		Memory::Write<float>( ANLE::get_world_glow_component( ANLE::WCGlowStartDistance ), k_restore_start );
		Memory::Write<float>( ANLE::get_world_glow_component( ANLE::WCGlowDensity ), k_restore_density );
		Memory::Write<float>( ANLE::get_world_glow_component( ANLE::WCGlowRed ), 0.f );
		Memory::Write<float>( ANLE::get_world_glow_component( ANLE::WCGlowGreen ), 0.f );
		Memory::Write<float>( ANLE::get_world_glow_component( ANLE::WCGlowBlue ), 0.f );
	}

	static void apply_enabled( ) noexcept
	{
		const float type_f = ( world_modulation::player_glow_type == 1 ) ? 4.f : 0.f;

		Memory::Write<float>( ANLE::get_world_glow_component( ANLE::WCGlowStartDistance ), 0.f );
		Memory::Write<float>( ANLE::get_world_glow_component( ANLE::WCGlowEndDistance ), 0.f );
		Memory::Write<float>( ANLE::get_world_glow_component( ANLE::WCGlowDensity ), type_f );
		Memory::Write<float>( ANLE::get_world_glow_component( ANLE::WCGlowRed ), world_modulation::player_glow_rgb.x * 255.f );
		Memory::Write<float>( ANLE::get_world_glow_component( ANLE::WCGlowGreen ), world_modulation::player_glow_rgb.y * 255.f );
		Memory::Write<float>( ANLE::get_world_glow_component( ANLE::WCGlowBlue ), world_modulation::player_glow_rgb.z * 255.f );
	}

} // namespace

auto IceBox::world_glow_apply( ) -> void
{
	const bool on = world_modulation::player_glow_enabled;
	static bool s_was_on = false;

	if ( !on ) {
		if ( s_was_on ) {
			write_restore_values( );
			s_was_on = false;
		}
		return;
	}

	if ( !s_was_on ) {
		Memory::Write<float>(
		    AnvilNextLightingEngine::get_world_glow_component( AnvilNextLightingEngine::WCGlowStartDistance ), 0.f );
		Memory::Write<float>(
		    AnvilNextLightingEngine::get_world_glow_component( AnvilNextLightingEngine::WCGlowEndDistance ), 0.f );
		s_was_on = true;
	}

	apply_enabled( );
}

auto IceBox::world_glow_prepare_uninject( ) -> void
{
	if ( world_modulation::player_glow_enabled )
		write_restore_values( );
}
