#include "stdafx.h"
#include "program.h"
#include <sys/stat.h>
#include <io.h>
#include <shellapi.h>
using namespace std;

#include "GFFFormat.h"
#include "MSHFormat.h"
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MESH;

#ifndef ASSERT
#ifdef _DEBUG
#include <crtdbg.h>
#define ASSERT _ASSERTE
#else
#define ASSERT(exp)
#endif
#endif

//////////////////////////////////////////////////////////////////////////


template <typename T, typename U> 
void AssignValue(T& dst, const U& src) {
	ASSERT( !"Assignment not specified.");
}

template<> void AssignValue(float& dst, const float& src)				{ dst = src; }
template<> void AssignValue(Vector2f& dst, const Vector2f& src)			{ dst.Set(src[0], src[1]); }
template<> void AssignValue(Vector2f& dst, const Float16_2& src)		{ dst.Set(src[0], src[1]); }
template<> void AssignValue(Vector4f& dst, const Vector2f& src)			{ dst.Set(src[0], src[1], 0.0); }
template<> void AssignValue(Vector4f& dst, const Vector3f& src)			{ dst.Set(src[0], src[1], src[2]); }
template<> void AssignValue(Vector4f& dst, const Vector4f& src)			{ dst.Set(src[0], src[1], src[2]); }
template<> void AssignValue(Vector4f& dst, const Float16_4& src)		{ dst.Set(src[0], src[1], src[2], src[3]); }
template<> void AssignValue(Color4& dst, const Vector3f& src)			{ dst.Set(src[0], src[1], src[2]); }
template<> void AssignValue(Color4& dst, const Vector4f& src)			{ dst.Set(src[0], src[1], src[2], src[3]); }
template<> void AssignValue(Color4& dst, const ARGB4& src)				{ dst.Set(src[0], src[1], src[2], src[3]); }
template<> void AssignValue(Color4& dst, const Float16_4& src)			{ dst.Set(src[0], src[1], src[2], src[3]); }
template<> void AssignValue(Vector4f& dst, const Signed_10_10_10& src)	{ dst.Set(src[0], src[1], src[2]); }
template<> void AssignValue(Vector4f& dst, const Unsigned_10_10_10& src){ dst.Set(src[0], src[1], src[2]); }
template<> void AssignValue(Color4& dst, const Signed_10_10_10& src)	{ dst.Set(src[0], src[1], src[2], src[3]); }
template<> void AssignValue(Color4& dst, const Unsigned_10_10_10& src)	{ dst.Set(src[0], src[1], src[2], src[3]); }


void Dump(IDAODumpStream& out, LPCTSTR name, const UShort3& val)
{
	char buffer[256];
	sprintf(buffer, "[%4hu,%4hu,%4hu]", val[0], val[1], val[2]);
	Dump(out, name, buffer);
}
void Dump(IDAODumpStream& out, LPCTSTR name, const UShort4& val)
{
	char buffer[256];
	sprintf(buffer, "[%4hu,%4hu,%4hu,%4hu]", val[0], val[1], val[2], val[3]);
	Dump(out, name, buffer);
}


template <class T>
void AssignValue(T& dst, const DECLValue& src) {
	switch (src.get_type())
	{
	case DECLTYPE_FLOAT1:  AssignValue(dst, src.asFloat()); break;
	case DECLTYPE_FLOAT2:  AssignValue(dst, src.asVector2f()); break;
	case DECLTYPE_FLOAT3:  AssignValue(dst, src.asVector3f()); break;
	case DECLTYPE_FLOAT4:  AssignValue(dst, src.asVector4f()); break;
	case DECLTYPE_COLOR:  AssignValue(dst, src.asColor()); break;
	case DECLTYPE_UBYTE4:  AssignValue(dst, src.asUByte4()); break;
	case DECLTYPE_SHORT2:  AssignValue(dst, src.asShort2()); break;
	case DECLTYPE_SHORT4:  AssignValue(dst, src.asShort4()); break;
	case DECLTYPE_UBYTE4N:  AssignValue(dst, src.asUByte4()); break;
	case DECLTYPE_SHORT2N:  AssignValue(dst, src.asShort2()); break;
	case DECLTYPE_SHORT4N:  AssignValue(dst, src.asShort4()); break;
	case DECLTYPE_USHORT2N:  AssignValue(dst, src.asUShort2()); break;
	case DECLTYPE_USHORT4N:  AssignValue(dst, src.asUShort4()); break;
	case DECLTYPE_UDEC3:  AssignValue(dst, src.asU101010()); break;
	case DECLTYPE_DEC3N:  AssignValue(dst, src.asS101010()); break;
	case DECLTYPE_FLOAT16_2:  AssignValue(dst, src.asFloat16_2()); break;
	case DECLTYPE_FLOAT16_4:  AssignValue(dst, src.asFloat16_4()); break;
	case DECLTYPE_UNUSED:  
		break;
	}
}

void ImportMESH(  DAODumpStream& log
				, CHNKRef chnk
				, DAOMemStream& idxStream
				, DAOMemStream& vertStream
				)
{
	LPCTSTR nodeName = chnk->get_name().c_str();
	int nverts = chnk->get_Vertexcount();
	unsigned long vsize = chnk->get_Vertexsize();
	unsigned long voff = chnk->get_Vertexoffset();

	GFFListRef declList = chnk->get_Vertexdeclarator();
	DAOArray<DECLPtr> decls;
	if ( DynamicCast<DECL>(declList, decls) ) {
		for (DAOArray<DECLPtr>::iterator itr = decls.begin(); itr != decls.end(); ++itr) {
			DECLRef decl(*itr);
			int strmIdx = decl->get_Stream();
			if (strmIdx == -1)
				continue;

			ASSERT( 0 == strmIdx );
			ASSERT( 0 == decl->get_usageIndex() );
			ASSERT( 0 == decl->get_Method() );

			switch ( decl->get_usage() )
			{
			case DECLUSAGE_POSITION:		
				{
					DECLTYPE type = decl->get_dataType();
					long offset = decl->get_Offset();
					DAOArray< Vector4f > verts;
					verts.resize(nverts);
					for (int i = 0; i<nverts; ++i) {
						vertStream.Seek(SEEK_SET, voff + i * vsize + offset);
						Vector4f& vert = verts[i];
						AssignValue( vert, DECLValue(type, vertStream.dataptr()) );
						vert = vert;
					}
					Dump(log, "Position", verts);
				}
				break;
#if 0
			case DECLUSAGE_NORMAL:			
				{
					// We want to have one normal for each vertex (or control point),
					// so we set the mapping mode to eBY_CONTROL_POINT.
					LayerElementNormal* pNormLayer = LayerElementNormal::Create(pMesh, "");
					pNormLayer->SetMappingMode(LayerElement::eBY_CONTROL_POINT);
					pNormLayer->SetReferenceMode(LayerElement::eDIRECT);
					lLayer->SetNormals(pNormLayer);
					LayerElementArrayTemplate<Vector4>& aNorms = pNormLayer->GetDirectArray();
					aNorms.Resize(nverts);
					for (int i = 0; i<nverts; ++i) {
						vertStream.Seek(SEEK_SET, voff + i * vsize + decl->get_Offset());
						AssignValue( aNorms.GetAt(i), DECLValue(decl->get_dataType(), vertStream.dataptr()) );
					}
				}
				break;

			case DECLUSAGE_TEXCOORD:
				{
					LayerElementUV* lUVLayer=LayerElementUV::Create(pMesh, "UVChannel_1");
					lUVLayer->SetMappingMode(LayerElement::eBY_CONTROL_POINT);
					lUVLayer->SetReferenceMode(LayerElement::eDIRECT);
					lLayer->SetUVs(lUVLayer, LayerElement::eDIFFUSE_TEXTURES);
					LayerElementArrayTemplate<Vector2>& aUVs = lUVLayer->GetDirectArray();
					aUVs.Resize(nverts);
					for (int i = 0; i<nverts; ++i) {
						Vector2 &uvw = aUVs.GetAt(i);
						vertStream.Seek(SEEK_SET, voff + i * vsize + decl->get_Offset());
						AssignValue(uvw, DECLValue(decl->get_dataType(), vertStream.dataptr()) );

						if (uvw[0] < 0.0f || uvw[0] > 1.0f) uvw[0] -= floor(uvw[0]);
						if (uvw[1] < 0.0f || uvw[1] > 1.0f) uvw[1] -= floor(uvw[1]);
						if (flipUV) uvw[1] = 1.0f-uvw[1];

						//if (enableScale) 
						//	uvw.Set(uvw[0] * scaleFactor, uvw[1] * scaleFactor);
					}
				}
				break;
			case DECLUSAGE_TANGENT:			
				{
					LayerElementTangent* pTangentLayer = LayerElementTangent::Create(pMesh, "Tangents");
					pTangentLayer->SetMappingMode(LayerElement::eBY_CONTROL_POINT);
					pTangentLayer->SetReferenceMode(LayerElement::eDIRECT);
					lLayer->SetTangents(pTangentLayer);
					LayerElementArrayTemplate<Vector4>& aTangents = pTangentLayer->GetDirectArray();
					aTangents.Resize(nverts);
					for (int i = 0; i<nverts; ++i) {
						vertStream.Seek(SEEK_SET, voff + i * vsize + decl->get_Offset());
						AssignValue( aTangents.GetAt(i), DECLValue(decl->get_dataType(), vertStream.dataptr()) );
					}
				}
				break;

			case DECLUSAGE_BINORMAL:		
				{
					LayerElementBinormal* pBinormLayer = LayerElementBinormal::Create(pMesh, "Binormals");
					pBinormLayer->SetMappingMode(LayerElement::eBY_CONTROL_POINT);
					pBinormLayer->SetReferenceMode(LayerElement::eDIRECT);
					lLayer->SetBinormals(pBinormLayer);
					LayerElementArrayTemplate<Vector4>& aBinorms = pBinormLayer->GetDirectArray();
					aBinorms.Resize(nverts);
					for (int i = 0; i<nverts; ++i) {
						vertStream.Seek(SEEK_SET, voff + i * vsize + decl->get_Offset());
						AssignValue( aBinorms.GetAt(i), DECLValue(decl->get_dataType(), vertStream.dataptr()) );
					}
				}
				break;
				//case DECLUSAGE_BLENDWEIGHT:		AddVertexBlendWeight(pMesh, decl, value);	break;
				//case DECLUSAGE_BLENDINDICES:	AddVertexBlendIndices(pMesh, decl, value);	break;
				//case DECLUSAGE_PSIZE:			AddVertexPSize(pMesh, decl, value);			break;
				//case DECLUSAGE_TESSFACTOR:		AddVertexTessFactor(pMesh, decl, value);	break;
				//case DECLUSAGE_POSITIONT:		AddVertexPositionT(pMesh, decl, value);		break;
			case DECLUSAGE_COLOR:			
				{
					LayerElementVertexColor* pLayer = LayerElementVertexColor::Create(pMesh, "VertexColors");
					pLayer->SetMappingMode(LayerElement::eBY_CONTROL_POINT);
					pLayer->SetReferenceMode(LayerElement::eDIRECT);
					lLayer->SetVertexColors(pLayer);
					LayerElementArrayTemplate<Color>& aColors = pLayer->GetDirectArray();
					aColors.Resize(nverts);
					for (int i = 0; i<nverts; ++i) {
						vertStream.Seek(SEEK_SET, voff + i * vsize + decl->get_Offset());
						AssignValue( aColors.GetAt(i), DECLValue(decl->get_dataType(), vertStream.dataptr()) );
					}
				}
				break;
				//case DECLUSAGE_FOG:				AddVertexFog(pMesh, decl, value);			break;
				//case DECLUSAGE_DEPTH:			AddVertexDepth(pMesh, decl, value);			break;
				//case DECLUSAGE_SAMPLE:			AddVertexSample(pMesh, decl, value);		break;
			case DECLUSAGE_UNUSED:			
				break;
#endif
			}
		}
	}

	ASSERT( 0 == chnk->get_Indexformat());
	ASSERT( 0 == chnk->get_Basevertexindex());
	ASSERT( 0 == chnk->get_Minindex());

	DAOArray< long > verts;
	verts.resize(nverts);

	idxStream.Seek(SEEK_SET, sizeof(short) * chnk->get_Startindex());
	UShort3 * pFaces = (UShort3*)idxStream.dataptr();
	int nfaces = chnk->get_Indexcount() / 3;
	DumpArray(log, "Indices", pFaces, nfaces);
	for (int i=0; i<nfaces; ++i) {
		UShort3& t = pFaces[i];
		++verts[t[0]];
		++verts[t[1]];
		++verts[t[2]];
		//pMesh->BeginPolygon(i);
		//pMesh->AddPolygon(t[0]);
		//pMesh->AddPolygon(t[1]);
		//pMesh->AddPolygon(t[2]);
		//pMesh->EndPolygon ();
	}
	Dump(log, "VertInfo", verts);

}
//
//void ImportBones( DAODumpStream& log, Matrix44 pm, NODERef node)
//{
//	int indent = log.IncreaseIndent();
//	try 
//	{
//		Text name = !node.isNull() ? node->get_name() : Text();
//
//		GFFListRef childList = node->get_children();
//
//		Vector4f trans = node->GetLocalTranslation();
//		Quaternion quat = node->GetLocalRotation();
//		float scale = node->GetLocalScale();
//		Matrix44 tm(trans, quat, scale);
//
//		float len = trans.Length();
//
//
//		Matrix44 im = pm * tm;
//
//		//[-0.161288,0.000001,0.986907]
//		//[-0.000001,-1.000000,0.000001]
//		//[0.986907,-0.000001,0.161288]
//		//[-0.002451,0.000000,0.371050]
//
//		Matrix44& dm = im;
//
//		Text txtPos, txtRot, txtYpr, txtScale, txtFormat, txtMatrix;
//		txtPos.Format("(%7.4f, %7.4f, %7.4f)", trans[0], trans[1], trans[2]);
//		txtRot.Format("(%7.4f, %7.4f, %7.4f, %7.4f)", quat.w, quat.x, quat.y, quat.z);
//		txtFormat.Format("%%-%ds | %%-24s | %%-32s\n", max(60 - indent*2, 1) );
//		//log.Indent();
//		//log.PrintF(txtFormat, name.c_str(), txtPos.c_str(), txtRot.c_str());
//
//
//		Text txtPos0, txtPos1, txtPos2, txtPos3;
//		Vector4f r0 = dm.GetRow(0);
//		Vector4f r1 = dm.GetRow(1);
//		Vector4f r2 = dm.GetRow(2);
//		Vector4f r3 = dm.GetRow(3);
//		txtPos0.Format("[%7.4f,%7.4f,%7.4f]", r0.x, r0.y, r0.z);
//		txtPos1.Format("[%7.4f,%7.4f,%7.4f]", r1.x, r1.y, r1.z);
//		txtPos2.Format("[%7.4f,%7.4f,%7.4f]", r2.x, r2.y, r2.z);
//		txtPos3.Format("[%7.4f,%7.4f,%7.4f]", r3.x, r3.y, r3.z);
//
//		txtFormat.Format("%%-%ds | %%s %%s %%s %%s\n", max(60 - indent*2, 1) );
//		log.Indent();
//		log.PrintF(txtFormat, name.c_str(), txtPos0.c_str(), txtPos1.c_str(), txtPos2.c_str(), txtPos3.c_str());
//
//
//		DAOArray< NODEPtr > children;
//		if ( DynamicCast(childList, children) ) {
//			for (size_t i=0, n=children.size(); i<n; ++i)
//				ImportBones(log, im, NODERef(children[i]));
//		}
//	}
//	catch( std::exception & ) 
//	{
//	}
//	catch( ... ) 
//	{
//	}
//	log.DecreaseIndent();
//}

void ImportMesh(MESHRef root)
{
	DAODumpStream out(NULL, false);

	DAOMemStream vertStream;
	DAOMemStream idxStream;
	GFFListRef idxList = root->get_indexdata();
	GFFListRef vertList = root->get_vertexdata();
	idxStream.Open(idxList->pdata(), idxList->size());
	vertStream.Open(vertList->pdata(), vertList->size());

	DAOArray<CHNKPtr> chunks;
	if ( DynamicCast<CHNK>(root->get_chunks(), chunks) ) {
		for (DAOArray<CHNKPtr>::iterator itr = chunks.begin(); itr != chunks.end(); ++itr) {
			CHNKRef chnk(*itr);
			ImportMESH(out, chnk, idxStream, vertStream);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

static void HelpString(Cmd::HelpType type){
	switch (type)
	{
	case Cmd::htShort: cout << "DumpMesh - Dump MESH Mesh information to console"; break;
	case Cmd::htLong:  
		{
			TCHAR fullName[MAX_PATH], exeName[MAX_PATH];
			GetModuleFileName(NULL, fullName, MAX_PATH);
			_tsplitpath(fullName, NULL, NULL, exeName, NULL);
			cout << "Usage: " << exeName << " erf [-opts[modifiers]] <erf file>" << endl 
				<< "  Simple skeleton dump help for DA:O MESH files." << endl
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
		MESHFile file;
		file.open(fullpath);
		MESHPtr root = file.get_Root();

		ImportMesh( MESHRef(root) );
	}

	return false;
}

REGISTER_COMMAND(DumpMesh, HelpString, ExecuteCmd);
