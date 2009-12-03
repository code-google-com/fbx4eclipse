
/**********************************************************************
*<
FILE: COBJ.h

DESCRIPTION:	PHY File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once

#include "PHY/PHYCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"

namespace DAO {

using namespace GFF;
namespace PHY {
///////////////////////////////////////////////////////////////////

class COBJ
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  COBJ(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  float get_nodeCollisionObjDensity() const;
  void set_nodeCollisionObjDensity(float value);

  unsigned long get_nodeCollisionObjType() const;
  void set_nodeCollisionObjType(unsigned long value);

  unsigned long get_collisionGroup() const;
  void set_collisionGroup(unsigned long value);

  GFFListRef get_children() const;

};

typedef ValuePtr<COBJ> COBJPtr;
typedef ValueRef<COBJ> COBJRef;



} //namespace PHY
} //namespace DAO
