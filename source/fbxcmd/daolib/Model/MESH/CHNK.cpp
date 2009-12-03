
/**********************************************************************
*<
FILE: CHNK.h

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
#include "MESH/CHNK.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MESH;
///////////////////////////////////////////////////////////////////

ShortString CHNK::type("CHNK");CHNK::CHNK(GFFStructRef owner) : impl(owner) {
}

Text CHNK::get_name() const {
  return impl->GetField(GFF_NAME)->asECString();
}

void CHNK::set_name(const Text& value) {
  impl->GetField(GFF_NAME)->assign(value);
}

unsigned long CHNK::get_Vertexsize() const {
  return impl->GetField(GFF_MESH_CHUNK_VERTEXSIZE)->asUInt32();
}

void CHNK::set_Vertexsize(unsigned long value) {
  impl->GetField(GFF_MESH_CHUNK_VERTEXSIZE)->assign(value);
}

unsigned long CHNK::get_Vertexcount() const {
  return impl->GetField(GFF_MESH_CHUNK_VERTEXCOUNT)->asUInt32();
}

void CHNK::set_Vertexcount(unsigned long value) {
  impl->GetField(GFF_MESH_CHUNK_VERTEXCOUNT)->assign(value);
}

unsigned long CHNK::get_Indexcount() const {
  return impl->GetField(GFF_MESH_CHUNK_INDEXCOUNT)->asUInt32();
}

void CHNK::set_Indexcount(unsigned long value) {
  impl->GetField(GFF_MESH_CHUNK_INDEXCOUNT)->assign(value);
}

unsigned long CHNK::get_Primitivetype() const {
  return impl->GetField(GFF_MESH_CHUNK_PRIMITIVETYPE)->asUInt32();
}

void CHNK::set_Primitivetype(unsigned long value) {
  impl->GetField(GFF_MESH_CHUNK_PRIMITIVETYPE)->assign(value);
}

unsigned long CHNK::get_Indexformat() const {
  return impl->GetField(GFF_MESH_CHUNK_INDEXFORMAT)->asUInt32();
}

void CHNK::set_Indexformat(unsigned long value) {
  impl->GetField(GFF_MESH_CHUNK_INDEXFORMAT)->assign(value);
}

unsigned long CHNK::get_Basevertexindex() const {
  return impl->GetField(GFF_MESH_CHUNK_BASEVERTEXINDEX)->asUInt32();
}

void CHNK::set_Basevertexindex(unsigned long value) {
  impl->GetField(GFF_MESH_CHUNK_BASEVERTEXINDEX)->assign(value);
}

unsigned long CHNK::get_Vertexoffset() const {
  return impl->GetField(GFF_MESH_CHUNK_VERTEXOFFSET)->asUInt32();
}

void CHNK::set_Vertexoffset(unsigned long value) {
  impl->GetField(GFF_MESH_CHUNK_VERTEXOFFSET)->assign(value);
}

unsigned long CHNK::get_Minindex() const {
  return impl->GetField(GFF_MESH_CHUNK_MININDEX)->asUInt32();
}

void CHNK::set_Minindex(unsigned long value) {
  impl->GetField(GFF_MESH_CHUNK_MININDEX)->assign(value);
}

unsigned long CHNK::get_Verticesreferenced() const {
  return impl->GetField(GFF_MESH_CHUNK_VERTICESREFERENCED)->asUInt32();
}

void CHNK::set_Verticesreferenced(unsigned long value) {
  impl->GetField(GFF_MESH_CHUNK_VERTICESREFERENCED)->assign(value);
}

unsigned long CHNK::get_Startindex() const {
  return impl->GetField(GFF_MESH_CHUNK_STARTINDEX)->asUInt32();
}

void CHNK::set_Startindex(unsigned long value) {
  impl->GetField(GFF_MESH_CHUNK_STARTINDEX)->assign(value);
}

GFFListRef CHNK::get_Additionalstreams() const {
  return impl->GetField(GFF_MESH_CHUNK_ADDITIONALSTREAMS)->asList();
}

GFFStructRef CHNK::get_Bounds() const {
  return impl->GetField(GFF_MESH_CHUNK_BOUNDS)->asStruct();
}

GFFListRef CHNK::get_Vertexdeclarator() const {
  return impl->GetField(GFF_MESH_CHUNK_VERTEXDECLARATOR)->asList();
}

unsigned long CHNK::get_InstancesCount() const {
  return impl->GetField(GFF_MESH_CHUNK_INSTANCES_COUNT)->asUInt32();
}

void CHNK::set_InstancesCount(unsigned long value) {
  impl->GetField(GFF_MESH_CHUNK_INSTANCES_COUNT)->assign(value);
}

