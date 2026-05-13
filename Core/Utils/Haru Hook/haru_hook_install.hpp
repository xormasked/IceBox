#pragma once

#include "mid_hook.hpp"
#include "../memory.hpp"

namespace HaruHook {

#ifdef _WIN64

// Mid-hook at ImageBase + rva: steals patch_bytes, redirects execution to stub asm.
// Optional resume_rip: asm writes the post-patch return address here when used.

[[nodiscard]] inline bool install(
    mid_hook::MidHook& hook,
    std::uintptr_t rva,
    std::size_t patch_bytes,
    void* stub,
    std::uint64_t* resume_rip = nullptr )
{
    const std::uintptr_t site = Memory::ImageBase + rva;
    return mid_hook::midhook( hook, site, patch_bytes, stub, resume_rip );
}

inline void remove( mid_hook::MidHook& hook )
{
    mid_hook::midhook_remove( hook );
}

#endif

} // namespace HaruHook
