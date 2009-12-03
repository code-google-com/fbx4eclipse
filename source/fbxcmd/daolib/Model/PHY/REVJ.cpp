
/**********************************************************************
*<
FILE: REVJ.h

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
#include "PHY/REVJ.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::PHY;
///////////////////////////////////////////////////////////////////

ShortString REVJ::type("REVJ");REVJ::REVJ(GFFStructRef owner) : impl(owner) {
}

Vector3f REVJ::get_jointRevoluteLimitLow() const {
  return impl->GetField(GFF_MMH_JOINT_REVOLUTE_LIMIT_LOW)->asVector3f();
}

void REVJ::set_jointRevoluteLimitLow(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_REVOLUTE_LIMIT_LOW)->assign(value);
}

Vector3f REVJ::get_jointRevoluteLimitHigh() const {
  return impl->GetField(GFF_MMH_JOINT_REVOLUTE_LIMIT_HIGH)->asVector3f();
}

void REVJ::set_jointRevoluteLimitHigh(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_REVOLUTE_LIMIT_HIGH)->assign(value);
}

float REVJ::get_jointRevoluteProjectionDistance() const {
  return impl->GetField(GFF_MMH_JOINT_REVOLUTE_PROJECTION_DISTANCE)->asFloat32();
}

void REVJ::set_jointRevoluteProjectionDistance(float value) {
  impl->GetField(GFF_MMH_JOINT_REVOLUTE_PROJECTION_DISTANCE)->assign(value);
}

float REVJ::get_jointRevoluteProjectionAngle() const {
  return impl->GetField(GFF_MMH_JOINT_REVOLUTE_PROJECTION_ANGLE)->asFloat32();
}

void REVJ::set_jointRevoluteProjectionAngle(float value) {
  impl->GetField(GFF_MMH_JOINT_REVOLUTE_PROJECTION_ANGLE)->assign(value);
}

unsigned long REVJ::get_jointRevoluteProjectionMode() const {
  return impl->GetField(GFF_MMH_JOINT_REVOLUTE_PROJECTION_MODE)->asUInt32();
}

void REVJ::set_jointRevoluteProjectionMode(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_REVOLUTE_PROJECTION_MODE)->assign(value);
}

Vector3f REVJ::get_jointRevoluteSpring() const {
  return impl->GetField(GFF_MMH_JOINT_REVOLUTE_SPRING)->asVector3f();
}

void REVJ::set_jointRevoluteSpring(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_REVOLUTE_SPRING)->assign(value);
}

float REVJ::get_jointRevoluteMotorVelTarget() const {
  return impl->GetField(GFF_MMH_JOINT_REVOLUTE_MOTOR_VEL_TARGET)->asFloat32();
}

void REVJ::set_jointRevoluteMotorVelTarget(float value) {
  impl->GetField(GFF_MMH_JOINT_REVOLUTE_MOTOR_VEL_TARGET)->assign(value);
}

float REVJ::get_jointRevoluteMotorMaxForce() const {
  return impl->GetField(GFF_MMH_JOINT_REVOLUTE_MOTOR_MAX_FORCE)->asFloat32();
}

void REVJ::set_jointRevoluteMotorMaxForce(float value) {
  impl->GetField(GFF_MMH_JOINT_REVOLUTE_MOTOR_MAX_FORCE)->assign(value);
}

unsigned char REVJ::get_jointRevoluteMotorFreeSpin() const {
  return impl->GetField(GFF_MMH_JOINT_REVOLUTE_MOTOR_FREE_SPIN)->asUInt8();
}

void REVJ::set_jointRevoluteMotorFreeSpin(unsigned char value) {
  impl->GetField(GFF_MMH_JOINT_REVOLUTE_MOTOR_FREE_SPIN)->assign(value);
}

unsigned long REVJ::get_jointRevoluteRevoluteFlags() const {
  return impl->GetField(GFF_MMH_JOINT_REVOLUTE_REVOLUTE_FLAGS)->asUInt32();
}

void REVJ::set_jointRevoluteRevoluteFlags(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_REVOLUTE_REVOLUTE_FLAGS)->assign(value);
}

