#pragma once

#ifdef _WIN64

#include "../../../Core/Utils/Haru Hook/mid_hook.hpp"
#include "../../../Core/Utils/memory.hpp"

extern "C" {
    extern float aspect_ratio_live;
    extern unsigned long long aspect_mid_resume_storage;
    void aspect_mid_stub( );
}

namespace aspect_ratio {

    inline constexpr std::uintptr_t k_rva = 0xB2FA0E;

    inline mid_hook::MidHook hook;

    inline bool install( )
    {
        return mid_hook::midhook( hook, Memory::ImageBase + k_rva, 8, reinterpret_cast< void* >( aspect_mid_stub ),
            reinterpret_cast< std::uint64_t* >( &aspect_mid_resume_storage ) );
    }

    inline void uninstall( )
    {
        mid_hook::midhook_remove( hook );
    }

    inline bool installed( )
    {
        return hook.installed( );
    }

} // namespace aspect_ratio

#else

namespace aspect_ratio {
    inline bool install( ) { return true; }
    inline void uninstall( ) { }
    inline bool installed( ) { return false; }
}

#endif
