
/**********************************************************************
*<
FILE: NODE.h

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

class NODE
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  NODE(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_name() const;
  void set_name(const Text& value);

  long get_boneIndex() const;
  void set_boneIndex(long value);

  unsigned char get_nodeSoundMaterial() const;
  void set_nodeSoundMaterial(unsigned char value);

  GFFListRef get_children() const;

  //////////////////////////////////////////////////////////////////////////
  
  Vector4f GetLocalTranslation() const;
  Quaternion GetLocalRotation() const;
  float GetLocalScale() const;
  Matrix43 GetLocalTransform() const;
};

typedef ValuePtr<NODE> NODEPtr;
typedef ValueRef<NODE> NODERef;



} //namespace MMH
} //namespace DAO
