
/**********************************************************************
*<
FILE: AMAP.h

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

class AMAP
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  AMAP(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  unsigned long get_nodeAgeMapCount() const;
  void set_nodeAgeMapCount(unsigned long value);

  GFFListRef get_children() const;

};

typedef ValuePtr<AMAP> AMAPPtr;
typedef ValueRef<AMAP> AMAPRef;



} //namespace MMH
} //namespace DAO
