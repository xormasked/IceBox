#pragma once
#include <iostream>
#include <unordered_map>

#include <Windows.h>
#include <winternl.h>

class Engine {
public:
    static Engine* Get( ) noexcept {
        static Engine instance;
        return &instance;
    }

    template<typename DefaultTy = uintptr_t>
    DefaultTy GetModule( ) noexcept {
        return ( DefaultTy ) NtCurrentTeb( )->ProcessEnvironmentBlock->Reserved3[ 1 ];
    };

    void SetupConsole( ) noexcept;

private:
    Engine( ) noexcept = default;

    Engine( const Engine& ) = delete;
    Engine& operator=( const Engine& ) = delete;

    ~Engine( ) noexcept;
};

namespace Update
{
    inline const auto ImageBase = Engine::Get( )->GetModule( );
    inline auto retaddr = ImageBase;

}

using Update::ImageBase, Update::retaddr;