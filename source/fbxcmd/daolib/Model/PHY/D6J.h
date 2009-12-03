
/**********************************************************************
*<
FILE: D6J.h

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

class D6J
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  D6J(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  unsigned long get_joint6DofXMotion() const;
  void set_joint6DofXMotion(unsigned long value);

  unsigned long get_joint6DofYMotion() const;
  void set_joint6DofYMotion(unsigned long value);

  unsigned long get_joint6DofZMotion() const;
  void set_joint6DofZMotion(unsigned long value);

  unsigned long get_joint6DofSwing1Motion() const;
  void set_joint6DofSwing1Motion(unsigned long value);

  unsigned long get_joint6DofSwing2Motion() const;
  void set_joint6DofSwing2Motion(unsigned long value);

  unsigned long get_joint6DofTwistMotion() const;
  void set_joint6DofTwistMotion(unsigned long value);

  Vector4f get_joint6DofLinearLimit() const;
  void set_joint6DofLinearLimit(Vector4f value);

  Vector4f get_joint6DofSwing1Limit() const;
  void set_joint6DofSwing1Limit(Vector4f value);

  Vector4f get_joint6DofSwing2Limit() const;
  void set_joint6DofSwing2Limit(Vector4f value);

  Vector4f get_joint6DofTwistLimitLow() const;
  void set_joint6DofTwistLimitLow(Vector4f value);

  Vector4f get_joint6DofTwistLimitHigh() const;
  void set_joint6DofTwistLimitHigh(Vector4f value);

  Quaternion get_joint6DofDriveOrientation() const;
  void set_joint6DofDriveOrientation(Quaternion value);

  unsigned long get_joint6DofDriveXDriveType() const;
  void set_joint6DofDriveXDriveType(unsigned long value);

  float get_joint6DofDriveXDriveSpring() const;
  void set_joint6DofDriveXDriveSpring(float value);

  float get_joint6DofDriveXDriveDamping() const;
  void set_joint6DofDriveXDriveDamping(float value);

  float get_joint6DofDriveXDriveForceLimit() const;
  void set_joint6DofDriveXDriveForceLimit(float value);

  unsigned long get_joint6DofDriveYDriveSpring() const;
  void set_joint6DofDriveYDriveSpring(unsigned long value);

  float get_joint6DofDriveYDriveDamping() const;
  void set_joint6DofDriveYDriveDamping(float value);

  float get_joint6DofDriveYDriveForceLimit() const;
  void set_joint6DofDriveYDriveForceLimit(float value);

  unsigned long get_joint6DofDriveZDriveType() const;
  void set_joint6DofDriveZDriveType(unsigned long value);

  float get_joint6DofDriveZDriveSpring() const;
  void set_joint6DofDriveZDriveSpring(float value);

  float get_joint6DofDriveZDriveDamping() const;
  void set_joint6DofDriveZDriveDamping(float value);

  float get_joint6DofDriveZDriveForceLimit() const;
  void set_joint6DofDriveZDriveForceLimit(float value);

  unsigned long get_joint6DofDriveSwingDriveType() const;
  void set_joint6DofDriveSwingDriveType(unsigned long value);

  float get_joint6DofDriveSwingDriveSpring() const;
  void set_joint6DofDriveSwingDriveSpring(float value);

  float get_joint6DofDriveSwingDriveDamping() const;
  void set_joint6DofDriveSwingDriveDamping(float value);

  float get_joint6DofDriveSwingDriveForceLimit() const;
  void set_joint6DofDriveSwingDriveForceLimit(float value);

  unsigned long get_joint6DofDriveTwistDriveType() const;
  void set_joint6DofDriveTwistDriveType(unsigned long value);

  float get_joint6DofDriveTwistDriveSpring() const;
  void set_joint6DofDriveTwistDriveSpring(float value);

  float get_joint6DofDriveTwistDriveDamping() const;
  void set_joint6DofDriveTwistDriveDamping(float value);

  float get_joint6DofDriveTwistDriveForceLimit() const;
  void set_joint6DofDriveTwistDriveForceLimit(float value);

  unsigned long get_joint6DofDriveSlerpDriveType() const;
  void set_joint6DofDriveSlerpDriveType(unsigned long value);

  float get_joint6DofDriveSlerpDriveSpring() const;
  void set_joint6DofDriveSlerpDriveSpring(float value);

  float get_joint6DofDriveSlerpDriveDamping() const;
  void set_joint6DofDriveSlerpDriveDamping(float value);

  float get_joint6DofDriveSlerpDriveForceLimit() const;
  void set_joint6DofDriveSlerpDriveForceLimit(float value);

  Vector3f get_joint6DofDrivePosition() const;
  void set_joint6DofDrivePosition(Vector3f value);

  Vector3f get_joint6DofDriveLinearVelocity() const;
  void set_joint6DofDriveLinearVelocity(Vector3f value);

  Vector3f get_joint6DofDriveAngularVelocity() const;
  void set_joint6DofDriveAngularVelocity(Vector3f value);

  float get_joint6DofProjectionDistance() const;
  void set_joint6DofProjectionDistance(float value);

  float get_joint6DofProjectionAngle() const;
  void set_joint6DofProjectionAngle(float value);

  float get_joint6DofGearRatio() const;
  void set_joint6DofGearRatio(float value);

  unsigned long get_joint6DofProjectionMode() const;
  void set_joint6DofProjectionMode(unsigned long value);

  unsigned long get_joint6DofD6Flags() const;
  void set_joint6DofD6Flags(unsigned long value);

};

typedef ValuePtr<D6J> D6JPtr;
typedef ValueRef<D6J> D6JRef;



} //namespace PHY
} //namespace DAO
