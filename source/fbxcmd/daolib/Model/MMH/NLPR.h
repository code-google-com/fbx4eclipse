
/**********************************************************************
*<
FILE: NLPR.h

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

class NLPR
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  NLPR(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_name() const;
  void set_name(const Text& value);

  GFFListRef get_children() const;

};

typedef ValuePtr<NLPR> NLPRPtr;
typedef ValueRef<NLPR> NLPRRef;



} //namespace MMH
} //namespace DAO
