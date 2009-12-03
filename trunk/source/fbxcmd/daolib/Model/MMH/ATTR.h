
/**********************************************************************
*<
FILE: ATTR.h

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

class ATTR
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  ATTR(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_attributeName() const;
  void set_attributeName(const Text& value);

  Text get_attributeSourceName() const;
  void set_attributeSourceName(const Text& value);

};

typedef ValuePtr<ATTR> ATTRPtr;
typedef ValueRef<ATTR> ATTRRef;



} //namespace MMH
} //namespace DAO
