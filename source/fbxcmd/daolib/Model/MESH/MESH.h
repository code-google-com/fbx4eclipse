
/**********************************************************************
*<
FILE: MESH.h

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

class MESH
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  MESH(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_name() const;
  void set_name(const Text& value);

  GFFListRef get_chunks() const;

  GFFListRef get_vertexdata() const;

  GFFListRef get_indexdata() const;

  unsigned long get_indexformat() const;
  void set_indexformat(unsigned long value);

  unsigned char get_instancedStream() const;
  void set_instancedStream(unsigned char value);

};

typedef ValuePtr<MESH> MESHPtr;
typedef ValueRef<MESH> MESHRef;



} //namespace MESH
} //namespace DAO
