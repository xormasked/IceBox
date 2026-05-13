#pragma once

#include "haru_hook.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

namespace mid_hook {

inline constexpr std::size_t k_max_patch = 32;

struct MidHook;

inline std::vector<MidHook*>& registry( )
{
    static std::vector<MidHook*> v;
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

#ifdef _WIN64

inline bool hook_site_readable( std::uintptr_t hook_site, std::size_t patch_bytes )
{
    if ( !hook_site || patch_bytes == 0 || patch_bytes > k_max_patch )
        return false;

    const std::uintptr_t limit = hook_site + patch_bytes;
    for ( std::uintptr_t cursor = hook_site; cursor < limit; ) {
        MEMORY_BASIC_INFORMATION mbi{};
        if ( VirtualQuery( reinterpret_cast<const void*>( cursor ), &mbi, sizeof( mbi ) ) != sizeof( mbi ) )
            return false;
        if ( mbi.State != MEM_COMMIT )
            return false;

        const std::uintptr_t region_end =
            reinterpret_cast<std::uintptr_t>( mbi.BaseAddress ) + mbi.RegionSize;
        if ( limit <= region_end )
            return true;
        cursor = region_end;
    }
    return false;
}

inline bool restore_original_bytes( std::uintptr_t hook_site, const std::uint8_t* backup, std::size_t nbytes )
{
    if ( !hook_site || !backup || nbytes == 0 )
        return false;

    void* const p = reinterpret_cast<void*>( hook_site );
    DWORD old_prot{};
    if ( !HaruHook::virtual_protect( p, nbytes, PAGE_EXECUTE_READWRITE, &old_prot ) )
        return false;

    std::memcpy( p, backup, nbytes );

    DWORD ignored{};
    HaruHook::virtual_protect( p, nbytes, old_prot, &ignored );
    HaruHook::flush_icache( p, nbytes );
    return true;
}

#endif

// Patch site: E9 rel32 → gateway cave → FF25 to handler (see haru_hook.hpp).
// resume_slot, when non-null, receives hook_site + patch_bytes for stub resume.

struct MidHook {
    [[nodiscard]] bool install(
        std::uintptr_t hook_site,
        std::size_t patch_bytes,
        void* handler,
        std::uint64_t* resume_slot = nullptr )
    {
        if ( patch_bytes < 5 || patch_bytes > k_max_patch || !hook_site || !handler )
            return false;

#ifdef _WIN64
        if ( !hook_site_readable( hook_site, patch_bytes ) )
            return false;

        if ( !active )
            std::memcpy( orig, reinterpret_cast<void*>( hook_site ), patch_bytes );

        void* site_ptr = reinterpret_cast<void*>( hook_site );
        const std::uintptr_t next_rip = hook_site + 5;
        const bool own_gateway = !gateway;

        auto drop_owned_gateway = [&, own_gateway]() {
            if ( own_gateway && gateway ) {
                HaruHook::free_code_cave( gateway );
                gateway = nullptr;
            }
        };

        if ( !gateway ) {
            void* const cave = HaruHook::allocate_executable_near_rel32( next_rip, 64 );
            if ( !cave || !HaruHook::write_jump_abs64( cave, handler ) ) {
                HaruHook::free_code_cave( cave );
                return false;
            }
            gateway = cave;
        }

        if ( !HaruHook::write_jump_rel32( site_ptr, gateway ) ) {
            drop_owned_gateway( );
            return false;
        }

        if ( !HaruHook::nop_pad( site_ptr, patch_bytes, 5 ) ) {
            (void)restore_original_bytes( hook_site, orig, patch_bytes );
            drop_owned_gateway( );
            return false;
        }

        if ( resume_slot )
            *resume_slot = hook_site + patch_bytes;

        site = hook_site;
        nbytes = patch_bytes;
        active = true;
        registry_add( this );
        HaruHook::flush_icache( site_ptr, patch_bytes );
        return true;
#else
        (void)resume_slot;
        (void)patch_bytes;
        (void)hook_site;
        (void)handler;
        return true;
#endif
    }

    void uninstall( )
    {
#ifdef _WIN64
        registry_remove( this );

        if ( !active )
            return;

        (void)restore_original_bytes( site, orig, nbytes );

        active = false;
        HaruHook::free_code_cave( gateway );
        gateway = nullptr;
#endif
    }

    [[nodiscard]] bool installed( ) const { return active; }

private:
    std::uintptr_t site{};
    std::size_t nbytes{};
    std::uint8_t orig[k_max_patch]{};
    void* gateway{};
    bool active{};
};

inline void uninstall_all( )
{
#ifdef _WIN64
    const std::vector<MidHook*> hooks = registry( );
    for ( MidHook* h : hooks )
        if ( h )
            h->uninstall( );
#endif
}

[[nodiscard]] inline bool midhook(
    MidHook& h,
    std::uintptr_t hook_site,
    std::size_t patch_bytes,
    void* handler,
    std::uint64_t* resume_slot = nullptr )
{
    return h.install( hook_site, patch_bytes, handler, resume_slot );
}

inline void midhook_remove( MidHook& h )
{
    h.uninstall( );
}

} // namespace mid_hook
