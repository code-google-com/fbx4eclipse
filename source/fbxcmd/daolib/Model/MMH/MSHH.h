
/**********************************************************************
*<
FILE: MSHH.h

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

class MSHH
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  MSHH(GFFStructRef owner);

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

  Text get_meshGroupName() const;
  void set_meshGroupName(const Text& value);

  unsigned char get_meshCastRuntimeShadow() const;
  void set_meshCastRuntimeShadow(unsigned char value);

  unsigned char get_meshCastBakedShadow() const;
  void set_meshCastBakedShadow(unsigned char value);

  unsigned char get_meshCutAway() const;
  void set_meshCutAway(unsigned char value);

  unsigned char get_meshPunchThrough() const;
  void set_meshPunchThrough(unsigned char value);

  GFFListRef get_meshBonesUsed() const;

  unsigned char get_meshReceiveBakedShadow() const;
  void set_meshReceiveBakedShadow(unsigned char value);

  unsigned char get_meshReceiveRuntimeShadow() const;
  void set_meshReceiveRuntimeShadow(unsigned char value);

  Text get_nodeMeshName() const;
  void set_nodeMeshName(const Text& value);

  unsigned char get_meshIsVfxMesh() const;
  void set_meshIsVfxMesh(unsigned char value);

  Color4 get_meshMaterialColor() const;
  void set_meshMaterialColor(Color4 value);

  unsigned char get_useVariationTint() const;
  void set_useVariationTint(unsigned char value);

  GFFListRef get_children() const;

};

typedef ValuePtr<MSHH> MSHHPtr;
typedef ValueRef<MSHH> MSHHRef;



} //namespace MMH
} //namespace DAO
