
/**********************************************************************
*<
FILE: NTRN.h

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

class NTRN
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  NTRN(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_name() const;
  void set_name(const Text& value);

  Text get_materialObject() const;
  void set_materialObject(const Text& value);

  Text get_materialLibrary() const;
  void set_materialLibrary(const Text& value);

  Text get_id() const;
  void set_id(const Text& value);

  GFFListRef get_children() const;

};

typedef ValuePtr<NTRN> NTRNPtr;
typedef ValueRef<NTRN> NTRNRef;



} //namespace MMH
} //namespace DAO
