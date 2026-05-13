#include "../../../IceBox.hpp"
#include "../../../../Core/Engine/Anvil/scimitar.hpp"
#include "../../../../Core/Engine/Anvil/skeletons.h"
#include "../../../../Core/Engine/Anvil/PRender/W2S.hpp"
#include "../../../../Core/Engine/Anvil/Raycasting/RayCasting.hpp"
#include "../../../../Core/Utils/memory.hpp"
#include "../../../../Resources/config.hpp"
#include "../../../../Resources/External/MinHook/MinHook.h"

#include <cmath>
#include <immintrin.h>

#ifdef _WIN64

namespace {

	using ShotFn = __int64 ( __fastcall* )( __int64, __m128*, const __m128i* );

	struct Bone {
		BipedBoneID id;
		uint8_t tier;
	};

	// Lower tier = higher priority. Tie → closer to screen center inside FOV.
	static constexpr Bone kBones[ ] = {
		{ BipedBoneID::BONE_HEAD, 0 },
		{ BipedBoneID::BONE_NECK, 1 },
		{ BipedBoneID::BONE_SPINE2, 1 },
		{ BipedBoneID::BONE_SPINE1, 1 },
		{ BipedBoneID::BONE_SPINE, 1 },
		{ BipedBoneID::BONE_HIPS, 1 },
		{ BipedBoneID::BONE_LEFTSHOULDER, 2 },
		{ BipedBoneID::BONE_RIGHTSHOULDER, 2 },
		{ BipedBoneID::BONE_LEFTARM, 2 },
		{ BipedBoneID::BONE_RIGHTARM, 2 },
		{ BipedBoneID::BONE_LEFTELBOW, 2 },
		{ BipedBoneID::BONE_RIGHTELBOW, 2 },
		{ BipedBoneID::BONE_LHIP, 3 },
		{ BipedBoneID::BONE_RHIP, 3 },
		{ BipedBoneID::BONE_LKNEE, 3 },
		{ BipedBoneID::BONE_RKNEE, 3 },
		{ BipedBoneID::BONE_LFOOT, 3 },
		{ BipedBoneID::BONE_RFOOT, 3 },
		{ BipedBoneID::BONE_LTOE, 3 },
		{ BipedBoneID::BONE_RTOE, 3 },
	};

	static ShotFn g_orig = nullptr;
	static void* g_target = nullptr;

	static bool pick_target( ubiVector4& out, const ubiVector4& muzzle, float cx, float cy, float r2 ) noexcept
	{
		auto* gm = Scimitar::game_manager::get( );
		auto* lc = gm ? gm->get_local_controller( ) : nullptr;
		if ( !gm || !lc )
			return false;

		auto* list = gm->get_controller_list( );
		int n = gm->get_controller_size( );
		if ( !list || n <= 0 )
			return false;
		n = n > 256 ? 256 : n;

		int best_t = 4;
		float best_d2 = 1e30f;
		bool got = false;

		for ( int i = 0; i < n; ++i ) {
			auto* oc = *reinterpret_cast< Scimitar::Controller** >( reinterpret_cast< uintptr_t >( list ) + i * 8 );
			if ( !oc || oc == lc )
				continue;
			auto* pd = oc->pawn_decrypt( );
			auto* oe = pd ? pd->entity_decrypt( ) : nullptr;
			auto* sk = oe ? oe->get_skeleton( ) : nullptr;
			if ( !sk )
				continue;

			for ( Bone bone : kBones ) {
				const havok::Vec4 b = sk->bone( bone.id );
				if ( !std::isfinite( b.x ) || !std::isfinite( b.y ) || !std::isfinite( b.z ) )
					continue;
				const ubiVector4 w( b.x, b.y, b.z, 1.f );
				if ( visuals::SilentAimVisCheck && !__RaycastData::is_visible( muzzle, w ) )
					continue;
				ubiVector2 sp{};
				if ( !W2S( ubiVector3( b.x, b.y, b.z ), sp ) )
					continue;
				const float dx = sp.x - cx, dy = sp.y - cy, d2 = dx * dx + dy * dy;
				if ( d2 > r2 )
					continue;
				if ( bone.tier < best_t || ( bone.tier == best_t && d2 < best_d2 ) ) {
					best_t = bone.tier;
					best_d2 = d2;
					out = w;
					got = true;
				}
			}
		}
		return got;
	}

	static __int64 __fastcall hook_shot( __int64 w, __m128* src, const __m128i* dir )
	{
		alignas( 16 ) __m128i local{};
		const __m128i* arg = dir;
		if ( visuals::SilentAim && g_orig && src ) {
			alignas( 16 ) float p[ 4 ];
			_mm_store_ps( p, *src );
			const ubiVector4 s( p[ 0 ], p[ 1 ], p[ 2 ], p[ 3 ] );
			const float cam = visuals::EyeFovDegrees > 1.f ? visuals::EyeFovDegrees : 90.f;
			const float rr = havok::screen_fov_cone_radius_px( globals::Width, visuals::SilentAimFovDegrees, cam );
			const float r2 = rr * rr;

			ubiVector4 t{};
			if ( pick_target( t, s, globals::Width * .5f, globals::Height * .5f, r2 ) ) {
				const ubiVector4 d = s - t;
				const float h = sqrtf( d.x * d.x + d.y * d.y + d.z * d.z );
				if ( h > 1e-6f ) {
					const ubiVector4 n( -d.x / h, -d.y / h, -d.z / h, 0.f );
					local = _mm_castps_si128( _mm_setr_ps( n.x, n.y, n.z, n.w ) );
					arg = &local;
				}
			}
		}
		return g_orig( w, src, arg );
	}

} // namespace

void IceBox::silent_aim_tick( )
{
	if ( visuals::SilentAim ) {
		if ( g_orig )
			return;
		g_target = reinterpret_cast< void* >( Memory::ImageBase + 0x1C4E7B0 );
		if ( MH_CreateHook( g_target, reinterpret_cast< LPVOID >( &hook_shot ), reinterpret_cast< LPVOID* >( &g_orig ) ) != MH_OK )
			return;
		if ( MH_EnableHook( g_target ) != MH_OK ) {
			MH_RemoveHook( g_target );
			g_orig = nullptr;
			g_target = nullptr;
		}
	} else if ( g_target ) {
		MH_DisableHook( g_target );
		MH_RemoveHook( g_target );
		g_orig = nullptr;
		g_target = nullptr;
	}
}

void IceBox::silent_aim_prepare_uninject( ) noexcept
{
	if ( !g_target )
		return;
	MH_DisableHook( g_target );
	MH_RemoveHook( g_target );
	g_orig = nullptr;
	g_target = nullptr;
}

#else

void IceBox::silent_aim_tick( )
{
}

void IceBox::silent_aim_prepare_uninject( ) noexcept
{
}

#endif
