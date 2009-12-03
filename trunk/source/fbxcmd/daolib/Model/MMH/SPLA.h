
/**********************************************************************
*<
FILE: SPLA.h

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

class SPLA
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  SPLA(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  float get_nodeEmitterSplatparamsWidth() const;
  void set_nodeEmitterSplatparamsWidth(float value);

  float get_nodeEmitterSplatparamsHeight() const;
  void set_nodeEmitterSplatparamsHeight(float value);

  unsigned long get_nodeEmitterSplatparamsNumsamplesWidth() const;
  void set_nodeEmitterSplatparamsNumsamplesWidth(unsigned long value);

  unsigned long get_nodeEmitterSplatparamsNumsamplesHeight() const;
  void set_nodeEmitterSplatparamsNumsamplesHeight(unsigned long value);

  float get_nodeEmitterSplatparamsOrientationRange() const;
  void set_nodeEmitterSplatparamsOrientationRange(float value);

  float get_nodeEmitterSplatparamsLife() const;
  void set_nodeEmitterSplatparamsLife(float value);

  unsigned char get_nodeEmitterSplatparamsFlipbookType() const;
  void set_nodeEmitterSplatparamsFlipbookType(unsigned char value);

  float get_nodeEmitterSplatparamsFlipbookFramesPerSecond() const;
  void set_nodeEmitterSplatparamsFlipbookFramesPerSecond(float value);

  unsigned char get_nodeEmitterSplatparamsFlipbookRows() const;
  void set_nodeEmitterSplatparamsFlipbookRows(unsigned char value);

  unsigned char get_nodeEmitterSplatparamsFlipbookColumns() const;
  void set_nodeEmitterSplatparamsFlipbookColumns(unsigned char value);

  unsigned char get_nodeEmitterSplatparamsFlipbookRandomStartFrame() const;
  void set_nodeEmitterSplatparamsFlipbookRandomStartFrame(unsigned char value);

  Text get_nodeEmitterSplatparamsMaterialname() const;
  void set_nodeEmitterSplatparamsMaterialname(const Text& value);

  unsigned char get_nodeEmitterSplatparamsHoldLastFrame() const;
  void set_nodeEmitterSplatparamsHoldLastFrame(unsigned char value);

  Color4 get_nodeEmitterSplatparamsAgemapColorMultiplier() const;
  void set_nodeEmitterSplatparamsAgemapColorMultiplier(Color4 value);

  GFFListRef get_children() const;

};

typedef ValuePtr<SPLA> SPLAPtr;
typedef ValueRef<SPLA> SPLARef;



} //namespace MMH
} //namespace DAO
