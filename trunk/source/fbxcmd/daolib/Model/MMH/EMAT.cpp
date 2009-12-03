
/**********************************************************************
*<
FILE: EMAT.h

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
#include "MMH/EMAT.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString EMAT::type("EMAT");EMAT::EMAT(GFFStructRef owner) : impl(owner) {
}

unsigned long EMAT::get_emitterEmitterAttachmentType() const {
  return impl->GetField(GFF_MMH_EMITTER_EMITTER_ATTACHMENT_TYPE)->asUInt32();
}

void EMAT::set_emitterEmitterAttachmentType(unsigned long value) {
  impl->GetField(GFF_MMH_EMITTER_EMITTER_ATTACHMENT_TYPE)->assign(value);
}

Text EMAT::get_emitterEmitterAttachmentName() const {
  return impl->GetField(GFF_MMH_EMITTER_EMITTER_ATTACHMENT_NAME)->asECString();
}

void EMAT::set_emitterEmitterAttachmentName(const Text& value) {
  impl->GetField(GFF_MMH_EMITTER_EMITTER_ATTACHMENT_NAME)->assign(value);
}

unsigned char EMAT::get_emitterEmitterAttachmentSpawnOnSurface() const {
  return impl->GetField(GFF_MMH_EMITTER_EMITTER_ATTACHMENT_SPAWN_ON_SURFACE)->asUInt8();
}

void EMAT::set_emitterEmitterAttachmentSpawnOnSurface(unsigned char value) {
  impl->GetField(GFF_MMH_EMITTER_EMITTER_ATTACHMENT_SPAWN_ON_SURFACE)->assign(value);
}

unsigned char EMAT::get_emitterEmitterAttachmentUseNormalForVelocity() const {
  return impl->GetField(GFF_MMH_EMITTER_EMITTER_ATTACHMENT_USE_NORMAL_FOR_VELOCITY)->asUInt8();
}

void EMAT::set_emitterEmitterAttachmentUseNormalForVelocity(unsigned char value) {
  impl->GetField(GFF_MMH_EMITTER_EMITTER_ATTACHMENT_USE_NORMAL_FOR_VELOCITY)->assign(value);
}

