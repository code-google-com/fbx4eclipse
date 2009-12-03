/**********************************************************************
*<
FILE: MMHExport.h

DESCRIPTION:	MMH Exporter 

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

#include "BaseExporter.h"

// MMH Exporter
class MMHExporter : public BaseExporter//, public IniFileSection
{
public:
   MMHExporter(const TCHAR *Name,ExpInterface *I,Interface *GI, BOOL SuppressPrompts, DWORD options);
   virtual void Initialize();

   void ApplyAppSettings();

   bool HasSkeleton();
   bool IsBiped();
   _tstring FindImage(const _tstring& name);
   bool ShowDialog();
   virtual bool DoExport();

   protected: 
      MMHExporter();
};
