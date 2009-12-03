#include "stdafx.h"
#include "program.h"
#include <sys/stat.h>
#include <io.h>
#include <shellapi.h>
using namespace std;

#include "GFF/GFFFile.h"
#include "GFF/GFFStruct.h"
#include "GFF/GFFField.h"
#include "GFF/GFFList.h"

//////////////////////////////////////////////////////////////////////////


static void HelpString(Cmd::HelpType type){
	switch (type)
	{
	case Cmd::htShort: cout << "DumpGFF - Dump GFF files to console"; break;
	case Cmd::htLong:  
		{
			TCHAR fullName[MAX_PATH], exeName[MAX_PATH];
			GetModuleFileName(NULL, fullName, MAX_PATH);
			_tsplitpath(fullName, NULL, NULL, exeName, NULL);
			cout << "Usage: " << exeName << " erf [-opts[modifiers]] <erf file>" << endl 
				<< "  Simple pack/unpack help for DA:O ERF files." << endl
				<< endl
				<< "  If input argument is a file then the program unpack the erf file." << endl
				<< "  If input argument is a path then the program packs an erf file from that path." << endl
				<< endl
				<< "<Switches>" << endl
				<< "  d <path>  Output directory or file" << endl
				<< "  v         Verbose Help (Lists specific versions)" << endl
				<< endl
				;
		}
		break;
	}
}

static bool ExecuteCmd(Program &prog)
{
	TCHAR infile[MAX_PATH], fullpath[MAX_PATH];
	int argc = prog.argc;
	TCHAR **argv = prog.argv;
	bool verboseHelp = false;
	TCHAR path[MAX_PATH];

	if (argc == 0)
		return false;

	infile[0] = 0;
	path[0] = 0;
	for (int i = 0; i < argc; i++)
	{
		TCHAR *arg = argv[i];
		if (arg == NULL)
			continue;
		if (arg[0] == '-' || arg[0] == '/')
		{
			switch (arg[1])
			{
				case 'v':
					verboseHelp = true;
					break;

				case 'd':
					if (++i < argc) {
						_tcscpy(path, argv[i]);
					}
					break;

				default:
					_fputts( _T("ERROR: Unknown argument specified \""), stderr );
					_fputts( arg, stderr );
					_fputts( _T("\".\n"), stderr );
					return false;
			}
		}
		else if (infile[0] == 0)
		{
			_tcscpy(infile, arg);
		}
	}
	if (infile[0] == 0)
	{
		_ftprintf( stderr, _T("File not specified.\n") );
		return false;
	}

	TCHAR drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
	GetFullPathName(infile, _countof(fullpath), fullpath, NULL);
	_tsplitpath(fullpath, drive, dir, fname, ext);
	if (path[0] == 0) {
		_tmakepath(path, drive, dir, fname, NULL);
	}

	struct _stat64 fstats;
	if ( -1 == _tstat64(fullpath, &fstats) ){
		_ftprintf( stderr, _T("Path is not a valid file or directory.\n") );
		return false;
	}

	if ( (fstats.st_mode & _S_IFDIR) != _S_IFDIR ) // file not directory
	{
		DAO::GFF::GFFFile gfffile;
		gfffile.open(fullpath);

		DAODumpStream out(NULL, false);
		const DAO::GFF::GFFInfo& info = gfffile.get_FileInfo();
		for (int i=0; i<info.Structs.size(); ++i) {
			DAO::GFF::GFFStructDef &sval = *info.Structs[i];

			Text fmt = "| %-64s| %-11s | %-18s | %-5s |\n";
			out.PrintF("%s\n", sval.StructType.data());
			out.PrintF("--------------------------------------------------------------------------------------------------------------\n");
			out.PrintF(fmt, "Label", "TypeID", "Flags", "Index");
			out.PrintF("--------------------------------------------------------------------------------------------------------------\n");

			for (int i=0; i<sval.Fields.count(); ++i)
			{
				DAO::GFF::GFFFieldDefRef field ( sval.Fields[i] );

				Text label, typeID, flags, index;

				label = EnumToText(field->Label);
				typeID = EnumToText(field->TypeID);
				flags = EnumToText(field->Flags);
				index.Format("%d", field->Index);

				out.PrintF(fmt.c_str(), label.c_str(), typeID.c_str(), flags.c_str(), index.c_str());
			}
			out.PrintF("--------------------------------------------------------------------------------------------------------------\n");

			out.Print("\n\n");
		}

		//gfffile.dump();

		//DAOStream in(fullpath, true);
		//DAO::GFFInfo info;
		//if (!Recall(in, info)) {
		//	throw runtime_error(FormatStringA("Invalid File Header for file: %s", fullpath));
		//}
		//DAODumpStream out(NULL, false);
		//Dump(out, PathFindFileName(fullpath), info);
	}

	return false;
}

REGISTER_COMMAND(DumpGFF, HelpString, ExecuteCmd);
