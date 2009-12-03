
/**********************************************************************
*<
FILE: SPNV.h

DESCRIPTION:	MMH File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once

#include "MMH/MMHCommon.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"
#include "GFF/GFFStruct.h"

namespace DAO {

using namespace GFF;
namespace MMH {
///////////////////////////////////////////////////////////////////

class SPNV
{
protected:
  GFFStructRef impl;
  static ShortString type;
public:
  SPNV(GFFStructRef owner);

  static const ShortString& Type() { return type; }
  const ShortString& get_type() const { return type; }

  unsigned char get_nodeSpawnVolumeOptionsSpawnWithinVolume() const;
  void set_nodeSpawnVolumeOptionsSpawnWithinVolume(unsigned char value);

  unsigned char get_nodeSpawnVolumeOptionsInvertSpawnVolumeNormals() const;
  void set_nodeSpawnVolumeOptionsInvertSpawnVolumeNormals(unsigned char value);

  unsigned char get_nodeSpawnVolumeType() const;
  void set_nodeSpawnVolumeType(unsigned char value);

  float get_nodeSpawnVolumeRadius() const;
  void set_nodeSpawnVolumeRadius(float value);

  float get_nodeSpawnVolumeCylinderLength() const;
  void set_nodeSpawnVolumeCylinderLength(float value);

  Vector3f get_nodeSpawnVolumeCylinderAxis() const;
  void set_nodeSpawnVolumeCylinderAxis(Vector3f value);

  Vector3f get_nodeSpawnVolumeBoxMin() const;
  void set_nodeSpawnVolumeBoxMin(Vector3f value);

  Vector3f get_nodeSpawnVolumeBoxMax() const;
  void set_nodeSpawnVolumeBoxMax(Vector3f value);

  unsigned char get_nodeSpawnVolumeOptionsNormalsAsDirection() const;
  void set_nodeSpawnVolumeOptionsNormalsAsDirection(unsigned char value);

  GFFListRef get_children() const;

};

typedef ValuePtr<SPNV> SPNVPtr;
typedef ValueRef<SPNV> SPNVRef;



} //namespace MMH
} //namespace DAO
