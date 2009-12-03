#pragma once

#include <string>
#include <functional>
#include <set>
#include "loki/Functor.h"
#include "utility.h"

class Program
{
public:
	Program(int argc, TCHAR **argv, bool zeroargs=true);
	~Program();
	
	_tstring infile;
	_tstring outfile;
	std::set<HMODULE> plugins;

	int argc;
	TCHAR **argv;

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
			return (_tcsicmp(lhs.Name.c_str(), rhs.Name.c_str()) < 0);
		}
		bool operator()(const Cmd* lhs, const Cmd* rhs) const{
			return (_tcsicmp(lhs->Name.c_str(), rhs->Name.c_str()) < 0);
		}
	};
	typedef std::set<Cmd*, Cmd::Less> CmdListType;
	static CmdListType CmdList;
public:

	enum HelpType {
		htShort,
		htLong
	};

	Cmd(_tstring name
		, Loki::Functor<void, LOKI_TYPELIST_1(HelpType)> helpCmd
		, Loki::Functor<bool, LOKI_TYPELIST_1(Program&)> executeCmd)
		: Name(name)
		, HelpCmd(helpCmd)
		, ExecuteCmd(executeCmd)
	{
		CmdList.insert(this);
	}

	_tstring Name;
	Loki::Functor<void, LOKI_TYPELIST_1(HelpType)> HelpCmd;
	Loki::Functor<bool, LOKI_TYPELIST_1(Program&)> ExecuteCmd;

	static Cmd* GetCmd(_tstring name);
	static std::list<Cmd*> GetCmds();
	static void PrintHelp();
	static bool ParseArgs(LPCTSTR line);
	static bool ParseArgs(int argc, TCHAR **argv);
	static void ParseLine(const TCHAR *start,TCHAR **argv,TCHAR *args,int *numargs,int *numchars);

};

#define REGISTER_COMMAND(name,help,cmd) \
	Cmd plugin_ ## name (_T(#name), help, cmd);

