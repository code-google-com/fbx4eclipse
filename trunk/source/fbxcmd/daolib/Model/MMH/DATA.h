
/**********************************************************************
*<
FILE: DATA.h

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

class DATA
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  DATA(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  GFFListRef get_16List() const;

  GFFListRef get_or3FList() const;

  unsigned long get_dataSemantic() const;
  void set_dataSemantic(unsigned long value);

  unsigned char get_dataIsIndexStream() const;
  void set_dataIsIndexStream(unsigned char value);

  unsigned long get_dataType() const;
  void set_dataType(unsigned long value);

  unsigned char get_dataLooping() const;
  void set_dataLooping(unsigned char value);

  unsigned char get_dataInstanced() const;
  void set_dataInstanced(unsigned char value);

  unsigned long get_dataCount() const;
  void set_dataCount(unsigned long value);

  unsigned long get_dataPrimitiveType() const;
  void set_dataPrimitiveType(unsigned long value);

  unsigned long get_dataFrequency() const;
  void set_dataFrequency(unsigned long value);

};

typedef ValuePtr<DATA> DATAPtr;
typedef ValueRef<DATA> DATARef;



} //namespace MMH
} //namespace DAO
