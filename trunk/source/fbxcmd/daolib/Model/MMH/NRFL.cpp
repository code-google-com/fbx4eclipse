
/**********************************************************************
*<
FILE: NRFL.h

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
#include "MMH/NRFL.h"
using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
///////////////////////////////////////////////////////////////////

ShortString NRFL::type("NRFL");NRFL::NRFL(GFFStructRef owner) : impl(owner) {
}

Text NRFL::get_lightProbeEnvmap() const {
  return impl->GetField(GFF_ENV_LIGHT_PROBE_ENVMAP)->asECString();
}

void NRFL::set_lightProbeEnvmap(const Text& value) {
  impl->GetField(GFF_ENV_LIGHT_PROBE_ENVMAP)->assign(value);
}

