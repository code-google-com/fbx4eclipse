
/**********************************************************************
*<
FILE: PRSJ.h

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

class PRSJ
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  PRSJ(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

};

typedef ValuePtr<PRSJ> PRSJPtr;
typedef ValueRef<PRSJ> PRSJRef;



} //namespace PHY
} //namespace DAO
