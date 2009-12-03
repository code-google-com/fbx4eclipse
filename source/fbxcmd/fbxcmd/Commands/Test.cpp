#include "stdafx.h"
#include "program.h"
#include "text.h"
using namespace std;

static void HelpString(Cmd::HelpType type){
	switch (type)
	{
	case Cmd::htShort: cout << "Test - Simple import/export."; break;
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

typedef double(mDouble44)[4][4] ;

KFbxXMatrix Make43(  float m11, float m12, float m13 
				   , float m21, float m22, float m23 
				   , float m31, float m32, float m33 
				   , float m41, float m42, float m43 )
{
	KFbxXMatrix mx;
	mDouble44 &m = const_cast<mDouble44 &>(static_cast<const mDouble44 &>(mx.Double44()));
	m[0][0] = m11; m[0][1] = m12; m[0][2] = m13; m[0][3] = 0.0;
	m[1][0] = m21; m[1][1] = m22; m[1][2] = m23; m[1][3] = 0.0;
	m[2][0] = m31; m[2][1] = m32; m[2][2] = m33; m[2][3] = 0.0;
	m[3][0] = m41; m[3][1] = m42; m[3][2] = m43; m[3][3] = 0.0;

	KFbxVector4 p = mx.GetT();
	KFbxQuaternion q = mx.GetQ();
	KFbxVector4 s = mx.GetS();

	return mx;
}

KFbxXMatrix Make44(  float m11, float m12, float m13, float m14
				   , float m21, float m22, float m23, float m24
				   , float m31, float m32, float m33, float m34
				   , float m41, float m42, float m43, float m44 )
{
	KFbxXMatrix mx;
	mDouble44 &m = const_cast<mDouble44 &>(static_cast<const mDouble44 &>(mx.Double44()));
	m[0][0] = m11; m[0][1] = m12; m[0][2] = m13; m[0][3] = m14;
	m[1][0] = m21; m[1][1] = m22; m[1][2] = m23; m[1][3] = m24;
	m[2][0] = m31; m[2][1] = m32; m[2][2] = m33; m[2][3] = m34;
	m[3][0] = m41; m[3][1] = m42; m[3][2] = m43; m[3][3] = m44;

	KFbxVector4 p = mx.GetT();
	KFbxQuaternion q = mx.GetQ();
	KFbxVector4 s = mx.GetS();
	
	return mx;
}

KFbxXMatrix& Dump(LPCSTR name, KFbxXMatrix& m)
{
	Text line;
	Text txtPos0, txtPos1, txtPos2, txtPos3, txtFormat;
	KFbxVector4 r0 = m.GetRow(0);
	KFbxVector4 r1 = m.GetRow(1);
	KFbxVector4 r2 = m.GetRow(2);
	KFbxVector4 r3 = m.GetRow(3);
	txtPos0.Format("[%7.4f,%7.4f,%7.4f]", r0[0], r0[1], r0[2]);
	txtPos1.Format("[%7.4f,%7.4f,%7.4f]", r1[0], r1[1], r1[2]);
	txtPos2.Format("[%7.4f,%7.4f,%7.4f]", r2[0], r2[1], r2[2]);
	txtPos3.Format("[%7.4f,%7.4f,%7.4f]", r3[0], r3[1], r3[2]);

	txtFormat.Format("%%-20s %%s %%s %%s %%s\n");
	line.Format(txtFormat.c_str(), name, txtPos0.c_str(), txtPos1.c_str(), txtPos2.c_str(), txtPos3.c_str());
	printf(txtFormat.c_str(), name, txtPos0.c_str(), txtPos1.c_str(), txtPos2.c_str(), txtPos3.c_str());
	return m;
}

static bool ExecuteCmd(Program &prog)
{
	KFbxSdkManager* pSdkManager = prog.pSdkManager;
	string infile = prog.infile;
	string outfile = prog.outfile;
	int argc = prog.argc;
	char **argv = prog.argv;
	bool verboseHelp = false;

	//KFbxXMatrix wtm;
	//Dumpwtm = wtm * Make43( 1.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000);
	//Dump(wtm);
	//wtm = wtm * Make43(-1.0000, 0.0000, 0.0000, 0.0000,-1.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000);
	//Dump(wtm);
	//wtm = wtm * Make43( 0.0000,-1.0000, 0.0000, 0.0000, 0.0000,-1.0000, 1.0000, 0.0000, 0.0000, 0.0000, 0.0053, 0.7895);
	//Dump(wtm);
	//wtm = wtm * Make43( 1.0000,-0.0000, 0.0083, 0.0000, 1.0000, 0.0000,-0.0083,-0.0000, 1.0000, 0.0414, 0.0000, 0.0000);
	//Dump(wtm);
	//KFbxXMatrix wtm = Make43( 0.000000,1.000000,0.000000,-1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000);
	KFbxXMatrix wtm;

	KFbxXMatrix n[20]; 
	n[0] = Make43( 1.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000);
	n[1] = Make43(-1.0000, 0.0000, 0.0000, 0.0000,-1.0000, 0.0000, 0.0000, 0.0000, 1.0000, 0.0000, 0.0000, 0.0000);
	n[2] = Make43( 0.0000,-1.0000, 0.0000, 0.0000, 0.0000,-1.0000, 1.0000, 0.0000, 0.0000, 0.0000, 0.0053, 0.7895);
	n[3] = Make43( 1.0000,-0.0000, 0.0083, 0.0000, 1.0000, 0.0000,-0.0083,-0.0000, 1.0000, 0.0414, 0.0000, 0.0000);
	n[4] = Make43( 0.9882, 0.0000, 0.1531, 0.0000, 1.0000,-0.0000,-0.1531, 0.0000, 0.9882, 0.1116, 0.0000,-0.0000);
	n[5] = Make43( 0.9959, 0.0000,-0.0906, 0.0000, 1.0000, 0.0000, 0.0906,-0.0000, 0.9959, 0.1116, 0.0000,-0.0000);
	n[6] = Make43( 0.9632,-0.0000,-0.2689, 0.0000, 1.0000,-0.0000, 0.2689, 0.0000, 0.9632, 0.1116, 0.0000, 0.0000);
	n[7] = Make43( 0.9843, 0.1764,-0.0000, 0.0000, 0.0000, 1.0000, 0.1764,-0.9843, 0.0000, 0.1070, 0.0000,-0.0194);
	n[8] = Make43( 0.9863,-0.1649, 0.0000, 0.1649, 0.9863, 0.0000,-0.0000,-0.0000, 1.0000, 0.0425,-0.0000, 0.0000);
	n[9] = Make43( 0.9783,-0.2070, 0.0000, 0.2070, 0.9783,-0.0000,-0.0000, 0.0000, 1.0000, 0.0392, 0.0000,-0.0000);
	n[10] = Make43( 0.0000,-0.0039, 1.0000,-0.0000, 1.0000, 0.0039,-1.0000,-0.0000, 0.0000, 0.0887,-0.0211, 0.0000);
	n[11] = Make43( 1.0000, 0.0000,-0.0000,-0.0000, 0.0002,-1.0000,-0.0000, 1.0000, 0.0002,-0.0000,-0.0193,-0.0592);
	n[12] = Make43( 1.0000, 0.0000, 0.0000, 0.0000, 1.0000,-0.0000, 0.0000, 0.0000, 1.0000, 0.0303,-0.0232, 0.0099);

	KFbxXMatrix m; 
	m = n[2] * n[1] * n[0];
	KFbxQuaternion q1 = m.GetQ();
	KFbxQuaternion q2 = n[3].GetQ();
	KFbxQuaternion q3 = q2 * q1;


	KFbxXMatrix m1;
	m1.SetTQS(m.GetT(), q3, m.GetS());

	KFbxXMatrix m2;
	m2.SetT(n[3].GetT());

	KFbxXMatrix m3 = m2 * m1;
	KFbxVector4 v1 = m3.GetT();


	

	//m = n[4].GetQ() * n[3] * n[2] * n[1] * n[0];
	//Dump("asfd",	m);


	//Dump("GOB",					wtm *= n[0] );
	//Dump("GOD",					wtm *= n[1] );
	//Dump("Root",				wtm *= n[2] );
	//Dump("LowerBack",			wtm *= n[3] );
	//Dump("Chest",				wtm *= n[4] );
	//Dump("Chest1",				wtm = n[5]  * wtm);
	//Dump("Chest2",				wtm = n[6]  * wtm);
	//Dump("Neck",				wtm = n[7]  * wtm);
	//Dump("Neck1",				wtm = n[8]  * wtm);
	//Dump("Head",				wtm = n[9]  * wtm);
	//Dump("headBase",			wtm = n[10] * wtm);
	//Dump("mouthBase",			wtm = n[11] * wtm);
	//Dump("outerUpperLip_right", wtm = n[12] * wtm);
	return true;
}

REGISTER_COMMAND(Test, HelpString, ExecuteCmd);
