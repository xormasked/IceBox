#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "External/ImGui/imgui.h"

namespace visuals {
    inline bool RageBot = false;
    inline bool RageBotVisCheck = true;
    inline bool RageBotPenCheck = false;

    inline bool LongMelee = false;

	inline bool ThirdPerson = false;
    inline int ThirdPersonVk = 'V';

    inline bool RaycastClosestDebug = false;
    inline bool RunAndShoot = false;
    inline bool UnlockAllMidHook = false;
    inline bool NoSpread = false;
    inline bool NoRecoil = false;

    inline bool SilentAim = false;
    inline bool SilentAimVisCheck = true;
    inline float SilentAimFovDegrees = 10.f;
    inline bool SilentAimShowFovCircle = true;
    inline bool SilentAimCircleFilled = false;
    inline ImVec4 SilentAimCircleColor = { 1.f, 0.35f, 0.35f, 0.6f };
    inline float SilentAimCircleOpacity = 1.f;

    inline float DustSpawnRadius = 3.f;
    inline ImVec4 DustSpawnColor = { 1.f, 1.f, 1.f, 1.f };

    inline bool JitterPeek = false;
    inline int JitterPeekVk = 'G'; // default: G
    inline int JitterPeekDelayMs = 80;

    inline bool ChatSpamEnabled = false;
    inline bool ChatSpamTeam = false;
    inline bool ChatSpamAll = false;
    inline int ChatSpamIntervalMs = 250;
    inline char ChatSpamMessage[ 256 ] = "";

    inline bool AspectRatioHook = false;
    inline float AspectRatio = 1.5f;

    inline bool BetterLight = false;

    inline bool Tracers = false;
    inline ImVec4 TracerColor = { 1.f, 1.f, 1.f, 1.f };
    inline const char* TracerPos[] = { "Bottom-Middle", "Center", "Top-Middle" };
    inline int TracerSelected = 0;

    inline bool Skeleton = true;
    inline ImVec4 SkeletonColor = { 1.f, 1.f, 1.f, 1.f };
    inline float SkeletonThickness = 1.5f;
    inline bool SkeletonVisCheck = false;

    inline bool EnemyOutline = false;
    inline ImVec4 EnemyOutlineColor = { 1.f, 0.25f, 0.25f, 1.f };

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

namespace world_modulation {
    inline bool enabled = false;

    inline bool player_glow_enabled = false;
    inline int player_glow_type = 0;
    inline ImVec4 player_glow_rgb = { 0.f, 0.f, 0.f, 1.f };

    inline bool edit_light = true;
    inline bool edit_reflection = true;
    inline bool edit_highlight = true;
    inline bool edit_top_bottom = true;
    inline bool edit_global_illum = true;
    inline bool edit_magic = true;
    inline bool edit_top_bottom_fog = true;
    inline bool edit_sky = true;
    inline bool edit_highlight2 = true;

    inline ImVec4 light_rgb = { 1.f, 1.f, 1.f, 1.f };
    inline ImVec4 reflection_rgb = { 1.f, 1.f, 1.f, 1.f };
    inline ImVec4 highlight_rgb = { 1.f, 1.f, 1.f, 1.f };
    inline ImVec4 top_bottom_rgb = { 1.f, 1.f, 1.f, 1.f };
    inline ImVec4 global_illum_rgb = { 1.f, 1.f, 1.f, 1.f };
    inline ImVec4 magic_rgb = { 1.f, 1.f, 1.f, 1.f };
    inline ImVec4 top_bottom_fog_rgb = { 1.f, 1.f, 1.f, 1.f };
    inline ImVec4 sky_rgb = { 1.f, 1.f, 1.f, 1.f };
    inline ImVec4 highlight2_rgb = { 1.f, 1.f, 1.f, 1.f };
}

namespace world_edit {
    inline bool enabled = false;
    inline float clarity = 1.10f;
    inline float saturation = 1.10f;
    inline float brightness = 1.10f;
    inline float illumination = 4.f;
}

namespace RageBot
{
    inline bool on = false;
    inline bool vischeck = false;
    inline bool pencheck = false;

    inline int DelayMs = 0;
}
