#include "../../IceBox.hpp"

#include "../../../Core/Engine/Anvil/scimitar.hpp"
#include "../../../Core/Utils/memory.hpp"
#include "../../../Resources/config.hpp"

#include <chrono>

namespace {

	std::chrono::steady_clock::time_point s_last_send{};

} // namespace

void IceBox::chat_spammer_tick( )
{
	if ( !visuals::ChatSpamEnabled )
		return;

	if ( !visuals::ChatSpamTeam && !visuals::ChatSpamAll )
		return;

	if ( !visuals::ChatSpamMessage[ 0 ] )
		return;

	auto* const gm = Scimitar::game_manager::get( );
	if ( !Memory::valid_pointer( gm ) )
		return;

	const int interval_ms = visuals::ChatSpamIntervalMs < 10 ? 10 : visuals::ChatSpamIntervalMs;
	const auto interval = std::chrono::milliseconds( interval_ms );
	const auto now = std::chrono::steady_clock::now( );

	if ( now < s_last_send + interval )
		return;

	if ( visuals::ChatSpamTeam )
		gm->send_chat_message( visuals::ChatSpamMessage, true );
	if ( visuals::ChatSpamAll )
		gm->send_chat_message( visuals::ChatSpamMessage, false );

	s_last_send = now;
}
