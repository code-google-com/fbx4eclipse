
/**********************************************************************
*<
FILE: NCLT.h

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
#include "MMH/NCLT.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString NCLT::type("NCLT");NCLT::NCLT(GFFStructRef owner) : impl(owner) {
}

Text NCLT::get_name() const {
  return impl->GetField(GFF_MMH_NAME)->asECString();
}

void NCLT::set_name(const Text& value) {
  impl->GetField(GFF_MMH_NAME)->assign(value);
}

Text NCLT::get_materialObject() const {
  return impl->GetField(GFF_MMH_MATERIAL_OBJECT)->asECString();
}

void NCLT::set_materialObject(const Text& value) {
  impl->GetField(GFF_MMH_MATERIAL_OBJECT)->assign(value);
}

Text NCLT::get_materialLibrary() const {
  return impl->GetField(GFF_MMH_MATERIAL_LIBRARY)->asECString();
}

void NCLT::set_materialLibrary(const Text& value) {
  impl->GetField(GFF_MMH_MATERIAL_LIBRARY)->assign(value);
}

Text NCLT::get_id() const {
  return impl->GetField(GFF_MMH_ID)->asECString();
}

void NCLT::set_id(const Text& value) {
  impl->GetField(GFF_MMH_ID)->assign(value);
}

unsigned char NCLT::get_shapeCollisionMaskAll() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_ALL)->asUInt8();
}

void NCLT::set_shapeCollisionMaskAll(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_ALL)->assign(value);
}

unsigned char NCLT::get_shapeCollisionMaskNone() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_NONE)->asUInt8();
}

void NCLT::set_shapeCollisionMaskNone(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_NONE)->assign(value);
}

unsigned char NCLT::get_shapeCollisionMaskItems() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_ITEMS)->asUInt8();
}

void NCLT::set_shapeCollisionMaskItems(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_ITEMS)->assign(value);
}

unsigned char NCLT::get_shapeCollisionMaskCreatures() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_CREATURES)->asUInt8();
}

void NCLT::set_shapeCollisionMaskCreatures(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_CREATURES)->assign(value);
}

unsigned char NCLT::get_shapeCollisionMaskPlaceables() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_PLACEABLES)->asUInt8();
}

void NCLT::set_shapeCollisionMaskPlaceables(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_PLACEABLES)->assign(value);
}

unsigned char NCLT::get_shapeCollisionMaskTriggers() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_TRIGGERS)->asUInt8();
}

void NCLT::set_shapeCollisionMaskTriggers(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_TRIGGERS)->assign(value);
}

unsigned char NCLT::get_shapeCollisionMaskStaticGeometry() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_STATIC_GEOMETRY)->asUInt8();
}

void NCLT::set_shapeCollisionMaskStaticGeometry(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_STATIC_GEOMETRY)->assign(value);
}

unsigned char NCLT::get_shapeCollisionMaskNonwalkable() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_NONWALKABLE)->asUInt8();
}

void NCLT::set_shapeCollisionMaskNonwalkable(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_NONWALKABLE)->assign(value);
}

unsigned char NCLT::get_meshCastRuntimeShadow() const {
  return impl->GetField(GFF_MMH_MESH_CAST_RUNTIME_SHADOW)->asUInt8();
}

void NCLT::set_meshCastRuntimeShadow(unsigned char value) {
  impl->GetField(GFF_MMH_MESH_CAST_RUNTIME_SHADOW)->assign(value);
}

unsigned char NCLT::get_shapeCollisionMaskEffects() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_EFFECTS)->asUInt8();
}

void NCLT::set_shapeCollisionMaskEffects(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_EFFECTS)->assign(value);
}

unsigned char NCLT::get_shapeCollisionMaskWaypoints() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_WAYPOINTS)->asUInt8();
}

void NCLT::set_shapeCollisionMaskWaypoints(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_WAYPOINTS)->assign(value);
}

float NCLT::get_clothThickness() const {
  return impl->GetField(GFF_MMH_CLOTH_THICKNESS)->asFloat32();
}

void NCLT::set_clothThickness(float value) {
  impl->GetField(GFF_MMH_CLOTH_THICKNESS)->assign(value);
}

float NCLT::get_clothDensity() const {
  return impl->GetField(GFF_MMH_CLOTH_DENSITY)->asFloat32();
}

void NCLT::set_clothDensity(float value) {
  impl->GetField(GFF_MMH_CLOTH_DENSITY)->assign(value);
}

float NCLT::get_clothBendingStiffness() const {
  return impl->GetField(GFF_MMH_CLOTH_BENDING_STIFFNESS)->asFloat32();
}

void NCLT::set_clothBendingStiffness(float value) {
  impl->GetField(GFF_MMH_CLOTH_BENDING_STIFFNESS)->assign(value);
}

float NCLT::get_clothStretchingStiffness() const {
  return impl->GetField(GFF_MMH_CLOTH_STRETCHING_STIFFNESS)->asFloat32();
}

void NCLT::set_clothStretchingStiffness(float value) {
  impl->GetField(GFF_MMH_CLOTH_STRETCHING_STIFFNESS)->assign(value);
}

float NCLT::get_clothDampingCoefficient() const {
  return impl->GetField(GFF_MMH_CLOTH_DAMPING_COEFFICIENT)->asFloat32();
}

void NCLT::set_clothDampingCoefficient(float value) {
  impl->GetField(GFF_MMH_CLOTH_DAMPING_COEFFICIENT)->assign(value);
}

float NCLT::get_clothFriction() const {
  return impl->GetField(GFF_MMH_CLOTH_FRICTION)->asFloat32();
}

void NCLT::set_clothFriction(float value) {
  impl->GetField(GFF_MMH_CLOTH_FRICTION)->assign(value);
}

float NCLT::get_clothPressure() const {
  return impl->GetField(GFF_MMH_CLOTH_PRESSURE)->asFloat32();
}

void NCLT::set_clothPressure(float value) {
  impl->GetField(GFF_MMH_CLOTH_PRESSURE)->assign(value);
}

float NCLT::get_clothTearFactor() const {
  return impl->GetField(GFF_MMH_CLOTH_TEAR_FACTOR)->asFloat32();
}

void NCLT::set_clothTearFactor(float value) {
  impl->GetField(GFF_MMH_CLOTH_TEAR_FACTOR)->assign(value);
}

float NCLT::get_clothCollisionResponseCoefficient() const {
  return impl->GetField(GFF_MMH_CLOTH_COLLISION_RESPONSE_COEFFICIENT)->asFloat32();
}

void NCLT::set_clothCollisionResponseCoefficient(float value) {
  impl->GetField(GFF_MMH_CLOTH_COLLISION_RESPONSE_COEFFICIENT)->assign(value);
}

float NCLT::get_clothAttachmentResponseCoefficient() const {
  return impl->GetField(GFF_MMH_CLOTH_ATTACHMENT_RESPONSE_COEFFICIENT)->asFloat32();
}

void NCLT::set_clothAttachmentResponseCoefficient(float value) {
  impl->GetField(GFF_MMH_CLOTH_ATTACHMENT_RESPONSE_COEFFICIENT)->assign(value);
}

float NCLT::get_clothAttachmentTearFactor() const {
  return impl->GetField(GFF_MMH_CLOTH_ATTACHMENT_TEAR_FACTOR)->asFloat32();
}

void NCLT::set_clothAttachmentTearFactor(float value) {
  impl->GetField(GFF_MMH_CLOTH_ATTACHMENT_TEAR_FACTOR)->assign(value);
}

unsigned long NCLT::get_clothSolverIterations() const {
  return impl->GetField(GFF_MMH_CLOTH_SOLVER_ITERATIONS)->asUInt32();
}

void NCLT::set_clothSolverIterations(unsigned long value) {
  impl->GetField(GFF_MMH_CLOTH_SOLVER_ITERATIONS)->assign(value);
}

Vector3f NCLT::get_clothExternalAcceleration() const {
  return impl->GetField(GFF_MMH_CLOTH_EXTERNAL_ACCELERATION)->asVector3f();
}

void NCLT::set_clothExternalAcceleration(Vector3f value) {
  impl->GetField(GFF_MMH_CLOTH_EXTERNAL_ACCELERATION)->assign(value);
}

float NCLT::get_clothWakeUpCounter() const {
  return impl->GetField(GFF_MMH_CLOTH_WAKE_UP_COUNTER)->asFloat32();
}

void NCLT::set_clothWakeUpCounter(float value) {
  impl->GetField(GFF_MMH_CLOTH_WAKE_UP_COUNTER)->assign(value);
}

float NCLT::get_clothSleepLinearVelocity() const {
  return impl->GetField(GFF_MMH_CLOTH_SLEEP_LINEAR_VELOCITY)->asFloat32();
}

void NCLT::set_clothSleepLinearVelocity(float value) {
  impl->GetField(GFF_MMH_CLOTH_SLEEP_LINEAR_VELOCITY)->assign(value);
}

unsigned char NCLT::get_clothFlagPressure() const {
  return impl->GetField(GFF_MMH_CLOTH_FLAG_PRESSURE)->asUInt8();
}

void NCLT::set_clothFlagPressure(unsigned char value) {
  impl->GetField(GFF_MMH_CLOTH_FLAG_PRESSURE)->assign(value);
}

unsigned char NCLT::get_clothFlagStatic() const {
  return impl->GetField(GFF_MMH_CLOTH_FLAG_STATIC)->asUInt8();
}

void NCLT::set_clothFlagStatic(unsigned char value) {
  impl->GetField(GFF_MMH_CLOTH_FLAG_STATIC)->assign(value);
}

unsigned char NCLT::get_clothFlagDisableCollision() const {
  return impl->GetField(GFF_MMH_CLOTH_FLAG_DISABLE_COLLISION)->asUInt8();
}

void NCLT::set_clothFlagDisableCollision(unsigned char value) {
  impl->GetField(GFF_MMH_CLOTH_FLAG_DISABLE_COLLISION)->assign(value);
}

unsigned char NCLT::get_clothFlagSelfcollision() const {
  return impl->GetField(GFF_MMH_CLOTH_FLAG_SELFCOLLISION)->asUInt8();
}

void NCLT::set_clothFlagSelfcollision(unsigned char value) {
  impl->GetField(GFF_MMH_CLOTH_FLAG_SELFCOLLISION)->assign(value);
}

unsigned char NCLT::get_clothFlagVisualization() const {
  return impl->GetField(GFF_MMH_CLOTH_FLAG_VISUALIZATION)->asUInt8();
}

void NCLT::set_clothFlagVisualization(unsigned char value) {
  impl->GetField(GFF_MMH_CLOTH_FLAG_VISUALIZATION)->assign(value);
}

unsigned char NCLT::get_clothFlagGravity() const {
  return impl->GetField(GFF_MMH_CLOTH_FLAG_GRAVITY)->asUInt8();
}

void NCLT::set_clothFlagGravity(unsigned char value) {
  impl->GetField(GFF_MMH_CLOTH_FLAG_GRAVITY)->assign(value);
}

unsigned char NCLT::get_clothFlagBending() const {
  return impl->GetField(GFF_MMH_CLOTH_FLAG_BENDING)->asUInt8();
}

void NCLT::set_clothFlagBending(unsigned char value) {
  impl->GetField(GFF_MMH_CLOTH_FLAG_BENDING)->assign(value);
}

unsigned char NCLT::get_clothFlagBendingOrtho() const {
  return impl->GetField(GFF_MMH_CLOTH_FLAG_BENDING_ORTHO)->asUInt8();
}

void NCLT::set_clothFlagBendingOrtho(unsigned char value) {
  impl->GetField(GFF_MMH_CLOTH_FLAG_BENDING_ORTHO)->assign(value);
}

unsigned char NCLT::get_clothFlagDamping() const {
  return impl->GetField(GFF_MMH_CLOTH_FLAG_DAMPING)->asUInt8();
}

void NCLT::set_clothFlagDamping(unsigned char value) {
  impl->GetField(GFF_MMH_CLOTH_FLAG_DAMPING)->assign(value);
}

unsigned char NCLT::get_clothFlagCollisionTwoway() const {
  return impl->GetField(GFF_MMH_CLOTH_FLAG_COLLISION_TWOWAY)->asUInt8();
}

void NCLT::set_clothFlagCollisionTwoway(unsigned char value) {
  impl->GetField(GFF_MMH_CLOTH_FLAG_COLLISION_TWOWAY)->assign(value);
}

unsigned char NCLT::get_clothFlagTriangleCollision() const {
  return impl->GetField(GFF_MMH_CLOTH_FLAG_TRIANGLE_COLLISION)->asUInt8();
}

void NCLT::set_clothFlagTriangleCollision(unsigned char value) {
  impl->GetField(GFF_MMH_CLOTH_FLAG_TRIANGLE_COLLISION)->assign(value);
}

unsigned char NCLT::get_clothFlagTearable() const {
  return impl->GetField(GFF_MMH_CLOTH_FLAG_TEARABLE)->asUInt8();
}

void NCLT::set_clothFlagTearable(unsigned char value) {
  impl->GetField(GFF_MMH_CLOTH_FLAG_TEARABLE)->assign(value);
}

unsigned char NCLT::get_clothFlagHardware() const {
  return impl->GetField(GFF_MMH_CLOTH_FLAG_HARDWARE)->asUInt8();
}

void NCLT::set_clothFlagHardware(unsigned char value) {
  impl->GetField(GFF_MMH_CLOTH_FLAG_HARDWARE)->assign(value);
}

unsigned char NCLT::get_clothFlagComdamping() const {
  return impl->GetField(GFF_MMH_CLOTH_FLAG_COMDAMPING)->asUInt8();
}

void NCLT::set_clothFlagComdamping(unsigned char value) {
  impl->GetField(GFF_MMH_CLOTH_FLAG_COMDAMPING)->assign(value);
}

GFFListRef NCLT::get_clothCookedDataStream() const {
  return impl->GetField(GFF_MMH_CLOTH_COOKED_DATA_STREAM)->asList();
}

GFFStructRef NCLT::get_clothMeshGroupStruct() const {
  return impl->GetField(GFF_MMH_CLOTH_MESH_GROUP_STRUCT)->asStruct();
}

unsigned char NCLT::get_shapeCollisionMaskWater() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_WATER)->asUInt8();
}

void NCLT::set_shapeCollisionMaskWater(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_WATER)->assign(value);
}

unsigned char NCLT::get_shapeCollisionMaskTerrainWall() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_TERRAIN_WALL)->asUInt8();
}

void NCLT::set_shapeCollisionMaskTerrainWall(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_TERRAIN_WALL)->assign(value);
}

unsigned char NCLT::get_shapeCollisionMaskWalkable() const {
  return impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_WALKABLE)->asUInt8();
}

void NCLT::set_shapeCollisionMaskWalkable(unsigned char value) {
  impl->GetField(GFF_MMH_SHAPE_COLLISION_MASK_WALKABLE)->assign(value);
}

GFFListRef NCLT::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

