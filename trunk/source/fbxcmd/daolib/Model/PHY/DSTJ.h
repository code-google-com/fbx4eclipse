
/**********************************************************************
*<
FILE: DSTJ.h

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

class DSTJ
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  DSTJ(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  float get_jointDistanceMinDistance() const;
  void set_jointDistanceMinDistance(float value);

  float get_jointDistanceMaxDistance() const;
  void set_jointDistanceMaxDistance(float value);

  Vector3f get_jointDistanceSpring() const;
  void set_jointDistanceSpring(Vector3f value);

  unsigned long get_jointDistanceDistanceFlags() const;
  void set_jointDistanceDistanceFlags(unsigned long value);

};

typedef ValuePtr<DSTJ> DSTJPtr;
typedef ValueRef<DSTJ> DSTJRef;



} //namespace PHY
} //namespace DAO
