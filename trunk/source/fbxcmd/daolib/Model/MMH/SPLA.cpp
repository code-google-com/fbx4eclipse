
/**********************************************************************
*<
FILE: SPLA.h

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
#include "MMH/SPLA.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString SPLA::type("SPLA");SPLA::SPLA(GFFStructRef owner) : impl(owner) {
}

float SPLA::get_nodeEmitterSplatparamsWidth() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_WIDTH)->asFloat32();
}

void SPLA::set_nodeEmitterSplatparamsWidth(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_WIDTH)->assign(value);
}

float SPLA::get_nodeEmitterSplatparamsHeight() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_HEIGHT)->asFloat32();
}

void SPLA::set_nodeEmitterSplatparamsHeight(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_HEIGHT)->assign(value);
}

unsigned long SPLA::get_nodeEmitterSplatparamsNumsamplesWidth() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_NUMSAMPLES_WIDTH)->asUInt32();
}

void SPLA::set_nodeEmitterSplatparamsNumsamplesWidth(unsigned long value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_NUMSAMPLES_WIDTH)->assign(value);
}

unsigned long SPLA::get_nodeEmitterSplatparamsNumsamplesHeight() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_NUMSAMPLES_HEIGHT)->asUInt32();
}

void SPLA::set_nodeEmitterSplatparamsNumsamplesHeight(unsigned long value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_NUMSAMPLES_HEIGHT)->assign(value);
}

float SPLA::get_nodeEmitterSplatparamsOrientationRange() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_ORIENTATION_RANGE)->asFloat32();
}

void SPLA::set_nodeEmitterSplatparamsOrientationRange(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_ORIENTATION_RANGE)->assign(value);
}

float SPLA::get_nodeEmitterSplatparamsLife() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_LIFE)->asFloat32();
}

void SPLA::set_nodeEmitterSplatparamsLife(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_LIFE)->assign(value);
}

unsigned char SPLA::get_nodeEmitterSplatparamsFlipbookType() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_FLIPBOOK_TYPE)->asUInt8();
}

void SPLA::set_nodeEmitterSplatparamsFlipbookType(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_FLIPBOOK_TYPE)->assign(value);
}

float SPLA::get_nodeEmitterSplatparamsFlipbookFramesPerSecond() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_FLIPBOOK_FRAMES_PER_SECOND)->asFloat32();
}

void SPLA::set_nodeEmitterSplatparamsFlipbookFramesPerSecond(float value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_FLIPBOOK_FRAMES_PER_SECOND)->assign(value);
}

unsigned char SPLA::get_nodeEmitterSplatparamsFlipbookRows() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_FLIPBOOK_ROWS)->asUInt8();
}

void SPLA::set_nodeEmitterSplatparamsFlipbookRows(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_FLIPBOOK_ROWS)->assign(value);
}

unsigned char SPLA::get_nodeEmitterSplatparamsFlipbookColumns() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_FLIPBOOK_COLUMNS)->asUInt8();
}

void SPLA::set_nodeEmitterSplatparamsFlipbookColumns(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_FLIPBOOK_COLUMNS)->assign(value);
}

unsigned char SPLA::get_nodeEmitterSplatparamsFlipbookRandomStartFrame() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_FLIPBOOK_RANDOM_START_FRAME)->asUInt8();
}

void SPLA::set_nodeEmitterSplatparamsFlipbookRandomStartFrame(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_FLIPBOOK_RANDOM_START_FRAME)->assign(value);
}

Text SPLA::get_nodeEmitterSplatparamsMaterialname() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_MATERIALNAME)->asECString();
}

void SPLA::set_nodeEmitterSplatparamsMaterialname(const Text& value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_MATERIALNAME)->assign(value);
}

unsigned char SPLA::get_nodeEmitterSplatparamsHoldLastFrame() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_HOLD_LAST_FRAME)->asUInt8();
}

void SPLA::set_nodeEmitterSplatparamsHoldLastFrame(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_HOLD_LAST_FRAME)->assign(value);
}

Color4 SPLA::get_nodeEmitterSplatparamsAgemapColorMultiplier() const {
  return impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_AGEMAP_COLOR_MULTIPLIER)->asColor4();
}

void SPLA::set_nodeEmitterSplatparamsAgemapColorMultiplier(Color4 value) {
  impl->GetField(GFF_MMH_NODE_EMITTER_SPLATPARAMS_AGEMAP_COLOR_MULTIPLIER)->assign(value);
}

GFFListRef SPLA::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}

