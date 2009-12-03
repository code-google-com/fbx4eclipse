
/**********************************************************************
*<
FILE: CHNK.h

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

class CHNK
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  CHNK(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_name() const;
  void set_name(const Text& value);

  unsigned long get_Vertexsize() const;
  void set_Vertexsize(unsigned long value);

  unsigned long get_Vertexcount() const;
  void set_Vertexcount(unsigned long value);

  unsigned long get_Indexcount() const;
  void set_Indexcount(unsigned long value);

  unsigned long get_Primitivetype() const;
  void set_Primitivetype(unsigned long value);

  unsigned long get_Indexformat() const;
  void set_Indexformat(unsigned long value);

  unsigned long get_Basevertexindex() const;
  void set_Basevertexindex(unsigned long value);

  unsigned long get_Vertexoffset() const;
  void set_Vertexoffset(unsigned long value);

  unsigned long get_Minindex() const;
  void set_Minindex(unsigned long value);

  unsigned long get_Verticesreferenced() const;
  void set_Verticesreferenced(unsigned long value);

  unsigned long get_Startindex() const;
  void set_Startindex(unsigned long value);

  GFFListRef get_Additionalstreams() const;

  GFFStructRef get_Bounds() const;

  GFFListRef get_Vertexdeclarator() const;

  unsigned long get_InstancesCount() const;
  void set_InstancesCount(unsigned long value);

};

typedef ValuePtr<CHNK> CHNKPtr;
typedef ValueRef<CHNK> CHNKRef;



} //namespace MESH
} //namespace DAO
