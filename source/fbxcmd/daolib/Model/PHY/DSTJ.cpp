
/**********************************************************************
*<
FILE: DSTJ.h

DESCRIPTION:	PHY File Format

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include "GFF/GFFCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"
#include "PHY/PHYCommon.h"
#include "PHY/DSTJ.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::PHY;
///////////////////////////////////////////////////////////////////

ShortString DSTJ::type("DSTJ");DSTJ::DSTJ(GFFStructRef owner) : impl(owner) {
}

float DSTJ::get_jointDistanceMinDistance() const {
  return impl->GetField(GFF_MMH_JOINT_DISTANCE_MIN_DISTANCE)->asFloat32();
}

void DSTJ::set_jointDistanceMinDistance(float value) {
  impl->GetField(GFF_MMH_JOINT_DISTANCE_MIN_DISTANCE)->assign(value);
}

float DSTJ::get_jointDistanceMaxDistance() const {
  return impl->GetField(GFF_MMH_JOINT_DISTANCE_MAX_DISTANCE)->asFloat32();
}

void DSTJ::set_jointDistanceMaxDistance(float value) {
  impl->GetField(GFF_MMH_JOINT_DISTANCE_MAX_DISTANCE)->assign(value);
}

Vector3f DSTJ::get_jointDistanceSpring() const {
  return impl->GetField(GFF_MMH_JOINT_DISTANCE_SPRING)->asVector3f();
}

void DSTJ::set_jointDistanceSpring(Vector3f value) {
  impl->GetField(GFF_MMH_JOINT_DISTANCE_SPRING)->assign(value);
}

unsigned long DSTJ::get_jointDistanceDistanceFlags() const {
  return impl->GetField(GFF_MMH_JOINT_DISTANCE_DISTANCE_FLAGS)->asUInt32();
}

void DSTJ::set_jointDistanceDistanceFlags(unsigned long value) {
  impl->GetField(GFF_MMH_JOINT_DISTANCE_DISTANCE_FLAGS)->assign(value);
}

