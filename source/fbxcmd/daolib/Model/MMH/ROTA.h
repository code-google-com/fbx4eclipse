
/**********************************************************************
*<
FILE: ROTA.h

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

class ROTA
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  ROTA(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Quaternion get_rotation() const;
  void set_rotation(Quaternion value);

};

typedef ValuePtr<ROTA> ROTAPtr;
typedef ValueRef<ROTA> ROTARef;



} //namespace MMH
} //namespace DAO
