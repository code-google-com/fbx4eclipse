/**********************************************************************
*<
FILE: STRM.h

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

class STRM
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  STRM(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  unsigned long get_Vertexsize() const;
  void set_Vertexsize(unsigned long value);

  unsigned long get_Vertexcount() const;
  void set_Vertexcount(unsigned long value);

  unsigned long get_Frequency() const;
  void set_Frequency(unsigned long value);

  unsigned char get_Looping() const;
  void set_Looping(unsigned char value);

  unsigned char get_Instanced() const;
  void set_Instanced(unsigned char value);

  GFFListRef get_Vertexdata() const;

};

typedef ValuePtr<STRM> STRMPtr;
typedef ValueRef<STRM> STRMRef;



} //namespace MESH
} //namespace DAO
