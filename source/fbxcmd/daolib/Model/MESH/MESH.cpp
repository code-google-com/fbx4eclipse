
/**********************************************************************
*<
FILE: MESH.h

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
#include "MESH/MESH.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
///////////////////////////////////////////////////////////////////

ShortString DAO::MESH::MESH::type("MESH");

DAO::MESH::MESH::MESH(GFFStructRef owner) : impl(owner) {
}

Text DAO::MESH::MESH::get_name() const {
  return impl->GetField(GFF_NAME)->asECString();
}

void DAO::MESH::MESH::set_name(const Text& value) {
  impl->GetField(GFF_NAME)->assign(value);
}

GFFListRef DAO::MESH::MESH::get_chunks() const {
  return impl->GetField(GFF_MESH_CHUNKS)->asList();
}

GFFListRef DAO::MESH::MESH::get_vertexdata() const {
  return impl->GetField(GFF_MESH_VERTEXDATA)->asList();
}

GFFListRef DAO::MESH::MESH::get_indexdata() const {
  return impl->GetField(GFF_MESH_INDEXDATA)->asList();
}

unsigned long DAO::MESH::MESH::get_indexformat() const {
  return impl->GetField(GFF_MESH_INDEXFORMAT)->asUInt32();
}

void DAO::MESH::MESH::set_indexformat(unsigned long value) {
  impl->GetField(GFF_MESH_INDEXFORMAT)->assign(value);
}

unsigned char DAO::MESH::MESH::get_instancedStream() const {
  return impl->GetField(GFF_MESH_INSTANCED_STREAM)->asUInt8();
}

void DAO::MESH::MESH::set_instancedStream(unsigned char value) {
  impl->GetField(GFF_MESH_INSTANCED_STREAM)->assign(value);
}

