
/**********************************************************************
*<
FILE: NODE.h

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
#include "MMH/NODE.h"
#include "MMH/TRSL.h"
#include "MMH/ROTA.h"
#include "MMH/SCAL.h"

using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString NODE::type("NODE");NODE::NODE(GFFStructRef owner) : impl(owner) {
}

Text NODE::get_name() const {
  return impl->GetField(GFF_MMH_NAME)->asECString();
}

void NODE::set_name(const Text& value) {
  impl->GetField(GFF_MMH_NAME)->assign(value);
}

long NODE::get_boneIndex() const {
  return impl->GetField(GFF_MMH_BONE_INDEX)->asInt32();
}

void NODE::set_boneIndex(long value) {
  impl->GetField(GFF_MMH_BONE_INDEX)->assign(value);
}

unsigned char NODE::get_nodeSoundMaterial() const {
  return impl->GetField(GFF_MMH_NODE_SOUND_MATERIAL)->asUInt8();
}

void NODE::set_nodeSoundMaterial(unsigned char value) {
  impl->GetField(GFF_MMH_NODE_SOUND_MATERIAL)->assign(value);
}

GFFListRef NODE::get_children() const {
  return impl->GetField(GFF_MMH_CHILDREN)->asList();
}


Vector4f NODE::GetLocalTranslation() const
{
	if (this == NULL) return Vector4f();
	TRSLPtr val = GetFirstOfType<TRSL>(get_children());
	return (!val.isNull()) ? val->get_translation() : Vector4f();
}

Quaternion NODE::GetLocalRotation() const
{
	if (this == NULL) return Quaternion();
	ROTAPtr val = GetFirstOfType<ROTA>(get_children());
	return (!val.isNull()) ? val->get_rotation() : Quaternion();
}

float NODE::GetLocalScale( ) const
{
	if (this == NULL) return 1.0f;
	SCALPtr val = GetFirstOfType<SCAL>(get_children());
	return (!val.isNull()) ? val->get_scale() : 1.0f;
}

Matrix43 NODE::GetLocalTransform() const
{
	return Matrix43( GetLocalTranslation(), GetLocalRotation(), GetLocalScale() );
}