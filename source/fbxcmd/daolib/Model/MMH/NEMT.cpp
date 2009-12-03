
/**********************************************************************
*<
FILE: NEMT.h

DESCRIPTION:	MMH File Format

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include "GFF/GFFCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"
#include "MMH/MMHCommon.h"
#include "MMH/NEMT.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString NEMT::type("NEMT");NEMT::NEMT(GFFStructRef owner) : impl(owner) {
}

Text NEMT::get_name() const {
  return impl->GetField(GFF_MMH_NAME)->asECString();
}

void NEMT::set_name(const Text& value) {
  impl->GetField(GFF_MMH_NAME)->assign(value);
}

Text NEMT::get_materialObject() const {
  return impl->GetField(GFF_MMH_MATERIAL_OBJECT)->asECString();
}

void NEMT::set_materialObject(const Text& value) {
  impl->GetField(GFF_MMH_MATERIAL_OBJECT)->assign(value);
}

Text NEMT::get_materialLibrary() const {
  return impl->GetField(GFF_MMH_MATERIAL_LIBRARY)->asECString();
}

void NEMT::set_materialLibrary(const Text& value) {
  impl->GetField(GFF_MMH_MATERIAL_LIBRARY)->assign(value);
}

float NEMT::get_nodeEmitterBirthRate() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_BIRTH_RATE)->asFloat32();
}

void NEMT::set_nodeEmitterBirthRate(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_BIRTH_RATE)->assign(value);
}

float NEMT::get_nodeEmitterBirthRateRange() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_BIRTH_RATE_RANGE)->asFloat32();
}

void NEMT::set_nodeEmitterBirthRateRange(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_BIRTH_RATE_RANGE)->assign(value);
}

float NEMT::get_nodeEmitterLife() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_LIFE)->asFloat32();
}

void NEMT::set_nodeEmitterLife(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_LIFE)->assign(value);
}

float NEMT::get_nodeEmitterLifeRange() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_LIFE_RANGE)->asFloat32();
}

void NEMT::set_nodeEmitterLifeRange(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_LIFE_RANGE)->assign(value);
}

float NEMT::get_nodeEmitterScaleRange() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_SCALE_RANGE)->asFloat32();
}

void NEMT::set_nodeEmitterScaleRange(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_SCALE_RANGE)->assign(value);
}

float NEMT::get_nodeEmitterInitialSpeed() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_INITIAL_SPEED)->asFloat32();
}

void NEMT::set_nodeEmitterInitialSpeed(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_INITIAL_SPEED)->assign(value);
}

float NEMT::get_nodeEmitterInitialSpeedRange() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_INITIAL_SPEED_RANGE)->asFloat32();
}

void NEMT::set_nodeEmitterInitialSpeedRange(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_INITIAL_SPEED_RANGE)->assign(value);
}

float NEMT::get_nodeEmitterAcceleration() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_ACCELERATION)->asFloat32();
}

void NEMT::set_nodeEmitterAcceleration(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_ACCELERATION)->assign(value);
}

float NEMT::get_nodeEmitterInitialRotationSpeed() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_INITIAL_ROTATION_SPEED)->asFloat32();
}

void NEMT::set_nodeEmitterInitialRotationSpeed(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_INITIAL_ROTATION_SPEED)->assign(value);
}

float NEMT::get_nodeEmitterInitialRotationSpeedRange() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_INITIAL_ROTATION_SPEED_RANGE)->asFloat32();
}

void NEMT::set_nodeEmitterInitialRotationSpeedRange(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_INITIAL_ROTATION_SPEED_RANGE)->assign(value);
}

float NEMT::get_nodeEmitterRotationalAcceleration() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_ROTATIONAL_ACCELERATION)->asFloat32();
}

void NEMT::set_nodeEmitterRotationalAcceleration(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_ROTATIONAL_ACCELERATION)->assign(value);
}

float NEMT::get_nodeInvEmitterMovementSpreadUpdateDelay() const {
  return impl->GetField(GFF_MMH_NODE_INV_EMITTER_MOVEMENT_SPREAD_UPDATE_DELAY)->asFloat32();
}

void NEMT::set_nodeInvEmitterMovementSpreadUpdateDelay(float value) {
  impl->GetField(GFF_MMH_NODE_INV_EMITTER_MOVEMENT_SPREAD_UPDATE_DELAY)->assign(value);
}

float NEMT::get_nodeEmitterSpawnSpreadX() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_SPAWN_SPREAD_X)->asFloat32();
}

void NEMT::set_nodeEmitterSpawnSpreadX(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_SPAWN_SPREAD_X)->assign(value);
}

float NEMT::get_nodeEmitterSpawnSpreadY() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_SPAWN_SPREAD_Y)->asFloat32();
}

void NEMT::set_nodeEmitterSpawnSpreadY(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_SPAWN_SPREAD_Y)->assign(value);
}

float NEMT::get_nodeEmitterMovementSpreadX() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_MOVEMENT_SPREAD_X)->asFloat32();
}

void NEMT::set_nodeEmitterMovementSpreadX(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_MOVEMENT_SPREAD_X)->assign(value);
}

float NEMT::get_nodeEmitterMovementSpreadY() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_MOVEMENT_SPREAD_Y)->asFloat32();
}

void NEMT::set_nodeEmitterMovementSpreadY(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_MOVEMENT_SPREAD_Y)->assign(value);
}

unsigned char NEMT::get_nodeEmitterOptionsBirthrateInParticlesPerMeter() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_BIRTHRATE_IN_PARTICLES_PER_METER)->asUInt8();
}

void NEMT::set_nodeEmitterOptionsBirthrateInParticlesPerMeter(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_BIRTHRATE_IN_PARTICLES_PER_METER)->assign(value);
}

unsigned char NEMT::get_nodeEmitterOptionsParticlesAffectedByWind() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_PARTICLES_AFFECTED_BY_WIND)->asUInt8();
}

void NEMT::set_nodeEmitterOptionsParticlesAffectedByWind(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_PARTICLES_AFFECTED_BY_WIND)->assign(value);
}

float NEMT::get_nodeEmitterGravityMultiplier() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_GRAVITY_MULTIPLIER)->asFloat32();
}

void NEMT::set_nodeEmitterGravityMultiplier(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_GRAVITY_MULTIPLIER)->assign(value);
}

unsigned char NEMT::get_nodeEmitterOptionsParticlesFollowPath() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_PARTICLES_FOLLOW_PATH)->asUInt8();
}

void NEMT::set_nodeEmitterOptionsParticlesFollowPath(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_PARTICLES_FOLLOW_PATH)->assign(value);
}

unsigned char NEMT::get_nodeEmitterOptionsLinkParticlesTogether() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_LINK_PARTICLES_TOGETHER)->asUInt8();
}

void NEMT::set_nodeEmitterOptionsLinkParticlesTogether(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_LINK_PARTICLES_TOGETHER)->assign(value);
}

unsigned char NEMT::get_nodeEmitterOptionsUpdateOnlyWhenVisible() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_UPDATE_ONLY_WHEN_VISIBLE)->asUInt8();
}

void NEMT::set_nodeEmitterOptionsUpdateOnlyWhenVisible(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_UPDATE_ONLY_WHEN_VISIBLE)->assign(value);
}

unsigned char NEMT::get_nodeEmitterOptionsEnableParticleCollisions() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_ENABLE_PARTICLE_COLLISIONS)->asUInt8();
}

void NEMT::set_nodeEmitterOptionsEnableParticleCollisions(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_ENABLE_PARTICLE_COLLISIONS)->assign(value);
}

unsigned char NEMT::get_nodeEmitterOptionsInheritVelocityInsteadOfPosition() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_INHERIT_VELOCITY_INSTEAD_OF_POSITION)->asUInt8();
}

void NEMT::set_nodeEmitterOptionsInheritVelocityInsteadOfPosition(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_INHERIT_VELOCITY_INSTEAD_OF_POSITION)->assign(value);
}

unsigned long NEMT::get_nodeEmitterOrientationBehavior() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_ORIENTATION_BEHAVIOR)->asUInt32();
}

void NEMT::set_nodeEmitterOrientationBehavior(unsigned long value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_ORIENTATION_BEHAVIOR)->assign(value);
}

unsigned long NEMT::get_nodeEmitterParticleInheritance() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_PARTICLE_INHERITANCE)->asUInt32();
}

void NEMT::set_nodeEmitterParticleInheritance(unsigned long value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_PARTICLE_INHERITANCE)->assign(value);
}

float NEMT::get_flipbookFramesPerSecond() const {
  return impl->GetField(GFF_MMH_FLIPBOOK_FRAMES_PER_SECOND)->asFloat32();
}

void NEMT::set_flipbookFramesPerSecond(float value) {
  impl->GetField(GFF_MMH_FLIPBOOK_FRAMES_PER_SECOND)->assign(value);
}

unsigned char NEMT::get_flipbookRows() const {
  return impl->GetField(GFF_MMH_FLIPBOOK_ROWS)->asUInt8();
}

void NEMT::set_flipbookRows(unsigned char value) {
  impl->GetField(GFF_MMH_FLIPBOOK_ROWS)->assign(value);
}

unsigned char NEMT::get_flipbookColumns() const {
  return impl->GetField(GFF_MMH_FLIPBOOK_COLUMNS)->asUInt8();
}

void NEMT::set_flipbookColumns(unsigned char value) {
  impl->GetField(GFF_MMH_FLIPBOOK_COLUMNS)->assign(value);
}

unsigned char NEMT::get_flipbookRandomStartFrame() const {
  return impl->GetField(GFF_MMH_FLIPBOOK_RANDOM_START_FRAME)->asUInt8();
}

void NEMT::set_flipbookRandomStartFrame(unsigned char value) {
  impl->GetField(GFF_MMH_FLIPBOOK_RANDOM_START_FRAME)->assign(value);
}

Text NEMT::get_emitterTargetName() const {
  return impl->GetField(GFF_MMH_EMITTER_TARGET_NAME)->asECString();
}

void NEMT::set_emitterTargetName(const Text& value) {
  impl->GetField(GFF_MMH_EMITTER_TARGET_NAME)->assign(value);
}

float NEMT::get_emitterTargetAttraction() const {
  return impl->GetField(GFF_MMH_EMITTER_TARGET_ATTRACTION)->asFloat32();
}

void NEMT::set_emitterTargetAttraction(float value) {
  impl->GetField(GFF_MMH_EMITTER_TARGET_ATTRACTION)->assign(value);
}

float NEMT::get_emitterTargetRadius() const {
  return impl->GetField(GFF_MMH_EMITTER_TARGET_RADIUS)->asFloat32();
}

void NEMT::set_emitterTargetRadius(float value) {
  impl->GetField(GFF_MMH_EMITTER_TARGET_RADIUS)->assign(value);
}

unsigned char NEMT::get_emitterSpawnDirectionTracksTarget() const {
  return impl->GetField(GFF_MMH_EMITTER_SPAWN_DIRECTION_TRACKS_TARGET)->asUInt8();
}

void NEMT::set_emitterSpawnDirectionTracksTarget(unsigned char value) {
  impl->GetField(GFF_MMH_EMITTER_SPAWN_DIRECTION_TRACKS_TARGET)->assign(value);
}

unsigned char NEMT::get_emitterKillParticleWhenTargetHit() const {
  return impl->GetField(GFF_MMH_EMITTER_KILL_PARTICLE_WHEN_TARGET_HIT)->asUInt8();
}

void NEMT::set_emitterKillParticleWhenTargetHit(unsigned char value) {
  impl->GetField(GFF_MMH_EMITTER_KILL_PARTICLE_WHEN_TARGET_HIT)->assign(value);
}

unsigned char NEMT::get_emitterFlipbookType() const {
  return impl->GetField(GFF_MMH_EMITTER_FLIPBOOK_TYPE)->asUInt8();
}

void NEMT::set_emitterFlipbookType(unsigned char value) {
  impl->GetField(GFF_MMH_EMITTER_FLIPBOOK_TYPE)->assign(value);
}

unsigned char NEMT::get_nodeEmitterType() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_TYPE)->asUInt8();
}

void NEMT::set_nodeEmitterType(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_TYPE)->assign(value);
}

unsigned char NEMT::get_emitterIsPhysicsEmitter() const {
  return impl->GetField(GFF_MMH_EMITTER_IS_PHYSICS_EMITTER)->asUInt8();
}

void NEMT::set_emitterIsPhysicsEmitter(unsigned char value) {
  impl->GetField(GFF_MMH_EMITTER_IS_PHYSICS_EMITTER)->assign(value);
}

unsigned char NEMT::get_emitterIsPhysicsObjectSpawnEmitter() const {
  return impl->GetField(GFF_MMH_EMITTER_IS_PHYSICS_OBJECT_SPAWN_EMITTER)->asUInt8();
}

void NEMT::set_emitterIsPhysicsObjectSpawnEmitter(unsigned char value) {
  impl->GetField(GFF_MMH_EMITTER_IS_PHYSICS_OBJECT_SPAWN_EMITTER)->assign(value);
}

Color4 NEMT::get_nodeEmitterAgemapColorMultiplier() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_AGEMAP_COLOR_MULTIPLIER)->asColor4();
}

void NEMT::set_nodeEmitterAgemapColorMultiplier(Color4 value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_AGEMAP_COLOR_MULTIPLIER)->assign(value);
}

float NEMT::get_nodeEmitterAgemapScalexMultiplier() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_AGEMAP_SCALEX_MULTIPLIER)->asFloat32();
}

void NEMT::set_nodeEmitterAgemapScalexMultiplier(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_AGEMAP_SCALEX_MULTIPLIER)->assign(value);
}

float NEMT::get_nodeEmitterAgemapScaleyMultiplier() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_AGEMAP_SCALEY_MULTIPLIER)->asFloat32();
}

void NEMT::set_nodeEmitterAgemapScaleyMultiplier(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_AGEMAP_SCALEY_MULTIPLIER)->assign(value);
}

float NEMT::get_nodeEmitterOptionsBounciness() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_BOUNCINESS)->asFloat32();
}

void NEMT::set_nodeEmitterOptionsBounciness(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_BOUNCINESS)->assign(value);
}

float NEMT::get_nodeEmitterOptionsFriction() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_FRICTION)->asFloat32();
}

void NEMT::set_nodeEmitterOptionsFriction(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_FRICTION)->assign(value);
}

Text NEMT::get_nodeEmitterMeshParticleModelname() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_MESH_PARTICLE_MODELNAME)->asECString();
}

void NEMT::set_nodeEmitterMeshParticleModelname(const Text& value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_MESH_PARTICLE_MODELNAME)->assign(value);
}

Vector3f NEMT::get_nodeEmitterWorldAxisAcceleration() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_WORLD_AXIS_ACCELERATION)->asVector3f();
}

void NEMT::set_nodeEmitterWorldAxisAcceleration(Vector3f value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_WORLD_AXIS_ACCELERATION)->assign(value);
}

unsigned char NEMT::get_nodeEmitterVertexFormat() const {
  return impl->GetField(GFF_MMN_NODE_EMITTER_VERTEX_FORMAT)->asUInt8();
}

void NEMT::set_nodeEmitterVertexFormat(unsigned char value) {
  impl->GetField(GFF_MMN_NODE_EMITTER_VERTEX_FORMAT)->assign(value);
}

unsigned char NEMT::get_nodeEmitterOptionsObjectSpaceAcceleration() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_OBJECT_SPACE_ACCELERATION)->asUInt8();
}

void NEMT::set_nodeEmitterOptionsObjectSpaceAcceleration(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_OPTIONS_OBJECT_SPACE_ACCELERATION)->assign(value);
}

float NEMT::get_nodeEmitterInitialRotation() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_INITIAL_ROTATION)->asFloat32();
}

void NEMT::set_nodeEmitterInitialRotation(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_INITIAL_ROTATION)->assign(value);
}

float NEMT::get_nodeEmitterInitialRotationRange() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_INITIAL_ROTATION_RANGE)->asFloat32();
}

void NEMT::set_nodeEmitterInitialRotationRange(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_INITIAL_ROTATION_RANGE)->assign(value);
}

Vector3f NEMT::get_nodeEmitterMeshParticleUpAxis() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_MESH_PARTICLE_UP_AXIS)->asVector3f();
}

void NEMT::set_nodeEmitterMeshParticleUpAxis(Vector3f value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_MESH_PARTICLE_UP_AXIS)->assign(value);
}

Vector3f NEMT::get_nodeEmitterMeshParticleRollAxis() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_MESH_PARTICLE_ROLL_AXIS)->asVector3f();
}

void NEMT::set_nodeEmitterMeshParticleRollAxis(Vector3f value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_MESH_PARTICLE_ROLL_AXIS)->assign(value);
}

unsigned long NEMT::get_nodeEmitterUvDistributionSize() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_UV_DISTRIBUTION_SIZE)->asUInt32();
}

void NEMT::set_nodeEmitterUvDistributionSize(unsigned long value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_UV_DISTRIBUTION_SIZE)->assign(value);
}

unsigned char NEMT::get_nodeEmitterIgnoreDistortion() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_IGNORE_DISTORTION)->asUInt8();
}

void NEMT::set_nodeEmitterIgnoreDistortion(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_IGNORE_DISTORTION)->assign(value);
}

unsigned char NEMT::get_nodeEmitterCanParticlesSplat() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_CAN_PARTICLES_SPLAT)->asUInt8();
}

void NEMT::set_nodeEmitterCanParticlesSplat(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_CAN_PARTICLES_SPLAT)->assign(value);
}

unsigned char NEMT::get_nodeEmitterLod() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_LOD)->asUInt8();
}

void NEMT::set_nodeEmitterLod(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_LOD)->assign(value);
}

Text NEMT::get_nodeEmitterUserParamName() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_USER_PARAM_NAME)->asECString();
}

void NEMT::set_nodeEmitterUserParamName(const Text& value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_USER_PARAM_NAME)->assign(value);
}

unsigned char NEMT::get_useVariationTint() const {
  return impl->GetField(GFF_MMH_USE_VARIATION_TINT)->asUInt8();
}

void NEMT::set_useVariationTint(unsigned char value) {
  impl->GetField(GFF_MMH_USE_VARIATION_TINT)->assign(value);
}

GFFListRef NEMT::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

