
/**********************************************************************
*<
FILE: D6J.h

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
#include "PHY/D6J.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::PHY;
///////////////////////////////////////////////////////////////////

ShortString D6J::type("D6J");

D6J::D6J(GFFStructRef owner) : impl(owner) {
}

unsigned long D6J::get_joint6DofXMotion() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_X_MOTION)->asUInt32();
}

void D6J::set_joint6DofXMotion(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_X_MOTION)->assign(value);
}

unsigned long D6J::get_joint6DofYMotion() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_Y_MOTION)->asUInt32();
}

void D6J::set_joint6DofYMotion(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_Y_MOTION)->assign(value);
}

unsigned long D6J::get_joint6DofZMotion() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_Z_MOTION)->asUInt32();
}

void D6J::set_joint6DofZMotion(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_Z_MOTION)->assign(value);
}

unsigned long D6J::get_joint6DofSwing1Motion() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_SWING_1_MOTION)->asUInt32();
}

void D6J::set_joint6DofSwing1Motion(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_SWING_1_MOTION)->assign(value);
}

unsigned long D6J::get_joint6DofSwing2Motion() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_SWING_2_MOTION)->asUInt32();
}

void D6J::set_joint6DofSwing2Motion(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_SWING_2_MOTION)->assign(value);
}

unsigned long D6J::get_joint6DofTwistMotion() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_TWIST_MOTION)->asUInt32();
}

void D6J::set_joint6DofTwistMotion(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_TWIST_MOTION)->assign(value);
}

Vector4f D6J::get_joint6DofLinearLimit() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_LINEAR_LIMIT)->asVector4f();
}

void D6J::set_joint6DofLinearLimit(Vector4f value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_LINEAR_LIMIT)->assign(value);
}

Vector4f D6J::get_joint6DofSwing1Limit() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_SWING_1_LIMIT)->asVector4f();
}

void D6J::set_joint6DofSwing1Limit(Vector4f value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_SWING_1_LIMIT)->assign(value);
}

Vector4f D6J::get_joint6DofSwing2Limit() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_SWING_2_LIMIT)->asVector4f();
}

void D6J::set_joint6DofSwing2Limit(Vector4f value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_SWING_2_LIMIT)->assign(value);
}

Vector4f D6J::get_joint6DofTwistLimitLow() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_TWIST_LIMIT_LOW)->asVector4f();
}

void D6J::set_joint6DofTwistLimitLow(Vector4f value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_TWIST_LIMIT_LOW)->assign(value);
}

Vector4f D6J::get_joint6DofTwistLimitHigh() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_TWIST_LIMIT_HIGH)->asVector4f();
}

void D6J::set_joint6DofTwistLimitHigh(Vector4f value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_TWIST_LIMIT_HIGH)->assign(value);
}

Quaternion D6J::get_joint6DofDriveOrientation() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_ORIENTATION)->asQuat();
}

void D6J::set_joint6DofDriveOrientation(Quaternion value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_ORIENTATION)->assign(value);
}

unsigned long D6J::get_joint6DofDriveXDriveType() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_X_DRIVE_TYPE)->asUInt32();
}

void D6J::set_joint6DofDriveXDriveType(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_X_DRIVE_TYPE)->assign(value);
}

float D6J::get_joint6DofDriveXDriveSpring() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_X_DRIVE_SPRING)->asFloat32();
}

void D6J::set_joint6DofDriveXDriveSpring(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_X_DRIVE_SPRING)->assign(value);
}

float D6J::get_joint6DofDriveXDriveDamping() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_X_DRIVE_DAMPING)->asFloat32();
}

void D6J::set_joint6DofDriveXDriveDamping(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_X_DRIVE_DAMPING)->assign(value);
}

float D6J::get_joint6DofDriveXDriveForceLimit() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_X_DRIVE_FORCE_LIMIT)->asFloat32();
}

void D6J::set_joint6DofDriveXDriveForceLimit(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_X_DRIVE_FORCE_LIMIT)->assign(value);
}

unsigned long D6J::get_joint6DofDriveYDriveSpring() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_Y_DRIVE_SPRING)->asUInt32();
}

void D6J::set_joint6DofDriveYDriveSpring(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_Y_DRIVE_SPRING)->assign(value);
}

float D6J::get_joint6DofDriveYDriveDamping() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_Y_DRIVE_DAMPING)->asFloat32();
}

void D6J::set_joint6DofDriveYDriveDamping(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_Y_DRIVE_DAMPING)->assign(value);
}

float D6J::get_joint6DofDriveYDriveForceLimit() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_Y_DRIVE_FORCE_LIMIT)->asFloat32();
}

void D6J::set_joint6DofDriveYDriveForceLimit(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_Y_DRIVE_FORCE_LIMIT)->assign(value);
}

unsigned long D6J::get_joint6DofDriveZDriveType() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_Z_DRIVE_TYPE)->asUInt32();
}

void D6J::set_joint6DofDriveZDriveType(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_Z_DRIVE_TYPE)->assign(value);
}

float D6J::get_joint6DofDriveZDriveSpring() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_Z_DRIVE_SPRING)->asFloat32();
}

void D6J::set_joint6DofDriveZDriveSpring(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_Z_DRIVE_SPRING)->assign(value);
}

float D6J::get_joint6DofDriveZDriveDamping() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_Z_DRIVE_DAMPING)->asFloat32();
}

void D6J::set_joint6DofDriveZDriveDamping(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_Z_DRIVE_DAMPING)->assign(value);
}

float D6J::get_joint6DofDriveZDriveForceLimit() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_Z_DRIVE_FORCE_LIMIT)->asFloat32();
}

void D6J::set_joint6DofDriveZDriveForceLimit(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_Z_DRIVE_FORCE_LIMIT)->assign(value);
}

unsigned long D6J::get_joint6DofDriveSwingDriveType() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_SWING_DRIVE_TYPE)->asUInt32();
}

void D6J::set_joint6DofDriveSwingDriveType(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_SWING_DRIVE_TYPE)->assign(value);
}

float D6J::get_joint6DofDriveSwingDriveSpring() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_SWING_DRIVE_SPRING)->asFloat32();
}

void D6J::set_joint6DofDriveSwingDriveSpring(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_SWING_DRIVE_SPRING)->assign(value);
}

float D6J::get_joint6DofDriveSwingDriveDamping() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_SWING_DRIVE_DAMPING)->asFloat32();
}

void D6J::set_joint6DofDriveSwingDriveDamping(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_SWING_DRIVE_DAMPING)->assign(value);
}

float D6J::get_joint6DofDriveSwingDriveForceLimit() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_SWING_DRIVE_FORCE_LIMIT)->asFloat32();
}

void D6J::set_joint6DofDriveSwingDriveForceLimit(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_SWING_DRIVE_FORCE_LIMIT)->assign(value);
}

unsigned long D6J::get_joint6DofDriveTwistDriveType() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_TWIST_DRIVE_TYPE)->asUInt32();
}

void D6J::set_joint6DofDriveTwistDriveType(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_TWIST_DRIVE_TYPE)->assign(value);
}

float D6J::get_joint6DofDriveTwistDriveSpring() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_TWIST_DRIVE_SPRING)->asFloat32();
}

void D6J::set_joint6DofDriveTwistDriveSpring(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_TWIST_DRIVE_SPRING)->assign(value);
}

float D6J::get_joint6DofDriveTwistDriveDamping() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_TWIST_DRIVE_DAMPING)->asFloat32();
}

void D6J::set_joint6DofDriveTwistDriveDamping(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_TWIST_DRIVE_DAMPING)->assign(value);
}

float D6J::get_joint6DofDriveTwistDriveForceLimit() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_TWIST_DRIVE_FORCE_LIMIT)->asFloat32();
}

void D6J::set_joint6DofDriveTwistDriveForceLimit(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_TWIST_DRIVE_FORCE_LIMIT)->assign(value);
}

unsigned long D6J::get_joint6DofDriveSlerpDriveType() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_SLERP_DRIVE_TYPE)->asUInt32();
}

void D6J::set_joint6DofDriveSlerpDriveType(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_SLERP_DRIVE_TYPE)->assign(value);
}

float D6J::get_joint6DofDriveSlerpDriveSpring() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_SLERP_DRIVE_SPRING)->asFloat32();
}

void D6J::set_joint6DofDriveSlerpDriveSpring(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_SLERP_DRIVE_SPRING)->assign(value);
}

float D6J::get_joint6DofDriveSlerpDriveDamping() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_SLERP_DRIVE_DAMPING)->asFloat32();
}

void D6J::set_joint6DofDriveSlerpDriveDamping(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_SLERP_DRIVE_DAMPING)->assign(value);
}

float D6J::get_joint6DofDriveSlerpDriveForceLimit() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_SLERP_DRIVE_FORCE_LIMIT)->asFloat32();
}

void D6J::set_joint6DofDriveSlerpDriveForceLimit(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_SLERP_DRIVE_FORCE_LIMIT)->assign(value);
}

Vector3f D6J::get_joint6DofDrivePosition() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_POSITION)->asVector3f();
}

void D6J::set_joint6DofDrivePosition(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_POSITION)->assign(value);
}

Vector3f D6J::get_joint6DofDriveLinearVelocity() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_LINEAR_VELOCITY)->asVector3f();
}

void D6J::set_joint6DofDriveLinearVelocity(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_LINEAR_VELOCITY)->assign(value);
}

Vector3f D6J::get_joint6DofDriveAngularVelocity() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_ANGULAR_VELOCITY)->asVector3f();
}

void D6J::set_joint6DofDriveAngularVelocity(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_DRIVE_ANGULAR_VELOCITY)->assign(value);
}

float D6J::get_joint6DofProjectionDistance() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_PROJECTION_DISTANCE)->asFloat32();
}

void D6J::set_joint6DofProjectionDistance(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_PROJECTION_DISTANCE)->assign(value);
}

float D6J::get_joint6DofProjectionAngle() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_PROJECTION_ANGLE)->asFloat32();
}

void D6J::set_joint6DofProjectionAngle(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_PROJECTION_ANGLE)->assign(value);
}

float D6J::get_joint6DofGearRatio() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_GEAR_RATIO)->asFloat32();
}

void D6J::set_joint6DofGearRatio(float value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_GEAR_RATIO)->assign(value);
}

unsigned long D6J::get_joint6DofProjectionMode() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_PROJECTION_MODE)->asUInt32();
}

void D6J::set_joint6DofProjectionMode(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_PROJECTION_MODE)->assign(value);
}

unsigned long D6J::get_joint6DofD6Flags() const {
  return impl->GetField(GFF_MMH_JOINT_6DOF_D6_FLAGS)->asUInt32();
}

void D6J::set_joint6DofD6Flags(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_6DOF_D6_FLAGS)->assign(value);
}

