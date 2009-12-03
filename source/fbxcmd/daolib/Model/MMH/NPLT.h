
/**********************************************************************
*<
FILE: NPLT.h

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

class NPLT
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  NPLT(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  Text get_name() const;
  void set_name(const Text& value);

  long get_id() const;
  void set_id(long value);

  Color4 get_nodePointLightColor() const;
  void set_nodePointLightColor(Color4 value);

  float get_nodePointLightRadius() const;
  void set_nodePointLightRadius(float value);

  unsigned char get_nodePointLightIsStatic() const;
  void set_nodePointLightIsStatic(unsigned char value);

  float get_nodePointLightIntensityVariation() const;
  void set_nodePointLightIntensityVariation(float value);

  float get_nodePointLightIntensityPeriod() const;
  void set_nodePointLightIntensityPeriod(float value);

  float get_nodePointLightIntensityPeriodDelta() const;
  void set_nodePointLightIntensityPeriodDelta(float value);

  unsigned char get_nodeLightAffectDomain() const;
  void set_nodeLightAffectDomain(unsigned char value);

  unsigned char get_lightCanBeOccluded() const;
  void set_lightCanBeOccluded(unsigned char value);

  unsigned char get_useVariationTint() const;
  void set_useVariationTint(unsigned char value);

  unsigned char get_nodeLightVersion() const;
  void set_nodeLightVersion(unsigned char value);

  GFFListRef get_children() const;

};

typedef ValuePtr<NPLT> NPLTPtr;
typedef ValueRef<NPLT> NPLTRef;



} //namespace MMH
} //namespace DAO
