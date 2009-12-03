
/**********************************************************************
*<
FILE: BNDS.h

DESCRIPTION:	MESH File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once

#include "MESH/MESHCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"

namespace DAO {

using namespace GFF;
namespace MESH {
///////////////////////////////////////////////////////////////////

class BNDS
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  BNDS(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Vector4f get_Boxmin() const;
  void set_Boxmin(Vector4f value);

  Vector4f get_Boxmax() const;
  void set_Boxmax(Vector4f value);

  Vector4f get_Sphere() const;
  void set_Sphere(Vector4f value);

};

typedef ValuePtr<BNDS> BNDSPtr;
typedef ValueRef<BNDS> BNDSRef;



} //namespace MESH
} //namespace DAO
