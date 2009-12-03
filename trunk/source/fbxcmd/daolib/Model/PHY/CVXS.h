
/**********************************************************************
*<
FILE: CVXS.h

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

class CVXS
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  CVXS(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  GFFListRef get_shapeCookedDataStream() const;

};

typedef ValuePtr<CVXS> CVXSPtr;
typedef ValueRef<CVXS> CVXSRef;



} //namespace PHY
} //namespace DAO
