#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Psapi.h>

#include <climits>
#include <cstdint>
#include <cstring>

#pragma comment( lib, "Psapi.lib" )

namespace HaruHook {

    struct ModuleSpan {
        void* base{};
        size_t size{};
    };

    inline ModuleSpan module_span_w( const wchar_t* module_name )
    {
        const HMODULE mod = GetModuleHandleW( module_name );
        if ( !mod ) return {};
        MODULEINFO mi{};
        if ( !GetModuleInformation( GetCurrentProcess( ), mod, &mi, sizeof( mi ) ) )
            return {};
        return { mi.lpBaseOfDll, mi.SizeOfImage };
    }

    inline ModuleSpan module_span_a( const char* module_name )
    {
        const HMODULE mod = GetModuleHandleA( module_name );
        if ( !mod ) return {};
        MODULEINFO mi{};
        if ( !GetModuleInformation( GetCurrentProcess( ), mod, &mi, sizeof( mi ) ) )
            return {};
        return { mi.lpBaseOfDll, mi.SizeOfImage };
    }

    inline void* allocate_code_cave( size_t bytes )
    {
        return VirtualAlloc( nullptr, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );
    }

    inline void free_code_cave( void* p )
    {
        if ( p ) VirtualFree( p, 0, MEM_RELEASE );
    }

    // Allocated once at DLL injection (see d3d11hook.cpp); released on uninject.
    inline void* injection_code_cave = nullptr;
    inline size_t injection_code_cave_bytes = 0;

    inline void release_injection_cave( )
    {
        free_code_cave( injection_code_cave );
        injection_code_cave = nullptr;
        injection_code_cave_bytes = 0;
    }

#ifdef _WIN64

    // VirtualAlloc at an address reachable by E9 rel32 from reference_next_rip (typically hook_site + 5).
    inline void* allocate_executable_near_rel32( uintptr_t reference_next_rip, size_t bytes )
    {
        constexpr uintptr_t align_mask = 0xFFFF;
        constexpr intptr_t step = static_cast< intptr_t >( 0x10000 );
        constexpr intptr_t max_delta = static_cast< intptr_t >( 0x70000000 );

        auto try_candidate = [&]( uintptr_t aligned_candidate ) -> void* {
            if ( aligned_candidate < 0x10000 )
                return nullptr;
            void* const p =
                VirtualAlloc( reinterpret_cast< LPVOID >( aligned_candidate ), bytes,
                              MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );
            if ( !p )
                return nullptr;
            const intptr_t disp =
                reinterpret_cast< intptr_t >( p ) - static_cast< intptr_t >( reference_next_rip );
            if ( disp >= INT32_MIN && disp <= INT32_MAX )
                return p;
            VirtualFree( p, 0, MEM_RELEASE );
            return nullptr;
        };

        if ( void* const p = try_candidate( reference_next_rip & ~align_mask ) )
            return p;

        const intptr_t base = static_cast< intptr_t >( reference_next_rip );
        for ( intptr_t d = step; d <= max_delta; d += step ) {
            const uintptr_t below = static_cast< uintptr_t >( base - d );
            if ( void* const p = try_candidate( below & ~align_mask ) )
                return p;
            const uintptr_t above = static_cast< uintptr_t >( base + d );
            if ( void* const p = try_candidate( above & ~align_mask ) )
                return p;
        }
        return nullptr;
    }

#endif

    inline bool virtual_protect( void* p, size_t bytes, DWORD new_prot, DWORD* old_out )
    {
        return VirtualProtect( p, bytes, new_prot, old_out ) != 0;
    }

    inline void flush_icache( void* p, size_t bytes )
    {
        FlushInstructionCache( GetCurrentProcess( ), p, bytes );
    }

    // E9 rel32 — fails if target not reachable within ±2GiB from jmp end.
    inline bool write_jump_rel32( void* patch_site, void* target )
    {
        auto* const u8 = static_cast< uint8_t* >( patch_site );
        const uintptr_t next = reinterpret_cast< uintptr_t >( u8 ) + 5;
        const intptr_t delta = reinterpret_cast< intptr_t >( target ) - static_cast< intptr_t >( next );
        if ( delta < INT32_MIN || delta > INT32_MAX )
            return false;

        DWORD old{};
        if ( !virtual_protect( patch_site, 5, PAGE_EXECUTE_READWRITE, &old ) )
            return false;
        u8[ 0 ] = 0xE9;
        *reinterpret_cast< int32_t* >( u8 + 1 ) = static_cast< int32_t >( delta );
        virtual_protect( patch_site, 5, old, &old );
        flush_icache( patch_site, 5 );
        return true;
    }

#ifdef _WIN64

    // FF 25 00000000 + imm64 at [rip+6] — always 14 bytes on patch site.
    inline bool write_jump_abs64( void* patch_site, void* target )
    {
        auto* const u8 = static_cast< uint8_t* >( patch_site );
        DWORD old{};
        if ( !virtual_protect( patch_site, 14, PAGE_EXECUTE_READWRITE, &old ) )
            return false;
        std::memcpy( u8, "\xFF\x25\x00\x00\x00\x00", 6 );
        *reinterpret_cast< uint64_t* >( u8 + 6 ) = reinterpret_cast< uint64_t >( target );
        virtual_protect( patch_site, 14, old, &old );
        flush_icache( patch_site, 14 );
        return true;
    }

    inline constexpr size_t mid_hook_patch_bytes( ) { return 14; }

#else

    inline bool write_jump_abs64( void*, void* ) { return false; }

    inline constexpr size_t mid_hook_patch_bytes( ) { return 5; }

#endif

    inline bool nop_pad( void* patch_site, size_t total_bytes, size_t jmp_bytes_used )
    {
        if ( jmp_bytes_used >= total_bytes ) return true;
        auto* const pad = static_cast< uint8_t* >( patch_site ) + jmp_bytes_used;
        const size_t n = total_bytes - jmp_bytes_used;
        DWORD old{};
        if ( !virtual_protect( pad, n, PAGE_EXECUTE_READWRITE, &old ) )
            return false;
        std::memset( pad, 0x90, n );
        virtual_protect( pad, n, old, &old );
        flush_icache( pad, n );
        return true;
    }

    struct BackupTrampoline {
        void* cave{};
        size_t cave_bytes{};
        size_t stolen_len{};
    };

    inline void free_backup_trampoline( BackupTrampoline& t )
    {
        free_code_cave( t.cave );
        t = {};
    }

    // Copies stolen_bytes from origin into a new cave; tail jumps back to origin + stolen_bytes.
    inline BackupTrampoline backup_instructions_trampoline( void* origin, size_t stolen_bytes )
    {
        BackupTrampoline out{};
        if ( !origin || stolen_bytes == 0 || stolen_bytes > 128 )
            return out;

#ifdef _WIN64
        constexpr size_t tail = 14;
#else
        constexpr size_t tail = 5;
#endif
        const size_t cave_sz = stolen_bytes + tail;
        void* const cave = allocate_code_cave( cave_sz );
        if ( !cave )
            return out;

        std::memcpy( cave, origin, stolen_bytes );

        void* const jmp_src = static_cast< uint8_t* >( cave ) + stolen_bytes;
        void* const resume = static_cast< uint8_t* >( origin ) + stolen_bytes;

#ifdef _WIN64
        if ( !write_jump_abs64( jmp_src, resume ) ) {
            free_code_cave( cave );
            return out;
        }
#else
        if ( !write_jump_rel32( jmp_src, resume ) ) {
            free_code_cave( cave );
            return out;
        }
#endif

        flush_icache( cave, cave_sz );
        out.cave = cave;
        out.cave_bytes = cave_sz;
        out.stolen_len = stolen_bytes;
        return out;
    }

    // Patch hook_site: jmp -> handler. Requires stolen_bytes >= patch jmp size and instruction-aligned steal.
    // On success, *out_tramp receives trampoline (caller must free_backup_trampoline).
    inline bool install_mid_hook( void* hook_site, size_t stolen_bytes, void* handler_routine, BackupTrampoline* out_tramp )
    {
        if ( !hook_site || !handler_routine || !out_tramp )
            return false;

#ifdef _WIN64
        constexpr size_t jmp_sz = 14;
        if ( stolen_bytes < jmp_sz )
            return false;
        BackupTrampoline tramp = backup_instructions_trampoline( hook_site, stolen_bytes );
        if ( !tramp.cave )
            return false;
        if ( !write_jump_abs64( hook_site, handler_routine ) ) {
            free_backup_trampoline( tramp );
            return false;
        }
        if ( !nop_pad( hook_site, stolen_bytes, jmp_sz ) ) {
            free_backup_trampoline( tramp );
            return false;
        }
#else
        constexpr size_t jmp_sz = 5;
        if ( stolen_bytes < jmp_sz )
            return false;
        BackupTrampoline tramp = backup_instructions_trampoline( hook_site, stolen_bytes );
        if ( !tramp.cave )
            return false;
        if ( !write_jump_rel32( hook_site, handler_routine ) ) {
            free_backup_trampoline( tramp );
            return false;
        }
        if ( !nop_pad( hook_site, stolen_bytes, jmp_sz ) ) {
            free_backup_trampoline( tramp );
            return false;
        }
#endif

        flush_icache( hook_site, stolen_bytes );
        *out_tramp = tramp;
        return true;
    }

    inline void* find_pattern( void* search_base, size_t search_len, const uint8_t* pat, size_t pat_len, int occurrence = 1 )
    {
        auto* const base = static_cast< uint8_t* >( search_base );
        if ( !base || search_len < pat_len || !pat || pat_len == 0 )
            return nullptr;

        int seen = 0;
        for ( size_t i = 0; i + pat_len <= search_len; ++i ) {
            if ( std::memcmp( base + i, pat, pat_len ) == 0 ) {
                if ( ++seen == occurrence )
                    return base + i;
            }
        }
        return nullptr;
    }

    inline void* find_pattern_module_w( const wchar_t* module_name, const uint8_t* pat, size_t pat_len, int occurrence = 1 )
    {
        const ModuleSpan m = module_span_w( module_name );
        return find_pattern( m.base, m.size, pat, pat_len, occurrence );
    }

    inline void* find_pattern_module_a( const char* module_name, const uint8_t* pat, size_t pat_len, int occurrence = 1 )
    {
        const ModuleSpan m = module_span_a( module_name );
        return find_pattern( m.base, m.size, pat, pat_len, occurrence );
    }

} // namespace HaruHook
