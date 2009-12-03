
/**********************************************************************
*<
FILE: CRST.h

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

class CRST
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  CRST(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_name() const;
  void set_name(const Text& value);

  unsigned char get_nodeCrustHookId() const;
  void set_nodeCrustHookId(unsigned char value);

  GFFListRef get_children() const;

};

typedef ValuePtr<CRST> CRSTPtr;
typedef ValueRef<CRST> CRSTRef;



} //namespace MMH
} //namespace DAO
