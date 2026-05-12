#include "../../IceBox.hpp"
#include "../../../Core/Engine/Anvil/scimitar.hpp"

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
	auto* lc = Scimitar::game_manager::get( )->get_local_controller( );
	void* cam = lc->get_cam_component( );
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

	static struct {
		bool prev_on, used_once;
		float acc;
		float vfb, vud, vlr, ofb, oud, olr;
		float lr_prev, lean_acc, lean_a, lean_b;
		uint32_t ld_prev;
	} s;

	const float dt =
	    delta_seconds > 1e-9f ? delta_seconds : ( 1.f / 60.f );
	const bool flip = enable != s.prev_on;

	if ( flip ) {
		s.acc = 0.f;
		if ( enable ) {
			s.vfb = Memory::Read< float >( b + 0x24 );
			s.vud = Memory::Read< float >( b + 0x28 );
			s.vlr = Memory::Read< float >( b + 0x20 );
			g_udSm = s.vud;
			g_spSm = 0.f;
			s.used_once = true;
		} else if ( s.used_once ) {
			s.ofb = Memory::Read< float >( b + 0x24 );
			s.oud = Memory::Read< float >( b + 0x28 );
			s.olr = Memory::Read< float >( b + 0x20 );
			s.ld_prev = ~0u;
		}
	}

	if ( flip || ( enable && s.acc < kBlend ) || ( !enable && s.used_once && s.acc < kBlend ) )
		s.acc += dt;
	s.prev_on = enable;

	const float t = fminf( s.acc, kBlend );
	float fb = 0.f, ud = 0.f, lr = 0.f;

	if ( enable ) {
		havok::slope_operator(
		    t, kBlend, true, kStagger, s.vfb, kTgtFb, s.vud, kTgtUd, fb, ud );
		Memory::Write< uint8_t >( reinterpret_cast< uintptr_t >( cam ), 1 );

		const float ldur = fmaxf( kLeanBlend, 1e-6f );
		if ( t + 1e-5f < kBlend ) {
			const float u = fminf( t / kBlend, 1.f );
			lr = s.vlr + ( lean_lr( ld ) - s.vlr ) * havok::ease_out_cubic( u );
			s.ld_prev = ld;
			s.lean_a = lr;
			s.lean_b = lean_lr( ld );
			s.lean_acc = kLeanBlend;
		} else {
			if ( ld != s.ld_prev ) {
				s.lean_a = s.lr_prev;
				s.lean_b = lean_lr( ld );
				s.lean_acc = 0.f;
				s.ld_prev = ld;
			}
			s.lean_acc += dt;
			const float u = fminf( s.lean_acc / ldur, 1.f );
			lr = s.lean_a + ( s.lean_b - s.lean_a ) * havok::ease_out_cubic( u );
			if ( u >= 1.f - 1e-5f )
				lr = s.lean_b;
		}
		s.lr_prev = lr;

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

	if ( !s.used_once ) {
		Memory::Write< float >( b + 0x24, 0.f );
		Memory::Write< float >( b + 0x28, 0.f );
		Memory::Write< float >( b + 0x20, 0.f );
		Memory::Write< uint8_t >( reinterpret_cast< uintptr_t >( cam ), 0 );
		s.lr_prev = 0.f;
		s.ld_prev = ~0u;
		return;
	}

	havok::slope_operator( t, kBlend, false, kStagger, s.ofb, 0.f, s.oud, 0.f, fb, ud );
	const bool done = s.acc + 1e-5f >= kBlend;
	if ( done ) {
		fb = 0.f;
		ud = 0.f;
	}
	Memory::Write< uint8_t >( reinterpret_cast< uintptr_t >( cam ),
	    ( done || ( fabsf( fb ) <= kTol && fabsf( ud ) <= kTol ) ) ? uint8_t{ 0 } : uint8_t{ 1 } );

	lr = s.olr * ( 1.f - havok::ease_out_cubic( fminf( t / kBlend, 1.f ) ) );
	if ( done )
		lr = 0.f;
	s.lr_prev = lr;

	Memory::Write< float >( b + 0x24, fb );
	Memory::Write< float >( b + 0x28, ud );
	Memory::Write< float >( b + 0x20, lr );
}

auto IceBox::third_person_reset( ) -> void
{
	g_udSm = 0.f;
	g_spSm = 0.f;

	uintptr_t b = Memory::Read< uintptr_t >( Memory::ImageBase + 0x6B8C208 );
	if ( !b || !Memory::valid_pointer( reinterpret_cast< const void* >( b ) ) )
		return;

	Memory::Write< float >( b + 0x20, 0.f );
	Memory::Write< float >( b + 0x24, 0.f );
	Memory::Write< float >( b + 0x28, 0.f );
	Memory::Write< uint8_t >(
	    reinterpret_cast< uintptr_t >(
		Scimitar::game_manager::get( )->get_local_controller( )->get_cam_component( ) ),
	    0 );
}
