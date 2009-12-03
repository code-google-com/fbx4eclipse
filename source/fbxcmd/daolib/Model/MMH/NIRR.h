
/**********************************************************************
*<
FILE: NIRR.h

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

class NIRR
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  NIRR(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_lightprobeIrradianceRes() const;
  void set_lightprobeIrradianceRes(const Text& value);

  Matrix44 get_lightprobeIrradianceRed() const;
  void set_lightprobeIrradianceRed(Matrix44 value);

  Matrix44 get_lightprobeIrradianceGreen() const;
  void set_lightprobeIrradianceGreen(Matrix44 value);

  Matrix44 get_lightprobeIrradianceBlue() const;
  void set_lightprobeIrradianceBlue(Matrix44 value);

};

typedef ValuePtr<NIRR> NIRRPtr;
typedef ValueRef<NIRR> NIRRRef;



} //namespace MMH
} //namespace DAO
