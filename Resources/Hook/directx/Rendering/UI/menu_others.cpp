#include "menu_others.hpp"

#include "../../../../config.hpp"

#include "../../../../../Core/Engine/Anvil/scimitar.hpp"
#include "../../../../../Core/Utils/memory.hpp"

#include <cstdint>
#include <cstdio>

namespace Render {

    namespace MenuOthers {

        const char* vk_display_label( int vk, char( &buf )[ 64 ] )
        {
            if ( vk >= '0' && vk <= '9' ) {
                sprintf_s( buf, sizeof( buf ), "%c", vk );
                return buf;
            }
            if ( vk >= 'A' && vk <= 'Z' ) {
                sprintf_s( buf, sizeof( buf ), "%c", vk );
                return buf;
            }
            if ( vk >= 'a' && vk <= 'z' ) {
                sprintf_s( buf, sizeof( buf ), "%c", vk - 32 );
                return buf;
            }

            switch ( vk ) {
            case VK_SPACE:
                return "Space";
            case VK_TAB:
                return "Tab";
            case VK_RETURN:
                return "Enter";
            case VK_SHIFT:
                return "Shift";
            case VK_CONTROL:
                return "Ctrl";
            case VK_MENU:
                return "Alt";
            case VK_ESCAPE:
                return "Escape";
            case VK_BACK:
                return "Backspace";
            case VK_DELETE:
                return "Delete";
            case VK_INSERT:
                return "Insert";
            case VK_HOME:
                return "Home";
            case VK_END:
                return "End";
            case VK_PRIOR:
                return "Page Up";
            case VK_NEXT:
                return "Page Down";
            case VK_LEFT:
                return "Left";
            case VK_RIGHT:
                return "Right";
            case VK_UP:
                return "Up";
            case VK_DOWN:
                return "Down";
            case VK_LBUTTON:
                return "Mouse L";
            case VK_RBUTTON:
                return "Mouse R";
            case VK_MBUTTON:
                return "Mouse M";
            case VK_XBUTTON1:
                return "Mouse 4";
            case VK_XBUTTON2:
                return "Mouse 5";
            default:
                sprintf_s( buf, sizeof( buf ), "VK 0x%02X", vk );
                return buf;
            }
        }

        bool any_mouse_button_down( )
        {
            return ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 ) != 0 || ( GetAsyncKeyState( VK_RBUTTON ) & 0x8000 ) != 0 ||
                ( GetAsyncKeyState( VK_MBUTTON ) & 0x8000 ) != 0 || ( GetAsyncKeyState( VK_XBUTTON1 ) & 0x8000 ) != 0 ||
                ( GetAsyncKeyState( VK_XBUTTON2 ) & 0x8000 ) != 0;
        }

        void spawn_dust_at_local_origin( )
        {
            auto* const gm = Scimitar::game_manager::get( );
            if ( !Memory::valid_pointer( gm ) )
                return;
            auto* const lc = gm->get_local_controller( );
            if ( !Memory::valid_pointer( lc ) )
                return;
            auto* const pawn = lc->pawn_decrypt( );
            if ( !Memory::valid_pointer( pawn ) )
                return;
            auto* const ent = pawn->entity_decrypt( );
            if ( !Memory::valid_pointer( ent ) )
                return;

            const ubiVector4 pos = ent->Origin4( );
            const ubiVector4 col(
                visuals::DustSpawnColor.x,
                visuals::DustSpawnColor.y,
                visuals::DustSpawnColor.z,
                visuals::DustSpawnColor.w );

            Scimitar::add_dust( pos, visuals::DustSpawnRadius, col );
        }

        void dump_local_ed_srv_components_to_console( )
        {
            struct Entry {
                const char* label;
                Scimitar::EDESrvComponents id;
            };
            static constexpr Entry k_entries[ ] = {
                { "EDrvAnimation", Scimitar::EDrvAnimation },
                { "EDrvSprinting", Scimitar::EDrvSprinting },
                { "EDrvWeapon", Scimitar::EDrvWeapon },
                { "EDrvMelee", Scimitar::EDrvMelee },
                { "EDrvShooting", Scimitar::EDrvShooting },
                { "EDrvDrone", Scimitar::EDrvDrone },
                { "EDrvGadget", Scimitar::EDrvGadget },
                { "EDrvLeaning", Scimitar::EDrvLeaning },
                { "EDrvVaulting", Scimitar::EDrvVaulting },
                { "EDrvContext", Scimitar::EDrvContext },
                { "EDrvNavigation", Scimitar::EDrvNavigation },
                { "EDrvInteraction", Scimitar::EDrvInteraction },
                { "ESrvR6AIData", Scimitar::ESrvR6AIData },
                { "ESrvEffect", Scimitar::ESrvEffect },
                { "ESrvMobility", Scimitar::ESrvMobility },
                { "ESrvDebug", Scimitar::ESrvDebug },
                { "ESrvGroup", Scimitar::ESrvGroup },
                { "ESrvCoordinator", Scimitar::ESrvCoordinator },
                { "ESrvScripting", Scimitar::ESrvScripting },
                { "ESrvPerception", Scimitar::ESrvPerception },
                { "ESrvLocation", Scimitar::ESrvLocation },
                { "ESrvAbilities", Scimitar::ESrvAbilities },
                { "ESrvHostage", Scimitar::ESrvHostage },
                { "Unknown1", Scimitar::Unknown1 },
                { "Unknown7", Scimitar::Unknown7 },
                { "Unknown10", Scimitar::Unknown10 },
            };

            auto* entity = Scimitar::game_manager::get( )->get_local_controller( )->pawn_decrypt( )->entity_decrypt( );

            std::printf( "[EDESrv] local_entity=%p EDESrvComponents (resolve):\n",
                reinterpret_cast< void* >( entity ) );
            for ( const auto& e : k_entries ) {
                const uintptr_t addr = entity->resolve_component( static_cast< uint64_t >( e.id ) );
                std::printf( "  %-16s [%08X] -> %p\n", e.label, static_cast< unsigned int >( e.id ),
                    reinterpret_cast< void* >( addr ) );
            }
            std::fflush( stdout );
        }

    } // namespace MenuOthers

} // namespace Render
