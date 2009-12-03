
/**********************************************************************
*<
FILE: JNT.h

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
#include "PHY/JNT.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::PHY;
///////////////////////////////////////////////////////////////////

ShortString JNT::type("JNT");JNT::JNT(GFFStructRef owner) : impl(owner) {
}

Text JNT::get_jointTarget() const {
  return impl->GetField(GFF_MMH_JOINT_TARGET)->asECString();
}

void JNT::set_jointTarget(const Text& value) {
  impl->GetField(GFF_MMH_JOINT_TARGET)->assign(value);
}

unsigned long JNT::get_jointType() const {
  return impl->GetField(GFF_MMH_JOINT_TYPE)->asUInt32();
}

void JNT::set_jointType(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_TYPE)->assign(value);
}

GFFFieldRef JNT::get_jointTypeStruct() const {
  return impl->GetField(GFF_MMH_JOINT_TYPE_STRUCT);
}

Vector3f JNT::get_jointLocalNormal1() const {
  return impl->GetField(GFF_MMH_JOINT_LOCAL_NORMAL_1)->asVector3f();
}

void JNT::set_jointLocalNormal1(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_LOCAL_NORMAL_1)->assign(value);
}

Vector3f JNT::get_jointLocalNormal2() const {
  return impl->GetField(GFF_MMH_JOINT_LOCAL_NORMAL_2)->asVector3f();
}

void JNT::set_jointLocalNormal2(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_LOCAL_NORMAL_2)->assign(value);
}

Vector3f JNT::get_jointLocalAnchor1() const {
  return impl->GetField(GFF_MMH_JOINT_LOCAL_ANCHOR_1)->asVector3f();
}

void JNT::set_jointLocalAnchor1(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_LOCAL_ANCHOR_1)->assign(value);
}

Vector3f JNT::get_jointLocalAnchor2() const {
  return impl->GetField(GFF_MMH_JOINT_LOCAL_ANCHOR_2)->asVector3f();
}

void JNT::set_jointLocalAnchor2(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_LOCAL_ANCHOR_2)->assign(value);
}

Vector3f JNT::get_jointLocalAxis1() const {
  return impl->GetField(GFF_MMH_JOINT_LOCAL_AXIS_1)->asVector3f();
}

void JNT::set_jointLocalAxis1(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_LOCAL_AXIS_1)->assign(value);
}

Vector3f JNT::get_jointLocalAxis2() const {
  return impl->GetField(GFF_MMH_JOINT_LOCAL_AXIS_2)->asVector3f();
}

void JNT::set_jointLocalAxis2(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_LOCAL_AXIS_2)->assign(value);
}

float JNT::get_jointMaxForce() const {
  return impl->GetField(GFF_MMH_JOINT_MAX_FORCE)->asFloat32();
}

void JNT::set_jointMaxForce(float value) {
  impl->GetField(GFF_MMH_JOINT_MAX_FORCE)->assign(value);
}

float JNT::get_jointMaxTorque() const {
  return impl->GetField(GFF_MMH_JOINT_MAX_TORQUE)->asFloat32();
}

void JNT::set_jointMaxTorque(float value) {
  impl->GetField(GFF_MMH_JOINT_MAX_TORQUE)->assign(value);
}

unsigned char JNT::get_jointCollisionEnabled() const {
  return impl->GetField(GFF_MMH_JOINT_COLLISION_ENABLED)->asUInt8();
}

void JNT::set_jointCollisionEnabled(unsigned char value) {
  impl->GetField(GFF_MMH_JOINT_COLLISION_ENABLED)->assign(value);
}

