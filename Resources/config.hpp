#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "External/ImGui/imgui.h"

namespace visuals {
    inline bool RageBot = false;
    inline bool RageBotVisCheck = false;
    inline bool RageBotPenCheck = false;

    inline bool LongMelee = false;

	inline bool ThirdPerson = false;

    inline bool RaycastClosestDebug = false;
    inline bool RunAndShoot = false;
    inline bool UnlockAllMidHook = false;

    inline bool JitterPeek = false;
    inline int JitterPeekVk = 'G'; // default: G

    inline bool AspectRatioHook = false;
    inline float AspectRatio = 1.5f;

    inline bool Tracers = true;
    inline ImVec4 TracerColor = { 1.f, 1.f, 1.f, 1.f };
    inline const char* TracerPos[] = { "Bottom-Middle", "Center", "Top-Middle" };
    inline int TracerSelected = 0;

    inline bool Skeleton = false;
    inline ImVec4 SkeletonColor = { 1.f, 1.f, 1.f, 1.f };
    inline float SkeletonThickness = 1.5f;
    inline bool SkeletonVisCheck = false;

    inline bool FovEnabled = false;
    inline float EyeFovDegrees = 90.f;
    inline float ViewmodelFovDegrees = 50.f;
}

namespace globals {
    inline int ScreenX = GetSystemMetrics( SM_CXSCREEN );
    inline int ScreenY = GetSystemMetrics( SM_CYSCREEN );
    inline float Width = ( float ) ScreenX;
    inline float Height = ( float ) ScreenY;
}

namespace RageBot
{
    inline bool on = false;
    inline bool vischeck = false;
    inline bool pencheck = false;

    inline int DelayMs = 0;
}
