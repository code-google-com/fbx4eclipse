
/**********************************************************************
*<
FILE: SHAP.h

DESCRIPTION:	PHY File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once

#include "PHY/PHYCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"

namespace DAO {

using namespace GFF;
namespace PHY {
///////////////////////////////////////////////////////////////////

class SHAP
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  SHAP(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  unsigned long get_shapeType() const;
  void set_shapeType(unsigned long value);

  Text get_shapePmatName() const;
  void set_shapePmatName(const Text& value);

  Quaternion get_shapeRotation() const;
  void set_shapeRotation(Quaternion value);

  Vector4f get_shapePosition() const;
  void set_shapePosition(Vector4f value);

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

  unsigned char get_shapeCollisionMaskEffects() const;
  void set_shapeCollisionMaskEffects(unsigned char value);

  unsigned char get_shapeCollisionMaskWaypoints() const;
  void set_shapeCollisionMaskWaypoints(unsigned char value);

  float get_shapeVolume() const;
  void set_shapeVolume(float value);

  Text get_shapeName() const;
  void set_shapeName(const Text& value);

  unsigned char get_shapeAllowEmitterSpawn() const;
  void set_shapeAllowEmitterSpawn(unsigned char value);

  unsigned long get_collisionGroup() const;
  void set_collisionGroup(unsigned long value);

  unsigned char get_shapeFadeable() const;
  void set_shapeFadeable(unsigned char value);

  unsigned char get_shapeCollisionMaskWater() const;
  void set_shapeCollisionMaskWater(unsigned char value);

  unsigned char get_shapeCollisionMaskTerrainWall() const;
  void set_shapeCollisionMaskTerrainWall(unsigned char value);

  unsigned char get_shapeCollisionMaskWalkable() const;
  void set_shapeCollisionMaskWalkable(unsigned char value);

  GFFFieldRef get_shapeTypeStruct() const;

  GFFListRef get_children() const;

};

typedef ValuePtr<SHAP> SHAPPtr;
typedef ValueRef<SHAP> SHAPRef;



} //namespace PHY
} //namespace DAO
