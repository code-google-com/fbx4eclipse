
/**********************************************************************
*<
FILE: MSHS.h

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

class MSHS
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  MSHS(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  unsigned long get_shapeMeshShapeFlags() const;
  void set_shapeMeshShapeFlags(unsigned long value);

  unsigned long get_shapeMeshHeightFieldAxis() const;
  void set_shapeMeshHeightFieldAxis(unsigned long value);

  float get_shapeMeshHeightFieldExtent() const;
  void set_shapeMeshHeightFieldExtent(float value);

  GFFListRef get_shapeCookedDataStream() const;

};

typedef ValuePtr<MSHS> MSHSPtr;
typedef ValueRef<MSHS> MSHSRef;



} //namespace PHY
} //namespace DAO
