/**********************************************************************
*<
FILE: MSHExport.h

DESCRIPTION:	MSH Exporter 

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

#include "BaseExporter.h"

// MSH Exporter
class MSHExporter : public BaseExporter//, public IniFileSection
{
public:
   MSHExporter(const TCHAR *Name,ExpInterface *I,Interface *GI, BOOL SuppressPrompts, DWORD options);
   virtual void Initialize();

   void ApplyAppSettings();

   bool HasSkeleton();
   bool IsBiped();
   _tstring FindImage(const _tstring& name);
   bool ShowDialog();
   virtual bool DoExport();

   protected: 
      MSHExporter();
};
