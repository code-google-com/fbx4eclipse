
/**********************************************************************
*<
FILE: NALT.h

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

class NALT
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  NALT(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_name() const;
  void set_name(const Text& value);

  Color4 get_nodeAmbientLightColor() const;
  void set_nodeAmbientLightColor(Color4 value);

  unsigned char get_useVariationTint() const;
  void set_useVariationTint(unsigned char value);

  GFFListRef get_children() const;

};

typedef ValuePtr<NALT> NALTPtr;
typedef ValueRef<NALT> NALTRef;



} //namespace MMH
} //namespace DAO
