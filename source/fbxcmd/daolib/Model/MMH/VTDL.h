
/**********************************************************************
*<
FILE: VTDL.h

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

class VTDL
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  VTDL(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  long get_Stream() const;
  void set_Stream(long value);

  long get_Offset() const;
  void set_Offset(long value);

  unsigned long get_Datatype() const;
  void set_Datatype(unsigned long value);

  unsigned long get_Usage() const;
  void set_Usage(unsigned long value);

  unsigned long get_Usageindex() const;
  void set_Usageindex(unsigned long value);

  unsigned long get_Method() const;
  void set_Method(unsigned long value);

};

typedef ValuePtr<VTDL> VTDLPtr;
typedef ValueRef<VTDL> VTDLRef;



} //namespace MMH
} //namespace DAO
