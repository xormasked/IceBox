#pragma once

#include "../../UbiHookBypass/UbiHook.hpp"
#include "../../../Core/Utils/memory.hpp"

namespace rappel_hook {

    using fn_t = void* ( __fastcall* )( void* rcx );
    inline fn_t o_fn = nullptr;

    inline void* __fastcall hk( void* rcx )
    {
        std::printf( "[Rappel Hook] rappel_component = 0x%llX\n", static_cast< unsigned long long >( reinterpret_cast< uintptr_t >( rcx ) ) );
        return o_fn( rcx );
    }

    inline bool install( )
    {
        void* function = reinterpret_cast< void* >( Memory::ImageBase + 0x2CAC380 );
        return hook_fn( function, &hk, &o_fn );
    }
}