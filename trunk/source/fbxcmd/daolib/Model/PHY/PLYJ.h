
/**********************************************************************
*<
FILE: PLYJ.h

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

class PLYJ
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  PLYJ(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Vector3f get_jointPulleyPulley1() const;
  void set_jointPulleyPulley1(Vector3f value);

  Vector3f get_jointPulleyPulley2() const;
  void set_jointPulleyPulley2(Vector3f value);

  float get_jointPulleyDistance() const;
  void set_jointPulleyDistance(float value);

  float get_jointPulleyStiffness() const;
  void set_jointPulleyStiffness(float value);

  float get_jointPulleyRatio() const;
  void set_jointPulleyRatio(float value);

  float get_jointPulleyMotorVelTarget() const;
  void set_jointPulleyMotorVelTarget(float value);

  float get_jointPulleyMotorMaxForce() const;
  void set_jointPulleyMotorMaxForce(float value);

  unsigned char get_jointPulleyMotorFreeSpin() const;
  void set_jointPulleyMotorFreeSpin(unsigned char value);

  unsigned long get_jointPulleyPulleyFlags() const;
  void set_jointPulleyPulleyFlags(unsigned long value);

};

typedef ValuePtr<PLYJ> PLYJPtr;
typedef ValueRef<PLYJ> PLYJRef;



} //namespace PHY
} //namespace DAO
