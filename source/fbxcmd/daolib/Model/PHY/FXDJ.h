
/**********************************************************************
*<
FILE: FXDJ.h

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

class FXDJ
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  FXDJ(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

};

typedef ValuePtr<FXDJ> FXDJPtr;
typedef ValueRef<FXDJ> FXDJRef;



} //namespace PHY
} //namespace DAO
