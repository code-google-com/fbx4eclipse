
/**********************************************************************
*<
FILE: BNDS.h

DESCRIPTION:	MMH File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once

#include "MMH/MMHCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"

namespace DAO {

using namespace GFF;
namespace MMH {
///////////////////////////////////////////////////////////////////

class BNDS
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  BNDS(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Vector4f get_boundsBoxmin() const;
  void set_boundsBoxmin(Vector4f value);

  Vector4f get_boundsBoxmax() const;
  void set_boundsBoxmax(Vector4f value);

  Vector4f get_boundsSphere() const;
  void set_boundsSphere(Vector4f value);

};

typedef ValuePtr<BNDS> BNDSPtr;
typedef ValueRef<BNDS> BNDSRef;



} //namespace MMH
} //namespace DAO
