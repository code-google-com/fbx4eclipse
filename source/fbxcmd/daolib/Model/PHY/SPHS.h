
/**********************************************************************
*<
FILE: SPHS.h

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

class SPHS
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  SPHS(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  float get_shapeRadius() const;
  void set_shapeRadius(float value);

};

typedef ValuePtr<SPHS> SPHSPtr;
typedef ValueRef<SPHS> SPHSRef;



} //namespace PHY
} //namespace DAO
