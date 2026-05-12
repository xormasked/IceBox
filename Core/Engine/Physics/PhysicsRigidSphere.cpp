#include "PhysicsRigidSphere.hpp"

#include "PhysicsClearance.hpp"
#include "PhysicsConfig.hpp"
#include "PhysicsMath.hpp"
#include "PhysicsTerrain.hpp"
#include "PhysicsWorld.hpp"

namespace IcePhysics::RigidSphere {

	namespace {

		using namespace World;

		inline void resolve_floor(
			ubiVector3& pos,
			ubiVector3& vel,
			bool& grounded,
			float radius_m )
		{
			grounded = false;
			const float need_clear = radius_m + Config::kSurfaceSkinM;
			const float probe_max = need_clear + Config::kFloorProbeBeyondNeedM;
			const float clear =
				Clearance::max_clear_prefix_m( pos, kGravityDown, probe_max );

			if ( clear >= need_clear - 1e-4f )
				return;

			const float penetration = need_clear - clear;
			pos -= kGravityDown * penetration;
			grounded = true;

			const float v_dn = vel.Dot( kGravityDown );
			if ( v_dn > 0.f )
				vel -= kGravityDown * ( v_dn * ( 1.f + Config::kRestitutionGround ) );
		}

		inline void resolve_walls_xy( ubiVector3& pos, ubiVector3& vel, float radius_m )
		{
			const float probe_ext = radius_m + Config::kHorizontalCollisionProbePaddingM;
			const float need = radius_m + Config::kSurfaceSkinM;
			ubiVector3 dirs[ Config::kHorizontalSampleDirections ];
			Math::horizontal_directions_xy( dirs );

			for ( int pass = 0; pass < 2; ++pass ) {
				for ( int i = 0; i < Config::kHorizontalSampleDirections; ++i ) {
					const ubiVector3& dh = dirs[ i ];
					const float clr =
						Clearance::max_clear_prefix_m( pos, dh, probe_ext );
					if ( clr >= need - 1e-4f )
						continue;
					const float pen = need - clr;
					pos -= dh * pen;
					const float vn = vel.Dot( dh );
					if ( vn > 0.f )
						vel -= dh * ( vn * ( 1.f + Config::kRestitutionWall ) );
				}
			}
		}

		inline void apply_ground_friction(
			ubiVector3& vel,
			float dt,
			bool grounded,
			float slope_gradient_mag )
		{
			if ( !grounded || dt < 1e-9f )
				return;
			const float slope_relief =
				1.f / ( 1.f + slope_gradient_mag * Config::kFrictionSlopeGamma );
			ubiVector3 tan = vel - kGravityDown * vel.Dot( kGravityDown );
			const float lh = Math::len3( tan );
			if ( lh < 1e-5f )
				return;
			const float drop = Config::kFrictionGround * Config::kGravityAccel * dt *
				slope_relief * slope_relief;
			const float nd = drop > lh ? lh : drop;
			vel -= tan * ( nd / lh );
		}

		inline void apply_slope_drive_from_gradient(
			ubiVector3& vel,
			float dt,
			float gx,
			float gy,
			float gmag )
		{
			if ( gmag < 2e-4f || dt < 1e-9f )
				return;

			const float nx = gx / gmag;
			const float ny = gy / gmag;
			const float steep =
				gmag / ( 2.f * Config::kTerrainGradientSampleOffsetM + 1e-3f );
			const float steep_clamped =
				steep > Config::kSlopeSteepClamp ? Config::kSlopeSteepClamp : steep;
			float accel = Config::kSlopeAccelBase +
				steep_clamped * ( Config::kSlopeAccelMax - Config::kSlopeAccelBase );
			if ( accel > Config::kSlopeAccelMax )
				accel = Config::kSlopeAccelMax;

			vel.x += nx * accel * dt;
			vel.y += ny * accel * dt;
		}

	} // namespace

	void simulate_step(
		float sub_dt,
		ubiVector3& pos,
		ubiVector3& vel,
		float radius_m )
	{
		vel += World::kGravityDown * ( Config::kGravityAccel * sub_dt );
		Math::clamp_velocity_horizontal_and_fall(
			vel,
			Config::kDefaultMaxHorizontalSpeed,
			Config::kDefaultMaxFallSpeed );

		pos += vel * sub_dt;

		bool grounded_any = false;
		for ( int p = 0; p < Config::kCollisionResolutionPasses; ++p ) {
			bool g = false;
			resolve_floor( pos, vel, g, radius_m );
			grounded_any = grounded_any || g;
			resolve_walls_xy( pos, vel, radius_m );
		}

		float gx = 0.f;
		float gy = 0.f;
		float slope_gmag = 0.f;
		if ( grounded_any )
			slope_gmag =
			Terrain::estimate_surface_gradient_xy( pos, radius_m, gx, gy );

		apply_slope_drive_from_gradient( vel, sub_dt, gx, gy, slope_gmag );
		apply_ground_friction( vel, sub_dt, grounded_any, slope_gmag );
		Math::clamp_velocity_horizontal_and_fall(
			vel,
			Config::kDefaultMaxHorizontalSpeed,
			Config::kDefaultMaxFallSpeed );
	}

} // namespace IcePhysics::RigidSphere
