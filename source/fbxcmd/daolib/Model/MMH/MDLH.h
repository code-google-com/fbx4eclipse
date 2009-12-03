
/**********************************************************************
*<
FILE: MDLH.h

DESCRIPTION:	MMH File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once

#include "MMH/MMHCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"
#include "MMH/NODE.h"
namespace DAO {

using namespace GFF;
namespace MMH {
///////////////////////////////////////////////////////////////////

class MDLH
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  MDLH(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_name() const;
  void set_name(const Text& value);

  Text get_modelHierarchyModelDataName() const;
  void set_modelHierarchyModelDataName(const Text& value);

  Text get_facialAnimationBlueprintName() const;
  void set_facialAnimationBlueprintName(const Text& value);

  unsigned long get_totalBones() const;
  void set_totalBones(unsigned long value);

  unsigned long get_totalExports() const;
  void set_totalExports(unsigned long value);

  GFFListRef get_modelMeshNameList() const;

  GFFListRef get_children() const;

  DAOArray< NODEPtr > get_Nodes() {
	  DAOArray< NODEPtr > retval;
	  DynamicCast<NODE>( get_children(), retval );
	  return retval;	  
  }

};

typedef ValuePtr<MDLH> MDLHPtr;
typedef ValueRef<MDLH> MDLHRef;



} //namespace MMH
} //namespace DAO
