
/**********************************************************************
*<
FILE: EMTG.h

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

class EMTG
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  EMTG(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_name() const;
  void set_name(const Text& value);

  GFFListRef get_children() const;

};

typedef ValuePtr<EMTG> EMTGPtr;
typedef ValueRef<EMTG> EMTGRef;



} //namespace MMH
} //namespace DAO
