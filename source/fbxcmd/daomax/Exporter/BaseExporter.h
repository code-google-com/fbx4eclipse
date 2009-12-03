/**********************************************************************
*<
FILE: BaseExporter.h

DESCRIPTION:	Base Exporter class

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

// Exporter Base
class BaseExporter
{
public:
   _tstring name;
   _tstring path;
   ExpInterface *i;
   Interface *gi;
   BOOL suppressPrompts;
   bool iniFileValid;
   _tstring iniFileName;
   DWORD options;

   BaseExporter(){}

   void BaseInit(const TCHAR *Name,ExpInterface *I,Interface *GI, BOOL SuppressPrompts, DWORD options)
   {
      name = Name;
      i=I;
      gi=GI;
      suppressPrompts = SuppressPrompts;
      
      char buffer[MAX_PATH] = {0}, *p = NULL;
      GetFullPathName(Name, _countof(buffer), buffer, &p);
      if (p) *p = 0;
      path = buffer;
      iniFileValid = false;
      this->options = options;

      Initialize();
   }

   virtual void Initialize() = 0;

   virtual bool isValid() const { return true; }
};
