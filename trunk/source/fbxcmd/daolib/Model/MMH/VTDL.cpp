
/**********************************************************************
*<
FILE: VTDL.h

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
#include "MMH/VTDL.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString VTDL::type("VTDL");VTDL::VTDL(GFFStructRef owner) : impl(owner) {
}

long VTDL::get_Stream() const {
  return impl->GetField(GFF_MESH_VERTEXDECLARATOR_STREAM)->asInt32();
}

void VTDL::set_Stream(long value) {
  impl->GetField(GFF_MESH_VERTEXDECLARATOR_STREAM)->assign(value);
}

long VTDL::get_Offset() const {
  return impl->GetField(GFF_MESH_VERTEXDECLARATOR_OFFSET)->asInt32();
}

void VTDL::set_Offset(long value) {
  impl->GetField(GFF_MESH_VERTEXDECLARATOR_OFFSET)->assign(value);
}

unsigned long VTDL::get_Datatype() const {
  return impl->GetField(GFF_MESH_VERTEXDECLARATOR_DATATYPE)->asUInt32();
}

void VTDL::set_Datatype(unsigned long value) {
  impl->GetField(GFF_MESH_VERTEXDECLARATOR_DATATYPE)->assign(value);
}

unsigned long VTDL::get_Usage() const {
  return impl->GetField(GFF_MESH_VERTEXDECLARATOR_USAGE)->asUInt32();
}

void VTDL::set_Usage(unsigned long value) {
  impl->GetField(GFF_MESH_VERTEXDECLARATOR_USAGE)->assign(value);
}

unsigned long VTDL::get_Usageindex() const {
  return impl->GetField(GFF_MESH_VERTEXDECLARATOR_USAGEINDEX)->asUInt32();
}

void VTDL::set_Usageindex(unsigned long value) {
  impl->GetField(GFF_MESH_VERTEXDECLARATOR_USAGEINDEX)->assign(value);
}

unsigned long VTDL::get_Method() const {
  return impl->GetField(GFF_MESH_VERTEXDECLARATOR_METHOD)->asUInt32();
}

void VTDL::set_Method(unsigned long value) {
  impl->GetField(GFF_MESH_VERTEXDECLARATOR_METHOD)->assign(value);
}

