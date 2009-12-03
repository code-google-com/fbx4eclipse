
/**********************************************************************
*<
FILE: NRFL.h

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

class NRFL
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  NRFL(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_lightProbeEnvmap() const;
  void set_lightProbeEnvmap(const Text& value);

};

typedef ValuePtr<NRFL> NRFLPtr;
typedef ValueRef<NRFL> NRFLRef;



} //namespace MMH
} //namespace DAO
