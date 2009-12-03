
/**********************************************************************
*<
FILE: SHAP.h

DESCRIPTION:	PHY File Format

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include "GFF/GFFCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"
#include "PHY/PHYCommon.h"
#include "PHY/SHAP.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::PHY;
///////////////////////////////////////////////////////////////////

ShortString SHAP::type("SHAP");SHAP::SHAP(GFFStructRef owner) : impl(owner) {
}

unsigned long SHAP::get_shapeType() const {
  return impl->GetField(GFF_MMH_SHAPE_TYPE)->asUInt32();
}

void SHAP::set_shapeType(unsigned long value) {
  impl->GetField(GFF_MMH_SHAPE_TYPE)->assign(value);
}

Text SHAP::get_shapePmatName() const {
  return impl->GetField(GFF_MMH_SHAPE_PMAT_NAME)->asECString();
}

void SHAP::set_shapePmatName(const Text& value) {
  impl->GetField(GFF_MMH_SHAPE_PMAT_NAME)->assign(value);
}

Quaternion SHAP::get_shapeRotation() const {
  return impl->GetField(GFF_MMH_SHAPE_ROTATION)->asQuat();
}

void SHAP::set_shapeRotation(Quaternion value) {
  impl->GetField(GFF_MMH_SHAPE_ROTATION)->assign(value);
}

Vector4f SHAP::get_shapePosition() const {
  return impl->GetField(GFF_MMH_SHAPE_POSITION)->asVector4f();
}

void SHAP::set_shapePosition(Vector4f value) {
  impl->GetField(GFF_MMH_SHAPE_POSITION)->assign(value);
}

unsigned char SHAP::get_shapeCollisionMaskAll() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_ALL)->asUInt8();
}

void SHAP::set_shapeCollisionMaskAll(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_ALL)->assign(value);
}

unsigned char SHAP::get_shapeCollisionMaskNone() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_NONE)->asUInt8();
}

void SHAP::set_shapeCollisionMaskNone(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_NONE)->assign(value);
}

unsigned char SHAP::get_shapeCollisionMaskItems() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_ITEMS)->asUInt8();
}

void SHAP::set_shapeCollisionMaskItems(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_ITEMS)->assign(value);
}

unsigned char SHAP::get_shapeCollisionMaskCreatures() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_CREATURES)->asUInt8();
}

void SHAP::set_shapeCollisionMaskCreatures(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_CREATURES)->assign(value);
}

unsigned char SHAP::get_shapeCollisionMaskPlaceables() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_PLACEABLES)->asUInt8();
}

void SHAP::set_shapeCollisionMaskPlaceables(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_PLACEABLES)->assign(value);
}

unsigned char SHAP::get_shapeCollisionMaskTriggers() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_TRIGGERS)->asUInt8();
}

void SHAP::set_shapeCollisionMaskTriggers(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_TRIGGERS)->assign(value);
}

unsigned char SHAP::get_shapeCollisionMaskStaticGeometry() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_STATIC_GEOMETRY)->asUInt8();
}

void SHAP::set_shapeCollisionMaskStaticGeometry(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_STATIC_GEOMETRY)->assign(value);
}

unsigned char SHAP::get_shapeCollisionMaskNonwalkable() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_NONWALKABLE)->asUInt8();
}

void SHAP::set_shapeCollisionMaskNonwalkable(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_NONWALKABLE)->assign(value);
}

unsigned char SHAP::get_shapeCollisionMaskEffects() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_EFFECTS)->asUInt8();
}

void SHAP::set_shapeCollisionMaskEffects(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_EFFECTS)->assign(value);
}

unsigned char SHAP::get_shapeCollisionMaskWaypoints() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_WAYPOINTS)->asUInt8();
}

void SHAP::set_shapeCollisionMaskWaypoints(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_WAYPOINTS)->assign(value);
}

float SHAP::get_shapeVolume() const {
  return impl->GetField(GFF_MMH_SHAPE_VOLUME)->asFloat32();
}

void SHAP::set_shapeVolume(float value) {
  impl->GetField(GFF_MMH_SHAPE_VOLUME)->assign(value);
}

Text SHAP::get_shapeName() const {
  return impl->GetField(GFF_MMH_SHAPE_NAME)->asECString();
}

void SHAP::set_shapeName(const Text& value) {
  impl->GetField(GFF_MMH_SHAPE_NAME)->assign(value);
}

unsigned char SHAP::get_shapeAllowEmitterSpawn() const {
  return impl->GetField(GFF_MMH_SHAPE_ALLOW_EMITTER_SPAWN)->asUInt8();
}

void SHAP::set_shapeAllowEmitterSpawn(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_ALLOW_EMITTER_SPAWN)->assign(value);
}

unsigned long SHAP::get_collisionGroup() const {
  return impl->GetField(GFF_MMH_COLLISION_GROUP)->asUInt32();
}

void SHAP::set_collisionGroup(unsigned long value) {
  impl->GetField(GFF_MMH_COLLISION_GROUP)->assign(value);
}

unsigned char SHAP::get_shapeFadeable() const {
  return impl->GetField(GFF_MMH_SHAPE_FADEABLE)->asUInt8();
}

void SHAP::set_shapeFadeable(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_FADEABLE)->assign(value);
}

unsigned char SHAP::get_shapeCollisionMaskWater() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_WATER)->asUInt8();
}

void SHAP::set_shapeCollisionMaskWater(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_WATER)->assign(value);
}

unsigned char SHAP::get_shapeCollisionMaskTerrainWall() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_TERRAIN_WALL)->asUInt8();
}

void SHAP::set_shapeCollisionMaskTerrainWall(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_TERRAIN_WALL)->assign(value);
}

unsigned char SHAP::get_shapeCollisionMaskWalkable() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_WALKABLE)->asUInt8();
}

void SHAP::set_shapeCollisionMaskWalkable(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_WALKABLE)->assign(value);
}

GFFFieldRef SHAP::get_shapeTypeStruct() const {
  return impl->GetField(GFF_MMH_SHAPE_TYPE_STRUCT);
}

GFFListRef SHAP::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

