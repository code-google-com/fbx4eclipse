
/**********************************************************************
*<
FILE: BOXS.h

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

class BOXS
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  BOXS(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Vector3f get_shapeBoxDim() const;
  void set_shapeBoxDim(Vector3f value);

};

typedef ValuePtr<BOXS> BOXSPtr;
typedef ValueRef<BOXS> BOXSRef;



} //namespace PHY
} //namespace DAO
