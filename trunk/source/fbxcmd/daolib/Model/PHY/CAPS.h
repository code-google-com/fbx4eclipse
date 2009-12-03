
/**********************************************************************
*<
FILE: CAPS.h

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

class CAPS
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  CAPS(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  float get_shapeRadius() const;
  void set_shapeRadius(float value);

  float get_shapeHeight() const;
  void set_shapeHeight(float value);

};

typedef ValuePtr<CAPS> CAPSPtr;
typedef ValueRef<CAPS> CAPSRef;



} //namespace PHY
} //namespace DAO
