
/**********************************************************************
*<
FILE: CATL.h

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

class CATL
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  CATL(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  GFFListRef get_children() const;

};

typedef ValuePtr<CATL> CATLPtr;
typedef ValueRef<CATL> CATLRef;



} //namespace MMH
} //namespace DAO
