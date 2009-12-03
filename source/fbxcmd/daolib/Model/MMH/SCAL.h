
/**********************************************************************
*<
FILE: SCAL.h

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

class SCAL
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  SCAL(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  float get_scale() const;
  void set_scale(float value);

};

typedef ValuePtr<SCAL> SCALPtr;
typedef ValueRef<SCAL> SCALRef;



} //namespace MMH
} //namespace DAO
