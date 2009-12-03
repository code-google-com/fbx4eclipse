
/**********************************************************************
*<
FILE: JNT.h

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

class JNT
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  JNT(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_jointTarget() const;
  void set_jointTarget(const Text& value);

  unsigned long get_jointType() const;
  void set_jointType(unsigned long value);

  GFFFieldRef get_jointTypeStruct() const;

  Vector3f get_jointLocalNormal1() const;
  void set_jointLocalNormal1(Vector3f value);

  Vector3f get_jointLocalNormal2() const;
  void set_jointLocalNormal2(Vector3f value);

  Vector3f get_jointLocalAnchor1() const;
  void set_jointLocalAnchor1(Vector3f value);

  Vector3f get_jointLocalAnchor2() const;
  void set_jointLocalAnchor2(Vector3f value);

  Vector3f get_jointLocalAxis1() const;
  void set_jointLocalAxis1(Vector3f value);

  Vector3f get_jointLocalAxis2() const;
  void set_jointLocalAxis2(Vector3f value);

  float get_jointMaxForce() const;
  void set_jointMaxForce(float value);

  float get_jointMaxTorque() const;
  void set_jointMaxTorque(float value);

  unsigned char get_jointCollisionEnabled() const;
  void set_jointCollisionEnabled(unsigned char value);

};

typedef ValuePtr<JNT> JNTPtr;
typedef ValueRef<JNT> JNTRef;



} //namespace PHY
} //namespace DAO
