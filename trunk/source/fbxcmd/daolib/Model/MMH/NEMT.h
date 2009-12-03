
/**********************************************************************
*<
FILE: NEMT.h

DESCRIPTION:	MMH File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once

#include "MMH/MMHCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"

namespace DAO {

using namespace GFF;
namespace MMH {
///////////////////////////////////////////////////////////////////

class NEMT
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  NEMT(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_name() const;
  void set_name(const Text& value);

  Text get_materialObject() const;
  void set_materialObject(const Text& value);

  Text get_materialLibrary() const;
  void set_materialLibrary(const Text& value);

  float get_nodeEmitterBirthRate() const;
  void set_nodeEmitterBirthRate(float value);

  float get_nodeEmitterBirthRateRange() const;
  void set_nodeEmitterBirthRateRange(float value);

  float get_nodeEmitterLife() const;
  void set_nodeEmitterLife(float value);

  float get_nodeEmitterLifeRange() const;
  void set_nodeEmitterLifeRange(float value);

  float get_nodeEmitterScaleRange() const;
  void set_nodeEmitterScaleRange(float value);

  float get_nodeEmitterInitialSpeed() const;
  void set_nodeEmitterInitialSpeed(float value);

  float get_nodeEmitterInitialSpeedRange() const;
  void set_nodeEmitterInitialSpeedRange(float value);

  float get_nodeEmitterAcceleration() const;
  void set_nodeEmitterAcceleration(float value);

  float get_nodeEmitterInitialRotationSpeed() const;
  void set_nodeEmitterInitialRotationSpeed(float value);

  float get_nodeEmitterInitialRotationSpeedRange() const;
  void set_nodeEmitterInitialRotationSpeedRange(float value);

  float get_nodeEmitterRotationalAcceleration() const;
  void set_nodeEmitterRotationalAcceleration(float value);

  float get_nodeInvEmitterMovementSpreadUpdateDelay() const;
  void set_nodeInvEmitterMovementSpreadUpdateDelay(float value);

  float get_nodeEmitterSpawnSpreadX() const;
  void set_nodeEmitterSpawnSpreadX(float value);

  float get_nodeEmitterSpawnSpreadY() const;
  void set_nodeEmitterSpawnSpreadY(float value);

  float get_nodeEmitterMovementSpreadX() const;
  void set_nodeEmitterMovementSpreadX(float value);

  float get_nodeEmitterMovementSpreadY() const;
  void set_nodeEmitterMovementSpreadY(float value);

  unsigned char get_nodeEmitterOptionsBirthrateInParticlesPerMeter() const;
  void set_nodeEmitterOptionsBirthrateInParticlesPerMeter(unsigned char value);

  unsigned char get_nodeEmitterOptionsParticlesAffectedByWind() const;
  void set_nodeEmitterOptionsParticlesAffectedByWind(unsigned char value);

  float get_nodeEmitterGravityMultiplier() const;
  void set_nodeEmitterGravityMultiplier(float value);

  unsigned char get_nodeEmitterOptionsParticlesFollowPath() const;
  void set_nodeEmitterOptionsParticlesFollowPath(unsigned char value);

  unsigned char get_nodeEmitterOptionsLinkParticlesTogether() const;
  void set_nodeEmitterOptionsLinkParticlesTogether(unsigned char value);

  unsigned char get_nodeEmitterOptionsUpdateOnlyWhenVisible() const;
  void set_nodeEmitterOptionsUpdateOnlyWhenVisible(unsigned char value);

  unsigned char get_nodeEmitterOptionsEnableParticleCollisions() const;
  void set_nodeEmitterOptionsEnableParticleCollisions(unsigned char value);

  unsigned char get_nodeEmitterOptionsInheritVelocityInsteadOfPosition() const;
  void set_nodeEmitterOptionsInheritVelocityInsteadOfPosition(unsigned char value);

  unsigned long get_nodeEmitterOrientationBehavior() const;
  void set_nodeEmitterOrientationBehavior(unsigned long value);

  unsigned long get_nodeEmitterParticleInheritance() const;
  void set_nodeEmitterParticleInheritance(unsigned long value);

  float get_flipbookFramesPerSecond() const;
  void set_flipbookFramesPerSecond(float value);

  unsigned char get_flipbookRows() const;
  void set_flipbookRows(unsigned char value);

  unsigned char get_flipbookColumns() const;
  void set_flipbookColumns(unsigned char value);

  unsigned char get_flipbookRandomStartFrame() const;
  void set_flipbookRandomStartFrame(unsigned char value);

  Text get_emitterTargetName() const;
  void set_emitterTargetName(const Text& value);

  float get_emitterTargetAttraction() const;
  void set_emitterTargetAttraction(float value);

  float get_emitterTargetRadius() const;
  void set_emitterTargetRadius(float value);

  unsigned char get_emitterSpawnDirectionTracksTarget() const;
  void set_emitterSpawnDirectionTracksTarget(unsigned char value);

  unsigned char get_emitterKillParticleWhenTargetHit() const;
  void set_emitterKillParticleWhenTargetHit(unsigned char value);

  unsigned char get_emitterFlipbookType() const;
  void set_emitterFlipbookType(unsigned char value);

  unsigned char get_nodeEmitterType() const;
  void set_nodeEmitterType(unsigned char value);

  unsigned char get_emitterIsPhysicsEmitter() const;
  void set_emitterIsPhysicsEmitter(unsigned char value);

  unsigned char get_emitterIsPhysicsObjectSpawnEmitter() const;
  void set_emitterIsPhysicsObjectSpawnEmitter(unsigned char value);

  Color4 get_nodeEmitterAgemapColorMultiplier() const;
  void set_nodeEmitterAgemapColorMultiplier(Color4 value);

  float get_nodeEmitterAgemapScalexMultiplier() const;
  void set_nodeEmitterAgemapScalexMultiplier(float value);

  float get_nodeEmitterAgemapScaleyMultiplier() const;
  void set_nodeEmitterAgemapScaleyMultiplier(float value);

  float get_nodeEmitterOptionsBounciness() const;
  void set_nodeEmitterOptionsBounciness(float value);

  float get_nodeEmitterOptionsFriction() const;
  void set_nodeEmitterOptionsFriction(float value);

  Text get_nodeEmitterMeshParticleModelname() const;
  void set_nodeEmitterMeshParticleModelname(const Text& value);

  Vector3f get_nodeEmitterWorldAxisAcceleration() const;
  void set_nodeEmitterWorldAxisAcceleration(Vector3f value);

  unsigned char get_nodeEmitterVertexFormat() const;
  void set_nodeEmitterVertexFormat(unsigned char value);

  unsigned char get_nodeEmitterOptionsObjectSpaceAcceleration() const;
  void set_nodeEmitterOptionsObjectSpaceAcceleration(unsigned char value);

  float get_nodeEmitterInitialRotation() const;
  void set_nodeEmitterInitialRotation(float value);

  float get_nodeEmitterInitialRotationRange() const;
  void set_nodeEmitterInitialRotationRange(float value);

  Vector3f get_nodeEmitterMeshParticleUpAxis() const;
  void set_nodeEmitterMeshParticleUpAxis(Vector3f value);

  Vector3f get_nodeEmitterMeshParticleRollAxis() const;
  void set_nodeEmitterMeshParticleRollAxis(Vector3f value);

  unsigned long get_nodeEmitterUvDistributionSize() const;
  void set_nodeEmitterUvDistributionSize(unsigned long value);

  unsigned char get_nodeEmitterIgnoreDistortion() const;
  void set_nodeEmitterIgnoreDistortion(unsigned char value);

  unsigned char get_nodeEmitterCanParticlesSplat() const;
  void set_nodeEmitterCanParticlesSplat(unsigned char value);

  unsigned char get_nodeEmitterLod() const;
  void set_nodeEmitterLod(unsigned char value);

  Text get_nodeEmitterUserParamName() const;
  void set_nodeEmitterUserParamName(const Text& value);

  unsigned char get_useVariationTint() const;
  void set_useVariationTint(unsigned char value);

  GFFListRef get_children() const;

};

typedef ValuePtr<NEMT> NEMTPtr;
typedef ValueRef<NEMT> NEMTRef;



} //namespace MMH
} //namespace DAO
