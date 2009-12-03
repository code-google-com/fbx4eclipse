/**********************************************************************
*<
FILE: MSHImport.h

DESCRIPTION:	MSH Importer 

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

#include "BaseImporter.h"

// MSH Importer
class MSHImporter : public BaseImporter//, public IniFileSection
{
public:
   MSHImporter(const TCHAR *Name,ImpInterface *I,Interface *GI, BOOL SuppressPrompts);
   virtual void Initialize();

   void ApplyAppSettings();

   bool HasSkeleton();
   bool ShowDialog();
   virtual bool DoImport();

   protected: 
      MSHImporter();
};
