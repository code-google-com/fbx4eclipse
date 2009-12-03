
/**********************************************************************
*<
FILE: SPNV.h

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
#include "MMH/SPNV.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString SPNV::type("SPNV");SPNV::SPNV(GFFStructRef owner) : impl(owner) {
}

unsigned char SPNV::get_nodeSpawnVolumeOptionsSpawnWithinVolume() const {
  return impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_OPTIONS_SPAWN_WITHIN_VOLUME)->asUInt8();
}

void SPNV::set_nodeSpawnVolumeOptionsSpawnWithinVolume(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_OPTIONS_SPAWN_WITHIN_VOLUME)->assign(value);
}

unsigned char SPNV::get_nodeSpawnVolumeOptionsInvertSpawnVolumeNormals() const {
  return impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_OPTIONS_INVERT_SPAWN_VOLUME_NORMALS)->asUInt8();
}

void SPNV::set_nodeSpawnVolumeOptionsInvertSpawnVolumeNormals(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_OPTIONS_INVERT_SPAWN_VOLUME_NORMALS)->assign(value);
}

unsigned char SPNV::get_nodeSpawnVolumeType() const {
  return impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_TYPE)->asUInt8();
}

void SPNV::set_nodeSpawnVolumeType(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_TYPE)->assign(value);
}

float SPNV::get_nodeSpawnVolumeRadius() const {
  return impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_RADIUS)->asFloat32();
}

void SPNV::set_nodeSpawnVolumeRadius(float value) {
  impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_RADIUS)->assign(value);
}

float SPNV::get_nodeSpawnVolumeCylinderLength() const {
  return impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_CYLINDER_LENGTH)->asFloat32();
}

void SPNV::set_nodeSpawnVolumeCylinderLength(float value) {
  impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_CYLINDER_LENGTH)->assign(value);
}

Vector3f SPNV::get_nodeSpawnVolumeCylinderAxis() const {
  return impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_CYLINDER_AXIS)->asVector3f();
}

void SPNV::set_nodeSpawnVolumeCylinderAxis(Vector3f value) {
  impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_CYLINDER_AXIS)->assign(value);
}

Vector3f SPNV::get_nodeSpawnVolumeBoxMin() const {
  return impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_BOX_MIN)->asVector3f();
}

void SPNV::set_nodeSpawnVolumeBoxMin(Vector3f value) {
  impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_BOX_MIN)->assign(value);
}

Vector3f SPNV::get_nodeSpawnVolumeBoxMax() const {
  return impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_BOX_MAX)->asVector3f();
}

void SPNV::set_nodeSpawnVolumeBoxMax(Vector3f value) {
  impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_BOX_MAX)->assign(value);
}

unsigned char SPNV::get_nodeSpawnVolumeOptionsNormalsAsDirection() const {
  return impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_OPTIONS_NORMALS_AS_DIRECTION)->asUInt8();
}

void SPNV::set_nodeSpawnVolumeOptionsNormalsAsDirection(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_SPAWN_VOLUME_OPTIONS_NORMALS_AS_DIRECTION)->assign(value);
}

GFFListRef SPNV::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

