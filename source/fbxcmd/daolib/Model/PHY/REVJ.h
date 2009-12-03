
/**********************************************************************
*<
FILE: REVJ.h

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

class REVJ
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  REVJ(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Vector3f get_jointRevoluteLimitLow() const;
  void set_jointRevoluteLimitLow(Vector3f value);

  Vector3f get_jointRevoluteLimitHigh() const;
  void set_jointRevoluteLimitHigh(Vector3f value);

  float get_jointRevoluteProjectionDistance() const;
  void set_jointRevoluteProjectionDistance(float value);

  float get_jointRevoluteProjectionAngle() const;
  void set_jointRevoluteProjectionAngle(float value);

  unsigned long get_jointRevoluteProjectionMode() const;
  void set_jointRevoluteProjectionMode(unsigned long value);

  Vector3f get_jointRevoluteSpring() const;
  void set_jointRevoluteSpring(Vector3f value);

  float get_jointRevoluteMotorVelTarget() const;
  void set_jointRevoluteMotorVelTarget(float value);

  float get_jointRevoluteMotorMaxForce() const;
  void set_jointRevoluteMotorMaxForce(float value);

  unsigned char get_jointRevoluteMotorFreeSpin() const;
  void set_jointRevoluteMotorFreeSpin(unsigned char value);

  unsigned long get_jointRevoluteRevoluteFlags() const;
  void set_jointRevoluteRevoluteFlags(unsigned long value);

};

typedef ValuePtr<REVJ> REVJPtr;
typedef ValueRef<REVJ> REVJRef;



} //namespace PHY
} //namespace DAO
