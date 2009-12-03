
/**********************************************************************
*<
FILE: FILE.h

DESCRIPTION:	FDL File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once

#include "FDL/FDLCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"

namespace DAO {

using namespace GFF;
namespace FDL {
///////////////////////////////////////////////////////////////////

class FILE
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  FILE(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_resref() const;
  void set_resref(const Text& value);

  GFFListRef get_dependencyList() const;

};

typedef ValuePtr<FILE> FILEPtr;
typedef ValueRef<FILE> FILERef;



} //namespace FDL
} //namespace DAO
