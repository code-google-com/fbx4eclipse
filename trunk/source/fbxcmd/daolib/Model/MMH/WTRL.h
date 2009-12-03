
/**********************************************************************
*<
FILE: WTRL.h

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

class WTRL
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  WTRL(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_name() const;
  void set_name(const Text& value);

  Text get_materialObject() const;
  void set_materialObject(const Text& value);

  float get_weapontrailSegmentLength() const;
  void set_weapontrailSegmentLength(float value);

  float get_weapontrailDuration() const;
  void set_weapontrailDuration(float value);

  unsigned char get_useVariationTint() const;
  void set_useVariationTint(unsigned char value);

  GFFListRef get_children() const;

};

typedef ValuePtr<WTRL> WTRLPtr;
typedef ValueRef<WTRL> WTRLRef;



} //namespace MMH
} //namespace DAO
