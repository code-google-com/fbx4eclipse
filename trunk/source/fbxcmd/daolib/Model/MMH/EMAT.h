
/**********************************************************************
*<
FILE: EMAT.h

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

class EMAT
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  EMAT(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  unsigned long get_emitterEmitterAttachmentType() const;
  void set_emitterEmitterAttachmentType(unsigned long value);

  Text get_emitterEmitterAttachmentName() const;
  void set_emitterEmitterAttachmentName(const Text& value);

  unsigned char get_emitterEmitterAttachmentSpawnOnSurface() const;
  void set_emitterEmitterAttachmentSpawnOnSurface(unsigned char value);

  unsigned char get_emitterEmitterAttachmentUseNormalForVelocity() const;
  void set_emitterEmitterAttachmentUseNormalForVelocity(unsigned char value);

};

typedef ValuePtr<EMAT> EMATPtr;
typedef ValueRef<EMAT> EMATRef;



} //namespace MMH
} //namespace DAO
