
/**********************************************************************
*<
FILE: BBOX.h

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

class BBOX
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  BBOX(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Vector4f get_boundingBoxMin() const;
  void set_boundingBoxMin(Vector4f value);

  Vector4f get_boundingBoxMax() const;
  void set_boundingBoxMax(Vector4f value);

};

typedef ValuePtr<BBOX> BBOXPtr;
typedef ValueRef<BBOX> BBOXRef;



} //namespace MMH
} //namespace DAO
