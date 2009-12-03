
/**********************************************************************
*<
FILE: MSGR.h

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
#include "MMH/MSGR.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString MSGR::type("MSGR");MSGR::MSGR(GFFStructRef owner) : impl(owner) {
}

Text MSGR::get_name() const {
  return impl->GetField(GFF_MMH_NAME)->asECString();
}

void MSGR::set_name(const Text& value) {
  impl->GetField(GFF_MMH_NAME)->assign(value);
}

unsigned long MSGR::get_chunkVertexsize() const {
  return impl->GetField(GFF_MESH_CHUNK_VERTEXSIZE)->asUInt32();
}

void MSGR::set_chunkVertexsize(unsigned long value) {
  impl->GetField(GFF_MESH_CHUNK_VERTEXSIZE)->assign(value);
}

unsigned long MSGR::get_chunkVertexcount() const {
  return impl->GetField(GFF_MESH_CHUNK_VERTEXCOUNT)->asUInt32();
}

void MSGR::set_chunkVertexcount(unsigned long value) {
  impl->GetField(GFF_MESH_CHUNK_VERTEXCOUNT)->assign(value);
}

unsigned long MSGR::get_chunkIndexcount() const {
  return impl->GetField(GFF_MESH_CHUNK_INDEXCOUNT)->asUInt32();
}

void MSGR::set_chunkIndexcount(unsigned long value) {
  impl->GetField(GFF_MESH_CHUNK_INDEXCOUNT)->assign(value);
}

unsigned long MSGR::get_chunkPrimitivetype() const {
  return impl->GetField(GFF_MESH_CHUNK_PRIMITIVETYPE)->asUInt32();
}

void MSGR::set_chunkPrimitivetype(unsigned long value) {
  impl->GetField(GFF_MESH_CHUNK_PRIMITIVETYPE)->assign(value);
}

unsigned long MSGR::get_chunkIndexformat() const {
  return impl->GetField(GFF_MESH_CHUNK_INDEXFORMAT)->asUInt32();
}

void MSGR::set_chunkIndexformat(unsigned long value) {
  impl->GetField(GFF_MESH_CHUNK_INDEXFORMAT)->assign(value);
}

GFFStructRef MSGR::get_chunkBounds() const {
  return impl->GetField(GFF_MESH_CHUNK_BOUNDS)->asStruct();
}

GFFListRef MSGR::get_vertexdata() const {
  return impl->GetField(GFF_MESH_VERTEXDATA)->asList();
}

GFFListRef MSGR::get_indexdata() const {
  return impl->GetField(GFF_MESH_INDEXDATA)->asList();
}

GFFListRef MSGR::get_chunkVertexdeclarator() const {
  return impl->GetField(GFF_MESH_CHUNK_VERTEXDECLARATOR)->asList();
}

