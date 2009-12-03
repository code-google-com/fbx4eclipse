
/**********************************************************************
*<
FILE: ATT.h

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

class ATT
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  ATT(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  unsigned long get_clothAttachmentType() const;
  void set_clothAttachmentType(unsigned long value);

  unsigned char get_clothAttachmentFlagTwoWayAttachment() const;
  void set_clothAttachmentFlagTwoWayAttachment(unsigned char value);

  unsigned char get_clothAttachmentFlagTearableAttachment() const;
  void set_clothAttachmentFlagTearableAttachment(unsigned char value);

  Text get_clothAttachmentShapeName() const;
  void set_clothAttachmentShapeName(const Text& value);

  unsigned long get_clothAttachmentVertexId() const;
  void set_clothAttachmentVertexId(unsigned long value);

  Vector3f get_clothAttachmentLocalPos() const;
  void set_clothAttachmentLocalPos(Vector3f value);

};

typedef ValuePtr<ATT> ATTPtr;
typedef ValueRef<ATT> ATTRef;



} //namespace MMH
} //namespace DAO
