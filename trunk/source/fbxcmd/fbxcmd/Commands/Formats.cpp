#include "stdafx.h"
#include "program.h"

using namespace std;

static void HelpString(Cmd::HelpType type){
	switch (type)
	{
	case Cmd::htShort: cout << "Convert - Simple import/export."; break;
	case Cmd::htLong:  
		{
			char fullName[MAX_PATH], exeName[MAX_PATH];
			GetModuleFileName(NULL, fullName, MAX_PATH);
			_splitpath(fullName, NULL, NULL, exeName, NULL);
			cout << "Usage: " << exeName << " formats [-opts[modifiers]]" << endl 
				<< "  Simply import and export." << endl
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
	KFbxSdkManager* pSdkManager = prog.pSdkManager;
	string infile = prog.infile;
	string outfile = prog.outfile;
	int argc = prog.argc;
	char **argv = prog.argv;
	bool verboseHelp = false;

	list<Cmd *> plugins;
	if (argc == 0) {
		return false;
	}

	for (int i = 0; i < argc; i++)
	{
		char *arg = argv[i];
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
					fputs( "ERROR: Unknown argument specified \"", stderr );
					fputs( arg, stderr );
					fputs( "\".\n", stderr );
					return false;
			}
		}
	}

	int lMajor, lMinor, lRevision;

	KFbxIO::GetCurrentVersion(lMajor, lMinor, lRevision);
	cout << FormatString("FBX SDK version %d.%d.%d", lMajor, lMinor, lRevision) << endl << endl;

	KFbxIOPluginRegistry* pRegistry = pSdkManager->GetIOPluginRegistry();

	int defaultReader = pRegistry->GetNativeReaderFormat();
	int defaultWriter = pRegistry->GetNativeWriterFormat();

	std::cout << "Supported Import Formats:" << std::endl;
	int nbReaders = pRegistry->GetReaderFormatCount();
	for(int i=0; i < nbReaders ; i++)
	{
		const char *ext = pRegistry->GetWriterFormatExtension(i);
		const char *desc = pRegistry->GetWriterFormatDescription(i);
		cout << FormatString("%-8s %s%s", ext, desc, defaultReader==i?" (default)": "") << endl;
	}
	std::cout << std::endl;

	std::cout << "Supported Export Formats:" << std::endl;
	int nbWriters = pRegistry->GetWriterFormatCount();
	for(int i=0; i < nbWriters; i++)
	{
		const char *ext = pRegistry->GetWriterFormatExtension(i);
		const char *desc = pRegistry->GetWriterFormatDescription(i);
		cout << FormatString("%-8s %s%s", ext, desc, defaultWriter==i?" (default)": "");
		if (verboseHelp)
		{
			if (char const* const* versions = pRegistry->GetWritableVersions(i))
			{
				bool first = true;
				int len = 120;
				for(; versions && *versions; ++versions) {
					if (len > 60)
					{
						cout << endl << FormatString("%-8s ", "");
						len = 9;
					}
					if (!first) { cout << ", "; } else { cout << "[ "; }
					cout << *versions; 
					len += (2 + strlen(*versions));
					first = false;
				}
				cout << " ]" << endl;
			}
		}
		cout << endl;
	}

	return true;
}

REGISTER_COMMAND(Formats, HelpString, ExecuteCmd);
