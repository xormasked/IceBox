#pragma once

#include "External/ImGui/imgui.h"

namespace visuals {
    inline bool Tracers = true;
    inline ImVec4 TracerColor = { 1.f, 1.f, 1.f, 1.f };
    inline const char* TracerPos[] = { "Bottom-Middle", "Center", "Top-Middle" };
    inline int TracerSelected = 0;

    inline bool Skeleton = false;
    inline ImVec4 SkeletonColor = { 1.f, 1.f, 1.f, 1.f };
    inline float SkeletonThickness = 1.5f;
    inline bool TeamCheck = false;
}
