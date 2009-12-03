#include "stdafx.h"
#include "program.h"
#include <sys/stat.h>
#include <io.h>
#include <shellapi.h>
using namespace std;

#include "GFFFormat.h"
#include "MMHFormat.h"
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;

//////////////////////////////////////////////////////////////////////////

void ImportBones( DAODumpStream& log, Matrix44 pm, NODERef node)
{
	int indent = log.IncreaseIndent();
	try 
	{
		Text name = !node.isNull() ? node->get_name() : Text();

		GFFListRef childList = node->get_children();

		Vector4f trans = node->GetLocalTranslation();
		Quaternion quat = node->GetLocalRotation();
		float scale = node->GetLocalScale();
		Matrix44 tm(trans, quat, scale);

		float len = trans.Length();


		Matrix44 im = pm * tm;

		//[-0.161288,0.000001,0.986907]
		//[-0.000001,-1.000000,0.000001]
		//[0.986907,-0.000001,0.161288]
		//[-0.002451,0.000000,0.371050]

		Matrix44& dm = im;

		Text txtPos, txtRot, txtYpr, txtScale, txtFormat, txtMatrix;
		txtPos.Format("(%7.4f, %7.4f, %7.4f)", trans[0], trans[1], trans[2]);
		txtRot.Format("(%7.4f, %7.4f, %7.4f, %7.4f)", quat.w, quat.x, quat.y, quat.z);
		txtFormat.Format("%%-%ds | %%-24s | %%-32s\n", max(60 - indent*2, 1) );
		//log.Indent();
		//log.PrintF(txtFormat, name.c_str(), txtPos.c_str(), txtRot.c_str());


		Text txtPos0, txtPos1, txtPos2, txtPos3;
		Vector4f r0 = dm.GetRow(0);
		Vector4f r1 = dm.GetRow(1);
		Vector4f r2 = dm.GetRow(2);
		Vector4f r3 = dm.GetRow(3);
		txtPos0.Format("[%7.4f,%7.4f,%7.4f]", r0.x, r0.y, r0.z);
		txtPos1.Format("[%7.4f,%7.4f,%7.4f]", r1.x, r1.y, r1.z);
		txtPos2.Format("[%7.4f,%7.4f,%7.4f]", r2.x, r2.y, r2.z);
		txtPos3.Format("[%7.4f,%7.4f,%7.4f]", r3.x, r3.y, r3.z);

		txtFormat.Format("%%-%ds | %%s %%s %%s %%s\n", max(60 - indent*2, 1) );
		log.Indent();
		log.PrintF(txtFormat, name.c_str(), txtPos0.c_str(), txtPos1.c_str(), txtPos2.c_str(), txtPos3.c_str());


		DAOArray< NODEPtr > children;
		if ( DynamicCast(childList, children) ) {
			for (size_t i=0, n=children.size(); i<n; ++i)
				ImportBones(log, im, NODERef(children[i]));
		}
	}
	catch( std::exception & ) 
	{
	}
	catch( ... ) 
	{
	}
	log.DecreaseIndent();
}

void ImportSkeleton(MDLHRef root)
{
	DAODumpStream out(NULL, false);

	GFFListRef list = root->get_children();
	DAOArray< NODEPtr > bones;
	if ( DynamicCast(list, bones) )
	{
		//Matrix44 wtm(true);
		Matrix44 wtm(
			Vector4f( 0.000000,1.000000,0.000000,0.00000),
			Vector4f(-1.000000,0.000000,0.000000,0.00000),
			Vector4f( 0.000000,0.000000,1.000000,0.00000),
			Vector4f( 0.000000,0.000000,0.000000,1.00000));

		for (size_t i=0, n=bones.size(); i<n; ++i)
		{
			ImportBones(out, wtm, bones[i]);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

static void HelpString(Cmd::HelpType type){
	switch (type)
	{
	case Cmd::htShort: cout << "DumpSkeleton - Dump MMH Skeleton information to console"; break;
	case Cmd::htLong:  
		{
			TCHAR fullName[MAX_PATH], exeName[MAX_PATH];
			GetModuleFileName(NULL, fullName, MAX_PATH);
			_tsplitpath(fullName, NULL, NULL, exeName, NULL);
			cout << "Usage: " << exeName << " erf [-opts[modifiers]] <erf file>" << endl 
				<< "  Simple skeleton dump help for DA:O MMH files." << endl
				<< endl
				<< "  If input argument is a file then the program unpack the erf file." << endl
				<< "  If input argument is a path then the program packs an erf file from that path." << endl
				<< endl
				<< "<Switches>" << endl
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
		MMHFile file;
		file.open(fullpath);
		MDLHPtr root = file.get_Root();

		ImportSkeleton( MDLHRef(root) );

		//DAODumpStream out(NULL, false);
		//const DAO::GFF::GFFInfo& info = gfffile.get_FileInfo();
		//for (int i=0; i<info.Structs.size(); ++i) {
		//	DAO::GFF::GFFStructDef &sval = *info.Structs[i];

		//	Text fmt = "| %-64s| %-11s | %-18s | %-5s |\n";
		//	out.PrintF("%s\n", sval.StructType.data());
		//	out.PrintF("--------------------------------------------------------------------------------------------------------------\n");
		//	out.PrintF(fmt, "Label", "TypeID", "Flags", "Index");
		//	out.PrintF("--------------------------------------------------------------------------------------------------------------\n");

		//	for (int i=0; i<sval.Fields.count(); ++i)
		//	{
		//		DAO::GFF::GFFFieldDefRef field ( sval.Fields[i] );

		//		Text label, typeID, flags, index;

		//		label = EnumToText(field->Label);
		//		typeID = EnumToText(field->TypeID);
		//		flags = EnumToText(field->Flags);
		//		index.Format("%d", field->Index);

		//		out.PrintF(fmt.c_str(), label.c_str(), typeID.c_str(), flags.c_str(), index.c_str());
		//	}
		//	out.PrintF("--------------------------------------------------------------------------------------------------------------\n");

		//	out.Print("\n\n");
		//}

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

REGISTER_COMMAND(DumpSkeleton, HelpString, ExecuteCmd);
