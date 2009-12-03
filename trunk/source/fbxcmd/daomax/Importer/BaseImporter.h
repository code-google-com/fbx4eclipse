/**********************************************************************
*<
FILE: BaseImporter.h

DESCRIPTION:	Base Importer class

CREATED BY: tazpn (Theo)

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

// Importer Base
class BaseImporter
{
public:
	std::string name;
	std::string path;
	ImpInterface *i;
	Interface *gi;
	BOOL suppressPrompts;
	bool iniFileValid;
	std::string iniFileName;

	BaseImporter(){}

	void BaseInit(const TCHAR *Name,ImpInterface *I,Interface *GI, BOOL SuppressPrompts)
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

		Initialize();
	}

	virtual void Initialize() = 0;

	virtual bool isValid() const { return true; }
};
