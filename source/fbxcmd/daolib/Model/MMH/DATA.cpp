
/**********************************************************************
*<
FILE: DATA.h

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
#include "MMH/DATA.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString DATA::type("DATA");DATA::DATA(GFFStructRef owner) : impl(owner) {
}

GFFListRef DATA::get_16List() const {
  return impl->GetField(GFF_UINT16_LIST)->asList();
}

GFFListRef DATA::get_or3FList() const {
  return impl->GetField(GFF_VECTOR3F_LIST)->asList();
}

unsigned long DATA::get_dataSemantic() const {
  return impl->GetField(GFF_MMH_DATA_SEMANTIC)->asUInt32();
}

void DATA::set_dataSemantic(unsigned long value) {
  impl->GetField(GFF_MMH_DATA_SEMANTIC)->assign(value);
}

unsigned char DATA::get_dataIsIndexStream() const {
  return impl->GetField(GFF_MMH_DATA_IS_INDEX_STREAM)->asUInt8();
}

void DATA::set_dataIsIndexStream(unsigned char value) {
  impl->GetField(GFF_MMH_DATA_IS_INDEX_STREAM)->assign(value);
}

unsigned long DATA::get_dataType() const {
  return impl->GetField(GFF_MMH_DATA_TYPE)->asUInt32();
}

void DATA::set_dataType(unsigned long value) {
  impl->GetField(GFF_MMH_DATA_TYPE)->assign(value);
}

unsigned char DATA::get_dataLooping() const {
  return impl->GetField(GFF_MMH_DATA_LOOPING)->asUInt8();
}

void DATA::set_dataLooping(unsigned char value) {
  impl->GetField(GFF_MMH_DATA_LOOPING)->assign(value);
}

unsigned char DATA::get_dataInstanced() const {
  return impl->GetField(GFF_MMH_DATA_INSTANCED)->asUInt8();
}

void DATA::set_dataInstanced(unsigned char value) {
  impl->GetField(GFF_MMH_DATA_INSTANCED)->assign(value);
}

unsigned long DATA::get_dataCount() const {
  return impl->GetField(GFF_MMH_DATA_COUNT)->asUInt32();
}

void DATA::set_dataCount(unsigned long value) {
  impl->GetField(GFF_MMH_DATA_COUNT)->assign(value);
}

unsigned long DATA::get_dataPrimitiveType() const {
  return impl->GetField(GFF_MMH_DATA_PRIMITIVE_TYPE)->asUInt32();
}

void DATA::set_dataPrimitiveType(unsigned long value) {
  impl->GetField(GFF_MMH_DATA_PRIMITIVE_TYPE)->assign(value);
}

unsigned long DATA::get_dataFrequency() const {
  return impl->GetField(GFF_MMH_DATA_FREQUENCY)->asUInt32();
}

void DATA::set_dataFrequency(unsigned long value) {
  impl->GetField(GFF_MMH_DATA_FREQUENCY)->assign(value);
}

