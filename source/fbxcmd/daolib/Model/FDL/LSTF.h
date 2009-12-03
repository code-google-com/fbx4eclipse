
/**********************************************************************
*<
FILE: LSTF.h

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

class LSTF
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  LSTF(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  GFFListRef get_fileList() const;

};

typedef ValuePtr<LSTF> LSTFPtr;
typedef ValueRef<LSTF> LSTFRef;



} //namespace FDL
} //namespace DAO
