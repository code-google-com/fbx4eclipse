
/**********************************************************************
*<
FILE: NCLT.h

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

class NCLT
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  NCLT(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_name() const;
  void set_name(const Text& value);

  Text get_materialObject() const;
  void set_materialObject(const Text& value);

  Text get_materialLibrary() const;
  void set_materialLibrary(const Text& value);

  Text get_id() const;
  void set_id(const Text& value);

  unsigned char get_shapeCollisionMaskAll() const;
  void set_shapeCollisionMaskAll(unsigned char value);

  unsigned char get_shapeCollisionMaskNone() const;
  void set_shapeCollisionMaskNone(unsigned char value);

  unsigned char get_shapeCollisionMaskItems() const;
  void set_shapeCollisionMaskItems(unsigned char value);

  unsigned char get_shapeCollisionMaskCreatures() const;
  void set_shapeCollisionMaskCreatures(unsigned char value);

  unsigned char get_shapeCollisionMaskPlaceables() const;
  void set_shapeCollisionMaskPlaceables(unsigned char value);

  unsigned char get_shapeCollisionMaskTriggers() const;
  void set_shapeCollisionMaskTriggers(unsigned char value);

  unsigned char get_shapeCollisionMaskStaticGeometry() const;
  void set_shapeCollisionMaskStaticGeometry(unsigned char value);

  unsigned char get_shapeCollisionMaskNonwalkable() const;
  void set_shapeCollisionMaskNonwalkable(unsigned char value);

  unsigned char get_meshCastRuntimeShadow() const;
  void set_meshCastRuntimeShadow(unsigned char value);

  unsigned char get_shapeCollisionMaskEffects() const;
  void set_shapeCollisionMaskEffects(unsigned char value);

  unsigned char get_shapeCollisionMaskWaypoints() const;
  void set_shapeCollisionMaskWaypoints(unsigned char value);

  float get_clothThickness() const;
  void set_clothThickness(float value);

  float get_clothDensity() const;
  void set_clothDensity(float value);

  float get_clothBendingStiffness() const;
  void set_clothBendingStiffness(float value);

  float get_clothStretchingStiffness() const;
  void set_clothStretchingStiffness(float value);

  float get_clothDampingCoefficient() const;
  void set_clothDampingCoefficient(float value);

  float get_clothFriction() const;
  void set_clothFriction(float value);

  float get_clothPressure() const;
  void set_clothPressure(float value);

  float get_clothTearFactor() const;
  void set_clothTearFactor(float value);

  float get_clothCollisionResponseCoefficient() const;
  void set_clothCollisionResponseCoefficient(float value);

  float get_clothAttachmentResponseCoefficient() const;
  void set_clothAttachmentResponseCoefficient(float value);

  float get_clothAttachmentTearFactor() const;
  void set_clothAttachmentTearFactor(float value);

  unsigned long get_clothSolverIterations() const;
  void set_clothSolverIterations(unsigned long value);

  Vector3f get_clothExternalAcceleration() const;
  void set_clothExternalAcceleration(Vector3f value);

  float get_clothWakeUpCounter() const;
  void set_clothWakeUpCounter(float value);

  float get_clothSleepLinearVelocity() const;
  void set_clothSleepLinearVelocity(float value);

  unsigned char get_clothFlagPressure() const;
  void set_clothFlagPressure(unsigned char value);

  unsigned char get_clothFlagStatic() const;
  void set_clothFlagStatic(unsigned char value);

  unsigned char get_clothFlagDisableCollision() const;
  void set_clothFlagDisableCollision(unsigned char value);

  unsigned char get_clothFlagSelfcollision() const;
  void set_clothFlagSelfcollision(unsigned char value);

  unsigned char get_clothFlagVisualization() const;
  void set_clothFlagVisualization(unsigned char value);

  unsigned char get_clothFlagGravity() const;
  void set_clothFlagGravity(unsigned char value);

  unsigned char get_clothFlagBending() const;
  void set_clothFlagBending(unsigned char value);

  unsigned char get_clothFlagBendingOrtho() const;
  void set_clothFlagBendingOrtho(unsigned char value);

  unsigned char get_clothFlagDamping() const;
  void set_clothFlagDamping(unsigned char value);

  unsigned char get_clothFlagCollisionTwoway() const;
  void set_clothFlagCollisionTwoway(unsigned char value);

  unsigned char get_clothFlagTriangleCollision() const;
  void set_clothFlagTriangleCollision(unsigned char value);

  unsigned char get_clothFlagTearable() const;
  void set_clothFlagTearable(unsigned char value);

  unsigned char get_clothFlagHardware() const;
  void set_clothFlagHardware(unsigned char value);

  unsigned char get_clothFlagComdamping() const;
  void set_clothFlagComdamping(unsigned char value);

  GFFListRef get_clothCookedDataStream() const;

  GFFStructRef get_clothMeshGroupStruct() const;

  unsigned char get_shapeCollisionMaskWater() const;
  void set_shapeCollisionMaskWater(unsigned char value);

  unsigned char get_shapeCollisionMaskTerrainWall() const;
  void set_shapeCollisionMaskTerrainWall(unsigned char value);

  unsigned char get_shapeCollisionMaskWalkable() const;
  void set_shapeCollisionMaskWalkable(unsigned char value);

  GFFListRef get_children() const;

};

typedef ValuePtr<NCLT> NCLTPtr;
typedef ValueRef<NCLT> NCLTRef;



} //namespace MMH
} //namespace DAO
