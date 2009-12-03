
/**********************************************************************
*<
FILE: SNAP.h

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

class SNAP
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  SNAP(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_name() const;
  void set_name(const Text& value);

  GFFListRef get_children() const;

};

typedef ValuePtr<SNAP> SNAPPtr;
typedef ValueRef<SNAP> SNAPRef;



} //namespace MMH
} //namespace DAO
