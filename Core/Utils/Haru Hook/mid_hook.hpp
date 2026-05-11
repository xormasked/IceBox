#pragma once

#include "haru_hook.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

namespace mid_hook {

    inline constexpr std::size_t k_max_patch = 32;

    struct MidHook;

    inline std::vector< MidHook* >& registry( )
    {
        static std::vector< MidHook* > v;
        return v;
    }

    inline void registry_add( MidHook* p )
    {
        if ( !p )
            return;
        auto& v = registry( );
        if ( std::find( v.begin( ), v.end( ), p ) == v.end( ) )
            v.push_back( p );
    }

    inline void registry_remove( MidHook* p )
    {
        if ( !p )
            return;
        auto& v = registry( );
        v.erase( std::remove( v.begin( ), v.end( ), p ), v.end( ) );
    }

    // near jmp → executable gateway → FF25 absolute jmp to handler. Stub should resume at *resume_slot (= site + patch_bytes) when non-null.
    struct MidHook {
        bool install( std::uintptr_t hook_site, std::size_t patch_bytes, void* handler, std::uint64_t* resume_slot = nullptr )
        {
            if ( patch_bytes < 5 || patch_bytes > k_max_patch || !hook_site || !handler )
                return false;

#ifdef _WIN64
            const bool was_on = active;

            if ( !active )
                std::memcpy( orig, reinterpret_cast< void* >( hook_site ), patch_bytes );

            if ( resume_slot )
                *resume_slot = hook_site + patch_bytes;

            const auto next_rip = hook_site + 5;
            if ( !gateway ) {
                gateway = HaruHook::allocate_executable_near_rel32( next_rip, 64 );
                if ( !gateway || !HaruHook::write_jump_abs64( gateway, handler ) ) {
                    HaruHook::free_code_cave( gateway );
                    gateway = nullptr;
                    return false;
                }
            }

            if ( !HaruHook::write_jump_rel32( reinterpret_cast< void* >( hook_site ), gateway ) ) {
                if ( !was_on && gateway ) {
                    HaruHook::free_code_cave( gateway );
                    gateway = nullptr;
                }
                return false;
            }
            HaruHook::nop_pad( reinterpret_cast< void* >( hook_site ), patch_bytes, 5 );

            site = hook_site;
            nbytes = patch_bytes;
            active = true;
            registry_add( this );
            return true;
#else
            ( void ) resume_slot;
            ( void ) patch_bytes;
            ( void ) hook_site;
            ( void ) handler;
            return true;
#endif
        }

        void uninstall( )
        {
#ifdef _WIN64
            registry_remove( this );

            if ( !active )
                return;

            DWORD old{};
            if ( HaruHook::virtual_protect( reinterpret_cast< void* >( site ), nbytes, PAGE_EXECUTE_READWRITE, &old ) ) {
                std::memcpy( reinterpret_cast< void* >( site ), orig, nbytes );
                HaruHook::virtual_protect( reinterpret_cast< void* >( site ), nbytes, old, &old );
                HaruHook::flush_icache( reinterpret_cast< void* >( site ), nbytes );
            }
            active = false;
            HaruHook::free_code_cave( gateway );
            gateway = nullptr;
#endif
        }

        bool installed( ) const { return active; }

    private:
        std::uintptr_t site{};
        std::size_t nbytes{};
        std::uint8_t orig[ k_max_patch ]{};
        void* gateway{};
        bool active{};
    };

    inline void uninstall_all( )
    {
#ifdef _WIN64
        const auto copy = registry( );
        for ( MidHook* h : copy )
            if ( h )
                h->uninstall( );
#endif
    }

    inline bool midhook( MidHook& h, std::uintptr_t hook_site, std::size_t patch_bytes, void* handler,
                         std::uint64_t* resume_slot = nullptr )
    {
        return h.install( hook_site, patch_bytes, handler, resume_slot );
    }

    inline void midhook_remove( MidHook& h )
    {
        h.uninstall( );
    }

} // namespace mid_hook
