#include "../../../IceBox.hpp"
#include "../../../../Core/Engine/Anvil/scimitar.hpp"
#include "../../../../Core/Utils/memory.hpp"

#include <cmath>

namespace {

constexpr float kBlend = 0.55f;
constexpr float kStagger = 0.06f;
constexpr float kTgtFb = -1.5f;
constexpr float kTgtUd = 0.3f;
constexpr float kTol = 0.1f;
constexpr float kLeanBlend = 0.22f;
constexpr float kUdTau = 0.2f;
constexpr float kSprDrop = 0.16f;
constexpr float kSprTau = 0.14f;

float g_udSm = 0.f;
float g_spSm = 0.f;

struct TPState {
	bool prev_on = false;
	bool used_once = false;
	float acc = 0.f;
	float vfb = 0.f, vud = 0.f, vlr = 0.f;
	float ofb = 0.f, oud = 0.f, olr = 0.f;
	float lr_prev = 0.f, lean_acc = 0.f, lean_a = 0.f, lean_b = 0.f;
	uint32_t ld_prev = ~0u;
} g_tp;

inline float clampf( float v, float lo, float hi ) noexcept
{
	return fminf( hi, fmaxf( lo, v ) );
}

inline float esmooth( float cur, float tgt, float dt, float tau ) noexcept
{
	const float a = ( tau > 1e-6f && dt > 0.f ) ? 1.f - expf( -dt / tau ) : 1.f;
	return cur + ( tgt - cur ) * a;
}

inline float lean_lr( uint32_t d ) noexcept
{
	return d == 1u ? -0.3f : ( d == 2u ? 0.3f : 0.f );
}

} // namespace

auto IceBox::third_person( bool enable, float delta_seconds ) -> void
{
	auto* gm = Scimitar::game_manager::get( );
	if ( !Memory::valid_pointer( gm ) )
		return;
	auto* lc = gm->get_local_controller( );
	if ( !Memory::valid_pointer( lc ) )
		return;
	void* cam = lc->get_cam_component( );
	if ( !cam || !Memory::valid_pointer( cam ) )
		return;
	uintptr_t b = Memory::Read< uintptr_t >( Memory::ImageBase + 0x6B8C208 );
	if ( !b )
		return;

	auto* ent = lc->pawn_decrypt( )->entity_decrypt( );
	const uintptr_t lean =
	    Memory::valid_pointer( ent ) ? ent->resolve_component( Scimitar::EDrvLeaning ) : 0;
	const uintptr_t sprint =
	    Memory::valid_pointer( ent ) ? ent->resolve_component( Scimitar::EDrvSprinting ) : 0;

	uint32_t ld = 0;
	if ( lean && Memory::valid_pointer( reinterpret_cast< const void* >( lean ) ) )
		ld = Memory::Read< uint8_t >( lean + 0x70 );

	const float dt =
	    delta_seconds > 1e-9f ? delta_seconds : ( 1.f / 60.f );
	const bool flip = enable != g_tp.prev_on;

	if ( flip ) {
		g_tp.acc = 0.f;
		if ( enable ) {
			g_tp.vfb = Memory::Read< float >( b + 0x24 );
			g_tp.vud = Memory::Read< float >( b + 0x28 );
			g_tp.vlr = Memory::Read< float >( b + 0x20 );
			g_udSm = g_tp.vud;
			g_spSm = 0.f;
			g_tp.used_once = true;
		} else if ( g_tp.used_once ) {
			g_tp.ofb = Memory::Read< float >( b + 0x24 );
			g_tp.oud = Memory::Read< float >( b + 0x28 );
			g_tp.olr = Memory::Read< float >( b + 0x20 );
			g_tp.ld_prev = ~0u;
		}
	}

	if ( flip || ( enable && g_tp.acc < kBlend ) || ( !enable && g_tp.used_once && g_tp.acc < kBlend ) )
		g_tp.acc += dt;
	g_tp.prev_on = enable;

	const float t = fminf( g_tp.acc, kBlend );
	float fb = 0.f, ud = 0.f, lr = 0.f;

	if ( enable ) {
		havok::slope_operator(
		    t, kBlend, true, kStagger, g_tp.vfb, kTgtFb, g_tp.vud, kTgtUd, fb, ud );
		Memory::Write< uint8_t >( reinterpret_cast< uintptr_t >( cam ), 1 );

		const float ldur = fmaxf( kLeanBlend, 1e-6f );
		if ( t + 1e-5f < kBlend ) {
			const float u = fminf( t / kBlend, 1.f );
			lr = g_tp.vlr + ( lean_lr( ld ) - g_tp.vlr ) * havok::ease_out_cubic( u );
			g_tp.ld_prev = ld;
			g_tp.lean_a = lr;
			g_tp.lean_b = lean_lr( ld );
			g_tp.lean_acc = kLeanBlend;
		} else {
			if ( ld != g_tp.ld_prev ) {
				g_tp.lean_a = g_tp.lr_prev;
				g_tp.lean_b = lean_lr( ld );
				g_tp.lean_acc = 0.f;
				g_tp.ld_prev = ld;
			}
			g_tp.lean_acc += dt;
			const float u = fminf( g_tp.lean_acc / ldur, 1.f );
			lr = g_tp.lean_a + ( g_tp.lean_b - g_tp.lean_a ) * havok::ease_out_cubic( u );
			if ( u >= 1.f - 1e-5f )
				lr = g_tp.lean_b;
		}
		g_tp.lr_prev = lr;

		g_udSm = esmooth( g_udSm, ld ? 0.f : ud, dt, kUdTau );
		g_spSm = esmooth(
		    g_spSm,
		    ( ld == 0 && sprint &&
			Memory::valid_pointer( reinterpret_cast< const void* >( sprint ) ) &&
			Memory::Read< uint8_t >( sprint + 0x80 ) == 1 ) ?
			kSprDrop :
			0.f,
		    dt,
		    kSprTau );
		ud = g_udSm - g_spSm;

		fb = clampf( fb, -2.f, 0.f );
		ud = clampf( ud, -0.65f, 0.85f );
		lr = clampf( lr, -0.95f, 0.95f );

		Memory::Write< float >( b + 0x24, fb );
		Memory::Write< float >( b + 0x28, ud );
		Memory::Write< float >( b + 0x20, lr );
		return;
	}

	if ( !g_tp.used_once ) {
		Memory::Write< float >( b + 0x24, 0.f );
		Memory::Write< float >( b + 0x28, 0.f );
		Memory::Write< float >( b + 0x20, 0.f );
		Memory::Write< uint8_t >( reinterpret_cast< uintptr_t >( cam ), 0 );
		g_tp.lr_prev = 0.f;
		g_tp.ld_prev = ~0u;
		return;
	}

	havok::slope_operator( t, kBlend, false, kStagger, g_tp.ofb, 0.f, g_tp.oud, 0.f, fb, ud );
	const bool done = g_tp.acc + 1e-5f >= kBlend;
	if ( done ) {
		fb = 0.f;
		ud = 0.f;
	}
	Memory::Write< uint8_t >( reinterpret_cast< uintptr_t >( cam ),
	    ( done || ( fabsf( fb ) <= kTol && fabsf( ud ) <= kTol ) ) ? uint8_t{ 0 } : uint8_t{ 1 } );

	lr = g_tp.olr * ( 1.f - havok::ease_out_cubic( fminf( t / kBlend, 1.f ) ) );
	if ( done )
		lr = 0.f;
	g_tp.lr_prev = lr;

	Memory::Write< float >( b + 0x24, fb );
	Memory::Write< float >( b + 0x28, ud );
	Memory::Write< float >( b + 0x20, lr );
}

auto IceBox::third_person_reset( ) -> void
{
	g_udSm = 0.f;
	g_spSm = 0.f;
	g_tp = TPState{};

	uintptr_t b = Memory::Read< uintptr_t >( Memory::ImageBase + 0x6B8C208 );
	if ( b && Memory::valid_pointer( reinterpret_cast< const void* >( b ) ) ) {
		Memory::Write< float >( b + 0x20, 0.f );
		Memory::Write< float >( b + 0x24, 0.f );
		Memory::Write< float >( b + 0x28, 0.f );
	}

	auto* gm = Scimitar::game_manager::get( );
	if ( !Memory::valid_pointer( gm ) )
		return;
	auto* lc = gm->get_local_controller( );
	if ( !Memory::valid_pointer( lc ) )
		return;
	void* cam = lc->get_cam_component( );
	if ( cam && Memory::valid_pointer( cam ) )
		Memory::Write< uint8_t >( reinterpret_cast< uintptr_t >( cam ), 0 );
}
