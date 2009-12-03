// Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include "program.h"
#include "utility.h"
using namespace std;

#pragma comment(lib, "shlwapi.lib")

using namespace std;

#pragma warning(disable : 4073)

Cmd::CmdListType Cmd::CmdList;

// routine for parsing white space separated lines.  Handled like command line parameters w.r.t quotes.
void Cmd::ParseLine (
						const TCHAR *start,
						TCHAR **argv,
						TCHAR *args,
						int *numargs,
						int *numchars
						)
{
	const TCHAR NULCHAR    = '\0';
	const TCHAR SPACECHAR  = ' ';
	const TCHAR TABCHAR    = '\t';
	const TCHAR RETURNCHAR = '\r';
	const TCHAR LINEFEEDCHAR = '\n';
	const TCHAR DQUOTECHAR = '\"';
	const TCHAR SLASHCHAR  = '\\';
	const TCHAR *p;
	int inquote;                    /* 1 = inside quotes */
	int copychar;                   /* 1 = copy TCHAR to *args */
	unsigned numslash;              /* num of backslashes seen */

	*numchars = 0;
	*numargs = 0;                   /* the program name at least */

	p = start;

	inquote = 0;

	/* loop on each argument */
	for(;;) 
	{
		if ( *p ) { while (*p == SPACECHAR || *p == TABCHAR || *p == RETURNCHAR || *p == LINEFEEDCHAR) ++p; }

		if (*p == NULCHAR) break; /* end of args */

		/* scan an argument */
		if (argv)
			*argv++ = args;     /* store ptr to arg */
		++*numargs;

		/* loop through scanning one argument */
		for (;;) 
		{
			copychar = 1;
			/* Rules: 2N backslashes + " ==> N backslashes and begin/end quote
			2N+1 backslashes + " ==> N backslashes + literal "
			N backslashes ==> N backslashes */
			numslash = 0;
			while (*p == SLASHCHAR) 
			{
				/* count number of backslashes for use below */
				++p;
				++numslash;
			}
			if (*p == DQUOTECHAR) 
			{
				/* if 2N backslashes before, start/end quote, otherwise copy literally */
				if (numslash % 2 == 0) {
					if (inquote) {
						if (p[1] == DQUOTECHAR)
							p++;    /* Double quote inside quoted string */
						else        /* skip first quote TCHAR and copy second */
							copychar = 0;
					} else
						copychar = 0;       /* don't copy quote */

					inquote = !inquote;
				}
				numslash /= 2;          /* divide numslash by two */
			}

			/* copy slashes */
			while (numslash--) {
				if (args)
					*args++ = SLASHCHAR;
				++*numchars;
			}

			/* if at end of arg, break loop */
			if (*p == NULCHAR || (!inquote && (*p == SPACECHAR || *p == TABCHAR || *p == RETURNCHAR || *p == LINEFEEDCHAR)))
				break;

			/* copy character into argument */
			if (copychar) 
			{
				if (args)
					*args++ = *p;
				++*numchars;
			}
			++p;
		}
		/* null-terminate the argument */
		if (args)
			*args++ = NULCHAR;          /* terminate string */
		++*numchars;
	}
	/* We put one last argument in -- a null ptr */
	if (argv)
		*argv++ = NULL;
	++*numargs;
}

bool Cmd::ParseArgs(LPCTSTR line)
{
	int nargs = 0, nchars = 0;
	ParseLine(line, NULL, NULL, &nargs, &nchars);
	TCHAR **largv = (TCHAR **)malloc(nargs * sizeof(TCHAR *) + nchars * sizeof(TCHAR));
	ParseLine(line, largv, ((TCHAR*)largv) + nargs * sizeof(TCHAR*), &nargs, &nchars);
	bool rv = ParseArgs(nargs, largv);
	free(largv);
	return rv;
}

void Cmd::PrintHelp()
{
	TCHAR fullName[MAX_PATH], exeName[MAX_PATH];
	GetModuleFileName(NULL, fullName, MAX_PATH);
	_tsplitpath(fullName, NULL, NULL, exeName, NULL);

	_tcout << exeName << " built on " << __TIMESTAMP__ << endl
		<< "Usage: " << exeName << " <command> [-opts[modifiers]]" << endl
		<< endl
		<< "<Commands>" << endl;

	for (CmdListType::iterator itr = CmdList.begin(), end = CmdList.end(); itr != end; ++itr){
		Cmd* p = (*itr);
		_tcout << FormatString("  %-13s ", p->Name.c_str());
		p->HelpCmd(htShort);
		_tcout << endl;
	}
	_tcout << endl
		<< "<Global Switches>" << endl
		<< FormatString("  %-13s %s", "help", "List of additional help options") << endl
		<< endl;
}

Cmd* Cmd::GetCmd(_tstring name)
{
	for (CmdListType::iterator itr = CmdList.begin(), end = CmdList.end(); itr != end; ++itr){
		Cmd* p = (*itr);
		if (0 == _tcsicmp(p->Name.c_str(), name.c_str())){
			return p;
		}
	}
	return NULL;
}
list<Cmd*> Cmd::GetCmds()
{
	list<Cmd*> list;
	for (CmdListType::iterator itr = CmdList.begin(), end = CmdList.end(); itr != end; ++itr){
		Cmd* p = (*itr);
		list.push_back(p);
	}
	return list;
}

bool Cmd::ParseArgs(int argc, TCHAR **argv)
{
	bool rv = false;
	try
	{
		if (argc == 0)
		{
			PrintHelp();
			return false;
		}
		else if (argv[0] && ( 0 == _tcsicmp(argv[0], _T("help"))) )
		{
			if (argc > 1 && argv[1] && argv[1][0])
			{
				if (Cmd* p = GetCmd(argv[1])) {
					p->HelpCmd(htLong);
					return false;
				}
			}
			PrintHelp();
		}
		else
		{
			if (Cmd* p = GetCmd(argv[0])) {
				Program cmdLine(argc-1, &argv[1]);
				rv |= p->ExecuteCmd(cmdLine);
			}
			else
			{
				cerr << "Unknown command " << argv[0] <<endl;
				PrintHelp();
			}
		}
	}
	catch (exception* e)
	{
		cerr << "Exception occurred:" << endl;
		cerr << "  " << e->what() << endl;
	}
	catch (exception& e)
	{
		cerr << "Exception occurred:" << endl;
		cerr << "  " << e.what() << endl;
	}
	catch (...)
	{
		cerr << "Unknown exception occurred." << endl;
	}
	return rv;
}




Program::Program(int argc, TCHAR **argv, bool zeroargs)
{
	stringlist paths, plugins;
	this->argc = argc;
	this->argv = argv;
	DefaultToUnknown();

	for (int i = 0; i < argc; i++)
	{
		TCHAR *arg = argv[i];
		if (arg == NULL)
			continue;
		if (arg[0] == '-' || arg[0] == '/')
		{
			switch ( tolower(arg[1]) )
			{
			case 'o':
				{
					const TCHAR *param = arg+2;
					argv[i] = NULL;
					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
						param = argv[++i];
						argv[i] = NULL;
					}
					if ( param[0] == 0 )
						break;
					if (outfile.empty())
					{
						outfile = param;
					}
					else
					{
						_tcerr << "ERROR: Input file already specified as \"" << infile << "\"" << endl;
					}
				}
				break;

			case 'i':
				{
					const TCHAR *param = arg+2;
					argv[i] = NULL;
					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
						param = argv[++i];
						argv[i] = NULL;
					}
					if ( param[0] == 0 )
						break;
					if (infile.empty())
					{
						infile = param;
					}
					else
					{
						_tcerr << "ERROR: Input file already specified as \"" << infile << "\"" << endl;
					}
				}
				break;

			case 'p':
				{
					const TCHAR *param = arg+2;
					argv[i] = NULL;
					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
						param = argv[++i];
						argv[i] = NULL;
					}
					if ( param[0] == 0 )
						break;
					stringlist params = TokenizeString(param, _T(",;"));
					for (stringlist::iterator itr = params.begin(); itr != params.end(); ++itr)
						paths.push_back( *itr );
				}

			}
		}
	}
	if (!infile.empty() && outfile.empty()) {
		TCHAR path[MAX_PATH], drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
		_tsplitpath(infile.c_str(), drive, dir, fname, ext);
		_tcscat(fname, _T("-out"));
		_tmakepath(path, drive, dir, fname, ext);
		outfile = path;
	}
	//DefaultToLatest();
}

Program::~Program()
{
}

void SetWorkingDirectory()
{
	TCHAR fullpath[MAX_PATH];
	GetModuleFileName(NULL, fullpath, _countof(fullpath));
	PathRemoveFileSpec(fullpath);
	SetCurrentDirectory(fullpath);
}

int _tmain(int argc, _TCHAR* argv[])
{
	//SetWorkingDirectory();
	return (Cmd::ParseArgs(argc-1, &argv[1]) ? 0 : 1);
}
