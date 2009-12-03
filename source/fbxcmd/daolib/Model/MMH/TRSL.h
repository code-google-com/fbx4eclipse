
/**********************************************************************
*<
FILE: TRSL.h

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

class TRSL
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  TRSL(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Vector4f get_translation() const;
  void set_translation(Vector4f value);

};

typedef ValuePtr<TRSL> TRSLPtr;
typedef ValueRef<TRSL> TRSLRef;



} //namespace MMH
} //namespace DAO
