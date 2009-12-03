
/**********************************************************************
*<
FILE: AMEL.h

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

class AMEL
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  AMEL(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  float get_nodeAgeMapElementPercentLifeElapsed() const;
  void set_nodeAgeMapElementPercentLifeElapsed(float value);

  float get_nodeAgeMapElementScaleX() const;
  void set_nodeAgeMapElementScaleX(float value);

  float get_nodeAgeMapElementScaleY() const;
  void set_nodeAgeMapElementScaleY(float value);

  Color4 get_nodeAgeMapElementColor() const;
  void set_nodeAgeMapElementColor(Color4 value);

};

typedef ValuePtr<AMEL> AMELPtr;
typedef ValueRef<AMEL> AMELRef;



} //namespace MMH
} //namespace DAO
