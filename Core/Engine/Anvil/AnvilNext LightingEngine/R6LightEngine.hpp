#pragma once

#include "../../../Utils/memory.hpp"

#include <cstdint>
#include <vector>

namespace AnvilNextLightingEngine {

	enum LightingComponent : uint32_t {
		LCLightBlue = 0x2B0,
		LCLightGreen = 0x2AC,
		LCLightRed = 0x2A8,
		LCLightRotation = 0x2CC,
		LCLightPower = 0x2D0,

		LCReflectionRed = 0x200,
		LCReflectionGreen = 0x210,
		LCReflectionBlue = 0x220,

		LCHighlightRed = 0x204,
		LCHighlightGreen = 0x214,
		LCHighlightBlue = 0x224,

		LCTopBottomRed = 0x208,
		LCTopBottomGreen = 0x218,
		LCTopBottomBlue = 0x228,

		LCGlobalEluminationRed = 0x20C,
		LCGlobalEluminationGreen = 0x21C,
		LCGlobalEluminationBlue = 0x22C,

		LCMagicRRB = 0x230,
		LCMagicGGP = 0x240,
		LCMagicBYB = 0x250,

		LCTopBottomFogRed = 0x234,
		LCTopBottomFogGreen = 0x244,
		LCTopBottomFogBlue = 0x254,

		LCSkyColorRed = 0x238,
		LCSkyColorGreen = 0x248,
		LCSkyColorBlue = 0x268,

		LCHighlight2Red = 0x24C,
		LCHighlight2Green = 0x25C,
		LCHighlight2Blue = 0x26C,
	};

	enum WorldComponentGlow : uint32_t {
		WCGlowRed = 0xD0,
		WCGlowGreen = 0xD4,
		WCGlowBlue = 0xD8,
		WCGlowEndDistance = 0x110,
		WCGlowStartDistance = 0x114,
		WCGlowDensity = 0x118,
	};

	inline uintptr_t get_world_glow_component( WorldComponentGlow id ) noexcept {
		uintptr_t component = Memory::Read< uintptr_t >( Memory::ImageBase + 0x05E07858 );
		component = Memory::Read< uintptr_t >( component + 0xB8 );

		return component + id;
	}

	inline uintptr_t get_lighting_component( LightingComponent id ) noexcept
	{
		uintptr_t camera = Memory::Read<uintptr_t>( Memory::ImageBase + 0x5E1F4B8 );
		camera = Memory::Read<uintptr_t>( camera + 0x10 );
		camera = Memory::Read< uintptr_t >( camera + 0x408 );

		return camera + id;
	}

} // namespace AnvilNextLightingEngine
