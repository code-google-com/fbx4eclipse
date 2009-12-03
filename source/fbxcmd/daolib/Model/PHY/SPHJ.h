
/**********************************************************************
*<
FILE: SPHJ.h

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

class SPHJ
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  SPHJ(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Vector3f get_jointSphericalSwingAxis() const;
  void set_jointSphericalSwingAxis(Vector3f value);

  float get_jointSphericalProjectionDistance() const;
  void set_jointSphericalProjectionDistance(float value);

  Vector3f get_jointSphericalTwistLimitLow() const;
  void set_jointSphericalTwistLimitLow(Vector3f value);

  Vector3f get_jointSphericalTwistLimitHigh() const;
  void set_jointSphericalTwistLimitHigh(Vector3f value);

  Vector3f get_jointSphericalSwingLimit() const;
  void set_jointSphericalSwingLimit(Vector3f value);

  Vector3f get_jointSphericalTwistSwing() const;
  void set_jointSphericalTwistSwing(Vector3f value);

  Vector3f get_jointSphericalSwingSpring() const;
  void set_jointSphericalSwingSpring(Vector3f value);

  Vector3f get_jointSphericalJointSpring() const;
  void set_jointSphericalJointSpring(Vector3f value);

  unsigned long get_jointSphericalProjectionMode() const;
  void set_jointSphericalProjectionMode(unsigned long value);

  unsigned long get_jointSphericalSphereFlags() const;
  void set_jointSphericalSphereFlags(unsigned long value);

};

typedef ValuePtr<SPHJ> SPHJPtr;
typedef ValueRef<SPHJ> SPHJRef;



} //namespace PHY
} //namespace DAO
