
/**********************************************************************
*<
FILE: NEMG.h

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

class NEMG
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  NEMG(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_name() const;
  void set_name(const Text& value);

  Text get_materialObject() const;
  void set_materialObject(const Text& value);

  GFFListRef get_children() const;

};

typedef ValuePtr<NEMG> NEMGPtr;
typedef ValueRef<NEMG> NEMGRef;



} //namespace MMH
} //namespace DAO
