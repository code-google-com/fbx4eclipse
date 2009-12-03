/**********************************************************************
*<
FILE: FBXImport.h

DESCRIPTION:	FBX Importer 

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

#include "BaseImporter.h"

// FBX Importer
class FBXImporter : public BaseImporter//, public IniFileSection
{
public:
   FBXImporter(const TCHAR *Name,ImpInterface *I,Interface *GI, BOOL SuppressPrompts);
   virtual void Initialize();

   void ApplyAppSettings();
   bool ShowDialog();
   virtual bool DoImport();

   // Generic IniFile reading routine
   template<typename T>
   T GetIniValue(LPCTSTR Section, LPCTSTR Setting, T Default){
	   if (!iniFileValid) 
		   return Default;
	   return ::GetIniValue<T>(Section, Setting, Default, iniFileName.c_str());
   }

   // Generic IniFile reading routine
   template<typename T>
   void SetIniValue(LPCTSTR Section, LPCTSTR Setting, T value){
	   if (!iniFileValid) 
		   return;
	   ::SetIniValue<T>(Section, Setting, value, iniFileName.c_str());
   }

   protected: 
      FBXImporter();
};
