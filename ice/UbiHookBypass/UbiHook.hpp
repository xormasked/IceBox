#pragma once

#include <Windows.h>
#include <winternl.h>

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <vector>

#include "../../Resources/External/MinHook/MinHook.h"

#ifdef _DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

#define RVA_CALL(fn, rva) fn = reinterpret_cast<decltype(fn)>(reinterpret_cast<uint64_t>(GetModuleHandle(NULL)) + rva)

extern "C" NTSTATUS NTAPI NtProtectVirtualMemory(
    HANDLE,
    PVOID*,
    PSIZE_T,
    ULONG,
    PULONG
);

static inline bool init = false;
using NtProtectVirtualMemory_t =
NTSTATUS( NTAPI* )( HANDLE, PVOID*, PSIZE_T, ULONG, PULONG );

template <typename Fn, typename... Args>
decltype( auto ) call_fn( Fn fn, Args... args )
{
    static_assert( std::is_pointer_v<Fn> );
    return fn( args... );
}

static inline std::vector<void*> g_hooks;

inline bool hook_addr( void* target, void* detour, void** original ) {
    if ( !target || !detour || !original ) return false;

    if ( MH_CreateHook( target, detour, original ) != MH_OK )
        return false;

    if ( MH_EnableHook( target ) != MH_OK )
        return false;

    g_hooks.push_back( target );
    return true;
}

inline auto init_hooks( ) -> bool {
    auto status = MH_Initialize( );
    if ( status != MH_OK && status != MH_ERROR_ALREADY_INITIALIZED )
    {
        LOG( "MH_Initialize failed: %d\n", status );
        return false;
    }

    HMODULE ntdll = GetModuleHandleW( L"ntdll.dll" );
    if ( !ntdll )
    {
        LOG( "GetModuleHandleW failed: %lu\n", GetLastError( ) );
        return false;
    }

    FARPROC proc = GetProcAddress( ntdll, "NtProtectVirtualMemory" );
    if ( !proc )
    {
        LOG( "GetProcAddress failed: %lu\n", GetLastError( ) );
        return false;
    }

    void* address = reinterpret_cast< void* >( proc );

    unsigned char before[ 16 ] = {};
    if ( !ReadProcessMemory( GetCurrentProcess( ), address, before, sizeof( before ), nullptr ) )
    {
        LOG( "ReadProcessMemory(before) failed: %lu\n", GetLastError( ) );
        return false;
    }

    const unsigned char data[ 16 ] = {
        0x4C, 0x8B, 0xD1, 0xB8,
        0x50, 0x00, 0x00, 0x00,
        0xF6, 0x04, 0x25, 0x08,
        0x03, 0xFE, 0x7F, 0x01
    };

    DWORD oldProt = 0;
    if ( !VirtualProtect( address, sizeof( data ), PAGE_EXECUTE_READWRITE, &oldProt ) )
    {
        LOG( "VirtualProtect RW failed: %lu\n", GetLastError( ) );
        return false;
    }

    std::memcpy( address, data, sizeof( data ) );

    DWORD tmp = 0;
    if ( !VirtualProtect( address, sizeof( data ), oldProt, &tmp ) )
    {
        LOG( "VirtualProtect restore failed: %lu\n", GetLastError( ) );
        return false;
    }

    if ( !FlushInstructionCache( GetCurrentProcess( ), address, sizeof( data ) ) )
    {
        LOG( "FlushInstructionCache failed: %lu\n", GetLastError( ) );
        return false;
    }

    unsigned char after[ 16 ] = {};
    if ( !ReadProcessMemory( GetCurrentProcess( ), address, after, sizeof( after ), nullptr ) )
    {
        LOG( "ReadProcessMemory(after) failed: %lu\n", GetLastError( ) );
        return false;
    }

    if ( std::memcmp( after, data, sizeof( data ) ) != 0 )
    {
        LOG( "Patch verification failed\n" );
        return false;
    }

    LOG( "OLD: " );
    for ( size_t i = 0; i < sizeof( data ); ++i )
        LOG( "%02X ", before[ i ] );

    LOG( "\nNEW: " );
    for ( size_t i = 0; i < sizeof( data ); ++i )
        LOG( "%02X ", after[ i ] );

    LOG( "\n" );

    init = true;
    return true;
}

template <typename T>
inline auto hook_fn( void* target, T detour, T* original ) -> bool {
    if ( !init ) return false;

    static_assert( std::is_pointer_v<T> );
    if ( !original ) return false;

    return hook_addr(
        target,
        reinterpret_cast< void* >( detour ),
        reinterpret_cast< void** >( original )
    );
}

inline auto unhook( ) -> void {
    for ( auto addr : g_hooks ) {
        MH_DisableHook( addr );
        MH_RemoveHook( addr );
    }

    g_hooks.clear( );
}
