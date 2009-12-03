
/**********************************************************************
*<
FILE: PIPJ.h

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

class PIPJ
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  PIPJ(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

};

typedef ValuePtr<PIPJ> PIPJPtr;
typedef ValueRef<PIPJ> PIPJRef;



} //namespace PHY
} //namespace DAO
