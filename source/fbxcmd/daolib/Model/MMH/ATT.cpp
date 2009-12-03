
/**********************************************************************
*<
FILE: ATT.h

DESCRIPTION:	MMH File Format

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include "GFF/GFFCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"
#include "MMH/MMHCommon.h"
#include "MMH/ATT.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString ATT::type("ATT");ATT::ATT(GFFStructRef owner) : impl(owner) {
}

unsigned long ATT::get_clothAttachmentType() const {
  return impl->GetField(GFF_MMH_CLOTH_ATTACHMENT_TYPE)->asUInt32();
}

void ATT::set_clothAttachmentType(unsigned long value) {
  impl->GetField(GFF_MMH_CLOTH_ATTACHMENT_TYPE)->assign(value);
}

unsigned char ATT::get_clothAttachmentFlagTwoWayAttachment() const {
  return impl->GetField(GFF_MMH_CLOTH_ATTACHMENT_FLAG_TWO_WAY_ATTACHMENT)->asUInt8();
}

void ATT::set_clothAttachmentFlagTwoWayAttachment(unsigned char value) {
  impl->GetField(GFF_MMH_CLOTH_ATTACHMENT_FLAG_TWO_WAY_ATTACHMENT)->assign(value);
}

unsigned char ATT::get_clothAttachmentFlagTearableAttachment() const {
  return impl->GetField(GFF_MMH_CLOTH_ATTACHMENT_FLAG_TEARABLE_ATTACHMENT)->asUInt8();
}

void ATT::set_clothAttachmentFlagTearableAttachment(unsigned char value) {
  impl->GetField(GFF_MMH_CLOTH_ATTACHMENT_FLAG_TEARABLE_ATTACHMENT)->assign(value);
}

Text ATT::get_clothAttachmentShapeName() const {
  return impl->GetField(GFF_MMH_CLOTH_ATTACHMENT_SHAPE_NAME)->asECString();
}

void ATT::set_clothAttachmentShapeName(const Text& value) {
  impl->GetField(GFF_MMH_CLOTH_ATTACHMENT_SHAPE_NAME)->assign(value);
}

unsigned long ATT::get_clothAttachmentVertexId() const {
  return impl->GetField(GFF_MMH_CLOTH_ATTACHMENT_VERTEX_ID)->asUInt32();
}

void ATT::set_clothAttachmentVertexId(unsigned long value) {
  impl->GetField(GFF_MMH_CLOTH_ATTACHMENT_VERTEX_ID)->assign(value);
}

Vector3f ATT::get_clothAttachmentLocalPos() const {
  return impl->GetField(GFF_MMH_CLOTH_ATTACHMENT_LOCAL_POS)->asVector3f();
}

void ATT::set_clothAttachmentLocalPos(Vector3f value) {
  impl->GetField(GFF_MMH_CLOTH_ATTACHMENT_LOCAL_POS)->assign(value);
}

