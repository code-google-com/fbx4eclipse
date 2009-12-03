
/**********************************************************************
*<
FILE: EMAS.h

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

class EMAS
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  EMAS(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  GFFListRef get_children() const;

};

typedef ValuePtr<EMAS> EMASPtr;
typedef ValueRef<EMAS> EMASRef;



} //namespace MMH
} //namespace DAO
