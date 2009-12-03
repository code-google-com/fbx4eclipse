
/**********************************************************************
*<
FILE: PLYJ.h

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
#include "PHY/PLYJ.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::PHY;
///////////////////////////////////////////////////////////////////

ShortString PLYJ::type("PLYJ");PLYJ::PLYJ(GFFStructRef owner) : impl(owner) {
}

Vector3f PLYJ::get_jointPulleyPulley1() const {
  return impl->GetField(GFF_MMH_JOINT_PULLEY_PULLEY_1)->asVector3f();
}

void PLYJ::set_jointPulleyPulley1(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_PULLEY_PULLEY_1)->assign(value);
}

Vector3f PLYJ::get_jointPulleyPulley2() const {
  return impl->GetField(GFF_MMH_JOINT_PULLEY_PULLEY_2)->asVector3f();
}

void PLYJ::set_jointPulleyPulley2(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_PULLEY_PULLEY_2)->assign(value);
}

float PLYJ::get_jointPulleyDistance() const {
  return impl->GetField(GFF_MMH_JOINT_PULLEY_DISTANCE)->asFloat32();
}

void PLYJ::set_jointPulleyDistance(float value) {
  impl->GetField(GFF_MMH_JOINT_PULLEY_DISTANCE)->assign(value);
}

float PLYJ::get_jointPulleyStiffness() const {
  return impl->GetField(GFF_MMH_JOINT_PULLEY_STIFFNESS)->asFloat32();
}

void PLYJ::set_jointPulleyStiffness(float value) {
  impl->GetField(GFF_MMH_JOINT_PULLEY_STIFFNESS)->assign(value);
}

float PLYJ::get_jointPulleyRatio() const {
  return impl->GetField(GFF_MMH_JOINT_PULLEY_RATIO)->asFloat32();
}

void PLYJ::set_jointPulleyRatio(float value) {
  impl->GetField(GFF_MMH_JOINT_PULLEY_RATIO)->assign(value);
}

float PLYJ::get_jointPulleyMotorVelTarget() const {
  return impl->GetField(GFF_MMH_JOINT_PULLEY_MOTOR_VEL_TARGET)->asFloat32();
}

void PLYJ::set_jointPulleyMotorVelTarget(float value) {
  impl->GetField(GFF_MMH_JOINT_PULLEY_MOTOR_VEL_TARGET)->assign(value);
}

float PLYJ::get_jointPulleyMotorMaxForce() const {
  return impl->GetField(GFF_MMH_JOINT_PULLEY_MOTOR_MAX_FORCE)->asFloat32();
}

void PLYJ::set_jointPulleyMotorMaxForce(float value) {
  impl->GetField(GFF_MMH_JOINT_PULLEY_MOTOR_MAX_FORCE)->assign(value);
}

unsigned char PLYJ::get_jointPulleyMotorFreeSpin() const {
  return impl->GetField(GFF_MMH_JOINT_PULLEY_MOTOR_FREE_SPIN)->asUInt8();
}

void PLYJ::set_jointPulleyMotorFreeSpin(unsigned char value) {
  impl->GetField(GFF_MMH_JOINT_PULLEY_MOTOR_FREE_SPIN)->assign(value);
}

unsigned long PLYJ::get_jointPulleyPulleyFlags() const {
  return impl->GetField(GFF_MMH_JOINT_PULLEY_PULLEY_FLAGS)->asUInt32();
}

void PLYJ::set_jointPulleyPulleyFlags(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_PULLEY_PULLEY_FLAGS)->assign(value);
}

