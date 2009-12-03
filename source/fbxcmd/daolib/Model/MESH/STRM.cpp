
/**********************************************************************
*<
FILE: STRM.h

DESCRIPTION:	MESH File Format

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include "GFF/GFFCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"
#include "MESH/MESHCommon.h"
#include "MESH/STRM.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MESH;
///////////////////////////////////////////////////////////////////

ShortString STRM::type("STRM");STRM::STRM(GFFStructRef owner) : impl(owner) {
}

unsigned long STRM::get_Vertexsize() const {
  return impl->GetField(GFF_MESH_STREAM_VERTEXSIZE)->asUInt32();
}

void STRM::set_Vertexsize(unsigned long value) {
  impl->GetField(GFF_MESH_STREAM_VERTEXSIZE)->assign(value);
}

unsigned long STRM::get_Vertexcount() const {
  return impl->GetField(GFF_MESH_STREAM_VERTEXCOUNT)->asUInt32();
}

void STRM::set_Vertexcount(unsigned long value) {
  impl->GetField(GFF_MESH_STREAM_VERTEXCOUNT)->assign(value);
}

unsigned long STRM::get_Frequency() const {
  return impl->GetField(GFF_MESH_STREAM_FREQUENCY)->asUInt32();
}

void STRM::set_Frequency(unsigned long value) {
  impl->GetField(GFF_MESH_STREAM_FREQUENCY)->assign(value);
}

unsigned char STRM::get_Looping() const {
  return impl->GetField(GFF_MESH_STREAM_LOOPING)->asUInt8();
}

void STRM::set_Looping(unsigned char value) {
  impl->GetField(GFF_MESH_STREAM_LOOPING)->assign(value);
}

unsigned char STRM::get_Instanced() const {
  return impl->GetField(GFF_MESH_STREAM_INSTANCED)->asUInt8();
}

void STRM::set_Instanced(unsigned char value) {
  impl->GetField(GFF_MESH_STREAM_INSTANCED)->assign(value);
}

GFFListRef STRM::get_Vertexdata() const {
  return impl->GetField(GFF_MESH_STREAM_VERTEXDATA)->asList();
}

