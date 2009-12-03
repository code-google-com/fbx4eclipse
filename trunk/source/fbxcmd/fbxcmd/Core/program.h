#pragma once

#include <string>
#include <functional>
#include <set>
#include "loki/Functor.h"
#include <fbxsdk.h>
using namespace FBXFILESDK_NAMESPACE;

class Program
{
public:
	Program(int argc, char **argv, bool zeroargs=true);
	~Program();
	
	KFbxSdkManager* pSdkManager;
	KFbxMemoryAllocator * pAllocator;
	std::string infile;
	std::string outfile;
	std::set<HMODULE> plugins;

	int argc;
	char **argv;

public:
	void DefaultToUnknown()
	{
	}
	void DefaultToLatest()
	{
	}
};

class Cmd
{
	struct Less
	{
		bool operator()(const Cmd& lhs, const Cmd& rhs) const{
			return (stricmp(lhs.Name.c_str(), rhs.Name.c_str()) < 0);
		}
		bool operator()(const Cmd* lhs, const Cmd* rhs) const{
			return (stricmp(lhs->Name.c_str(), rhs->Name.c_str()) < 0);
		}
	};
	typedef std::set<Cmd*, Cmd::Less> CmdListType;
	static CmdListType CmdList;
public:

	enum HelpType {
		htShort,
		htLong
	};

	Cmd(std::string name
		, Loki::Functor<void, LOKI_TYPELIST_1(HelpType)> helpCmd
		, Loki::Functor<bool, LOKI_TYPELIST_1(Program&)> executeCmd)
		: Name(name)
		, HelpCmd(helpCmd)
		, ExecuteCmd(executeCmd)
	{
		CmdList.insert(this);
	}

	std::string Name;
	Loki::Functor<void, LOKI_TYPELIST_1(HelpType)> HelpCmd;
	Loki::Functor<bool, LOKI_TYPELIST_1(Program&)> ExecuteCmd;

	static Cmd* GetCmd(std::string name);
	static std::list<Cmd*> GetCmds();
	static void PrintHelp();
	static bool ParseArgs(LPCTSTR line);
	static bool ParseArgs(int argc, char **argv);
	static void ParseLine(const char *start,char **argv,char *args,int *numargs,int *numchars);

};

extern std::string FormatString(const TCHAR* format,...);

#define REGISTER_COMMAND(name,help,cmd) \
	Cmd plugin_ ## name (#name, help, cmd);

