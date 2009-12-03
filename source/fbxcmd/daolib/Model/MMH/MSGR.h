
/**********************************************************************
*<
FILE: MSGR.h

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

class MSGR
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  MSGR(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_name() const;
  void set_name(const Text& value);

  unsigned long get_chunkVertexsize() const;
  void set_chunkVertexsize(unsigned long value);

  unsigned long get_chunkVertexcount() const;
  void set_chunkVertexcount(unsigned long value);

  unsigned long get_chunkIndexcount() const;
  void set_chunkIndexcount(unsigned long value);

  unsigned long get_chunkPrimitivetype() const;
  void set_chunkPrimitivetype(unsigned long value);

  unsigned long get_chunkIndexformat() const;
  void set_chunkIndexformat(unsigned long value);

  GFFStructRef get_chunkBounds() const;

  GFFListRef get_vertexdata() const;

  GFFListRef get_indexdata() const;

  GFFListRef get_chunkVertexdeclarator() const;

};

typedef ValuePtr<MSGR> MSGRPtr;
typedef ValueRef<MSGR> MSGRRef;



} //namespace MMH
} //namespace DAO
