#pragma once

namespace IcePhysics::Config {

	inline constexpr float kGravityAccel = 32.f;
	inline constexpr float kSurfaceSkinM = 0.018f;
	inline constexpr int kClearanceBisectIters = 14;
	inline constexpr int kCollisionResolutionPasses = 5;
	inline constexpr int kDefaultSubsteps = 4;

	inline constexpr float kRestitutionGround = 0.08f;
	inline constexpr float kRestitutionWall = 0.f;
	inline constexpr float kFrictionGround = 1.65f;

	// Velocity clamps used by current rigid-sphere integrator (tune per body later).
	inline constexpr float kDefaultMaxHorizontalSpeed = 14.f;
	inline constexpr float kDefaultMaxFallSpeed = 42.f;

	// Terrain gradient: finite-difference lateral offset on XY (metres).
	inline constexpr float kTerrainGradientSampleOffsetM = 0.2516f;
	inline constexpr float kTerrainVerticalProbeDepthM = 14.f;
	// Weight on asymmetric tilt rays around −Z (diagram heuristic).
	inline constexpr float kTerrainDiagramRayGain = 22.f;
	inline constexpr float kFrictionSlopeGamma = 5.5f;
	inline constexpr float kTerrainTiltRayHorizBlend = 0.62f;
	inline constexpr float kTerrainTiltRayDownBlend = 0.52f;
	inline constexpr float kTerrainTiltProbeExtraM = 5.2f;

	// Downhill acceleration along estimated gradient (steepness → accel).
	inline constexpr float kSlopeAccelBase = 10.f;
	inline constexpr float kSlopeAccelMax = 52.f;
	inline constexpr float kSlopeSteepClamp = 2.4f;

	inline constexpr int kHorizontalSampleDirections = 8;

	// Horizontal sweep distance beyond body radius when probing walls (sphere stub uses radius_m).
	inline constexpr float kHorizontalCollisionProbePaddingM = 0.48f;
	inline constexpr float kFloorProbeBeyondNeedM = 8.f;

} // namespace IcePhysics::Config
