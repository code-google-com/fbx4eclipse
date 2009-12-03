
/**********************************************************************
*<
FILE: XPRT.h

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

class XPRT
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  XPRT(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_exportExportName() const;
  void set_exportExportName(const Text& value);

  unsigned long get_exportControllerType() const;
  void set_exportControllerType(unsigned long value);

  unsigned long get_exportTagVariableType() const;
  void set_exportTagVariableType(unsigned long value);

  unsigned long get_exportControllerIndex() const;
  void set_exportControllerIndex(unsigned long value);

};

typedef ValuePtr<XPRT> XPRTPtr;
typedef ValueRef<XPRT> XPRTRef;



} //namespace MMH
} //namespace DAO
