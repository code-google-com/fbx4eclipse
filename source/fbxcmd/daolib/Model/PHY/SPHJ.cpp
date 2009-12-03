
/**********************************************************************
*<
FILE: SPHJ.h

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
#include "PHY/SPHJ.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::PHY;
///////////////////////////////////////////////////////////////////

ShortString SPHJ::type("SPHJ");SPHJ::SPHJ(GFFStructRef owner) : impl(owner) {
}

Vector3f SPHJ::get_jointSphericalSwingAxis() const {
  return impl->GetField(GFF_MMH_JOINT_SPHERICAL_SWING_AXIS)->asVector3f();
}

void SPHJ::set_jointSphericalSwingAxis(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_SPHERICAL_SWING_AXIS)->assign(value);
}

float SPHJ::get_jointSphericalProjectionDistance() const {
  return impl->GetField(GFF_MMH_JOINT_SPHERICAL_PROJECTION_DISTANCE)->asFloat32();
}

void SPHJ::set_jointSphericalProjectionDistance(float value) {
  impl->GetField(GFF_MMH_JOINT_SPHERICAL_PROJECTION_DISTANCE)->assign(value);
}

Vector3f SPHJ::get_jointSphericalTwistLimitLow() const {
  return impl->GetField(GFF_MMH_JOINT_SPHERICAL_TWIST_LIMIT_LOW)->asVector3f();
}

void SPHJ::set_jointSphericalTwistLimitLow(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_SPHERICAL_TWIST_LIMIT_LOW)->assign(value);
}

Vector3f SPHJ::get_jointSphericalTwistLimitHigh() const {
  return impl->GetField(GFF_MMH_JOINT_SPHERICAL_TWIST_LIMIT_HIGH)->asVector3f();
}

void SPHJ::set_jointSphericalTwistLimitHigh(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_SPHERICAL_TWIST_LIMIT_HIGH)->assign(value);
}

Vector3f SPHJ::get_jointSphericalSwingLimit() const {
  return impl->GetField(GFF_MMH_JOINT_SPHERICAL_SWING_LIMIT)->asVector3f();
}

void SPHJ::set_jointSphericalSwingLimit(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_SPHERICAL_SWING_LIMIT)->assign(value);
}

Vector3f SPHJ::get_jointSphericalTwistSwing() const {
  return impl->GetField(GFF_MMH_JOINT_SPHERICAL_TWIST_SWING)->asVector3f();
}

void SPHJ::set_jointSphericalTwistSwing(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_SPHERICAL_TWIST_SWING)->assign(value);
}

Vector3f SPHJ::get_jointSphericalSwingSpring() const {
  return impl->GetField(GFF_MMH_JOINT_SPHERICAL_SWING_SPRING)->asVector3f();
}

void SPHJ::set_jointSphericalSwingSpring(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_SPHERICAL_SWING_SPRING)->assign(value);
}

Vector3f SPHJ::get_jointSphericalJointSpring() const {
  return impl->GetField(GFF_MMH_JOINT_SPHERICAL_JOINT_SPRING)->asVector3f();
}

void SPHJ::set_jointSphericalJointSpring(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_SPHERICAL_JOINT_SPRING)->assign(value);
}

unsigned long SPHJ::get_jointSphericalProjectionMode() const {
  return impl->GetField(GFF_MMH_JOINT_SPHERICAL_PROJECTION_MODE)->asUInt32();
}

void SPHJ::set_jointSphericalProjectionMode(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_SPHERICAL_PROJECTION_MODE)->assign(value);
}

unsigned long SPHJ::get_jointSphericalSphereFlags() const {
  return impl->GetField(GFF_MMH_JOINT_SPHERICAL_SPHERE_FLAGS)->asUInt32();
}

void SPHJ::set_jointSphericalSphereFlags(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_SPHERICAL_SPHERE_FLAGS)->assign(value);
}

