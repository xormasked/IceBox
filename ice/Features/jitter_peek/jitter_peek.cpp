#include "../../IceBox.hpp"

#include "../../../Core/Engine/Anvil/scimitar.hpp"
#include "../../../Core/Utils/memory.hpp"
#include "../../../Resources/config.hpp"

#include <Windows.h>

void IceBox::jitter_peek_tick( float delta_seconds )
{
	static bool s_g_was_held = false;
	static ubiVector4 s_saved{};
	static float s_accum = 0.f;

	if ( !visuals::JitterPeek ) {
		s_g_was_held = false;
		s_accum = 0.f;
		return;
	}

	if ( !Scimitar::round_state::CurrentState( Scimitar::round_state::Prep ) &&
	     !Scimitar::round_state::CurrentState( Scimitar::round_state::Action ) )
		return;


	auto* ent = Scimitar::game_manager::get( )->get_local_controller( )->pawn_decrypt( )->entity_decrypt( );

	const bool hotkey_held = ( GetAsyncKeyState( visuals::JitterPeekVk ) & 0x8000 ) != 0;

	if ( !hotkey_held ) {
		s_g_was_held = false;
		s_accum = 0.f;
		return;
	}

	if ( !s_g_was_held ) {
		s_saved = ent->Origin4( );
		s_g_was_held = true;
		s_accum = 0.f;
	}

	const float interval_sec = static_cast< float >( visuals::JitterPeekDelayMs ) * 0.001f;

	s_accum += delta_seconds;
	while ( s_accum >= interval_sec ) {
		s_accum -= interval_sec;
		ent->set_origin( s_saved );
	}
}
