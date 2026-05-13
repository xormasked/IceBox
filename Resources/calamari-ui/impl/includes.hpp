#pragma once

#define _CRT_SECURE_NO_WARNINGS

// Standard Library
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <map>
#include <unordered_map>
#include <typeinfo>
#include <cstdint>
#include <filesystem>
#include <shared_mutex>
#include <mutex>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;

// Types
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

namespace math {
    struct vector2 {
        float x, y;
    };
}

#ifndef HASH_STR
#define HASH_STR(str) str
#endif

// Windows & DirectX
#define NOMINMAX
#include <Windows.h>
#include <d3d11.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

template <typename T>
struct dummy_li_fn {
    T* ptr;
    dummy_li_fn(T* p) : ptr(p) {}
    T* safe_cached() { return ptr; }
    T* get() { return ptr; }
};
#define LI_FN(name) dummy_li_fn<decltype(name)>(&name)

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>
#ifdef IMGUI_ENABLE_FREETYPE
#include <imgui/imgui_freetype.h>
#endif

namespace mock_auth {
    struct Brand {
        ID3D11Texture2D* get_texture() { return nullptr; }
        ImVec4 get_scheme() { return ImVec4( 197.f / 255.f , 87.f / 255.f , 83.f / 255.f , 1.f ); } // a nice red default accent sheme
    };//197, 87, 83
    struct Session {
        Brand* brand = nullptr;
    };
    struct Auth {
        Session* session = nullptr;
    };
}
inline mock_auth::Brand mock_brand_obj;
inline mock_auth::Session mock_session_obj{ &mock_brand_obj };
inline mock_auth::Auth mock_auth_obj{ &mock_session_obj };
inline mock_auth::Auth* monoauth = &mock_auth_obj;

#include <core/drawing/unicodes.hpp>
#include <core/drawing/menu/animations/animations.hpp>

// Variables & Data
#include <core/vars/color/color.hpp>
#include <core/vars/option/option.hpp>
using namespace option_interface;
#include <core/vars/vars.hpp>

// Rendering & Textures
#include <core/drawing/framework/framework.hpp>
#include <core/drawing/fonts/fonts.hpp>
#include <core/drawing/textures/textures.hpp>
#include <core/drawing/sprites/sprites.hpp>
#include <core/render/render.hpp>

#include <core/drawing/binaries.hpp>

#include <core/drawing/menu/components/component_builder.hpp>
#include <core/drawing/menu/widgets/widgets_manager.hpp>
#include <core/drawing/menu/childs/child_manager.hpp>
#include <core/drawing/menu/colors/color_picker.hpp>
#include <core/drawing/menu/help/help_manager.hpp>
#include <core/drawing/menu/language/language_manager.hpp>
#include <core/drawing/menu/pop ups/popup_manager.hpp>
#include <core/drawing/menu/search/search_manager.hpp>
#include <core/drawing/menu/styles/style_manager.hpp>
#include <core/drawing/menu/tabs/tabs_manager.hpp>
#include <core/drawing/menu/tooltips/tooltip_manager.hpp>

#include <core/menu/menu.hpp>
