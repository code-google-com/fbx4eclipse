/**********************************************************************
*<
FILE: ExporterCore.cpp

DESCRIPTION:	Core Export helper routines

HISTORY: 

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include "DAOExport.h"
#include "MMHExport.h"
#include "MMHFormat.h"
#include "MaxConvert.h"
#include <limits.h>


using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;

struct MMHExportImpl
{
   MMHExportImpl(MMHExporter *owner) 
      : o(owner)
      , mExportHidden(true)
	  , scaleFactor(1.0)
	  , root(NULL)
   {
   }
   ~MMHExportImpl()
   {
   }

   Matrix3 GetObjTMAfterWSM(INode *node, TimeValue t=0)
   {
      Matrix3 mtx = node->GetObjTMAfterWSM(0);
      if (scaleFactor != 1.0f)
         mtx *= ScaleMatrix(Point3(scaleFactor, scaleFactor, scaleFactor));
      return mtx;
   }
   bool WalkObjectList(INode *node);
   bool exportBone(INode *node);

   MMHExporter *o;
   DAO::MMH::MMHFile file;
   DAO::MMH::MDLHPtr root;
   DAODumpStream log;
   bool mExportHidden;
   float scaleFactor;
};

MMHExporter::MMHExporter(const TCHAR *Name,ExpInterface *I,Interface *GI, BOOL SuppressPrompts, DWORD options)
   : BaseExporter()
{
   BaseInit(Name,I,GI,SuppressPrompts, options);
}

MMHExporter::MMHExporter()
{
}

void MMHExporter::Initialize()
{
}

bool MMHExporter::DoExport()
{
	MMHExportImpl impl(this);

	Text txt;
	txt.Format("C:\\tmp\\%s-1.log", PathFindFileName(name.c_str()));
	impl.log.Open(txt.c_str(), false);

	bool ok = impl.WalkObjectList( gi->GetRootNode() );

   //MMHExportImpl impl(this);
   return true;
}


bool MMHExportImpl::WalkObjectList(INode *node)
{
	log.IncreaseIndent();
	// Abort if is hidden and we are not exporting hidden
	bool ignore = false;
	if (node->IsHidden() && !mExportHidden)
		ignore = true;

	// Dont really like this check but it works.
	if (o->options&SCENE_EXPORT_SELECTED)
	{
		ignore = true;
		for (int i=0, n = o->gi->GetSelNodeCount(); i<n; ++i) {
			if (node == o->gi->GetSelNode(i)) {
				ignore = false;
				break;
			}
		}
	}
	if (!ignore)
	{
		TimeValue t = 0;
		ObjectState os = node->EvalWorldState(t); 

		// Always skip bones and bipeds
		SClass_ID scid = node->SuperClassID();
		Class_ID ncid = node->ClassID();
		TSTR nodeClass; node->GetClassName(nodeClass);
		if (node->IsBoneShowing())
		{
			exportBone(node);
		}
		else if (os.obj && os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID)
		{
			TSTR objClass;
			os.obj->GetClassName(objClass);
			SClass_ID oscid = os.obj->SuperClassID();
			Class_ID oncid = os.obj->ClassID();
			if (  os.obj 
				&& (  os.obj->ClassID() == BONE_OBJ_CLASSID 
				|| os.obj->ClassID() == Class_ID(BONE_CLASS_ID,0)
				|| os.obj->ClassID() == Class_ID(0x00009125,0) /* Biped Twist Helpers */
				)
				) 
			{
				exportBone(node);
			} 
			else
			{
				if (TriObject *tri = (TriObject *)os.obj->ConvertToType(t, Class_ID(TRIOBJ_CLASS_ID, 0)))
				{
					// Mesh Export
				}
			}
		}
		else if (os.obj && os.obj->SuperClassID()==HELPER_CLASS_ID)
		{
			//if (os.obj->ClassID() == Class_ID(POINTHELP_CLASS_ID,0))
			//{
			//	if (wildmatch(hookMatch, node->GetName())) {
			//		BuildHOOK(node);
			//	} else if (wildmatch(hairMatch, node->GetName())) {
			//		BuildHAIR(node);
			//	} else if (wildmatch(helmMatch, node->GetName())) {
			//		BuildHELM(node);
			//	}
			//}
			exportBone(node);
		}
	}
	bool ok = true;
	for (int i=0; i<node->NumberOfChildren() && ok; i++) {
		ok = WalkObjectList(node->GetChildNode(i));
	}
	log.DecreaseIndent();
	return true;
}

bool MMHExportImpl::exportBone( INode *node )
{
	Matrix3 tm = node->GetNodeTM(0);
	Matrix3 ltm = GetNodeLocalTM(node);
	
	Matrix44 dm = TOMATRIX4(ltm);

	LPCSTR name = node->GetName();

	Vector3f trans;
	Quaternion quat;
	Vector3f scale;
	dm.Decompose(trans, quat, scale);

	Text txtFormat;

	Text txtPos, txtRot, txtYpr, txtScale;
	txtPos.Format("(%7.4f, %7.4f, %7.4f)", trans[0], trans[1], trans[2]);
	txtRot.Format("(%7.4f, %7.4f, %7.4f, %7.4f)", quat.w, quat.x, quat.y, quat.z);
	txtScale.Format("(%7.4f, %7.4f, %7.4f)", scale[0], scale[1], scale[2]);
	txtFormat.Format("%%-%ds | %%-36s | %%-46s | %%-36s\n", max(60 - log.get_Indent()*2, 1) );
	//log.Indent();
	//log.PrintF(txtFormat, name, txtPos.c_str(), txtRot.c_str(), txtScale.c_str());

	Matrix3& dm3 = tm;

	Text txtPos0, txtPos1, txtPos2, txtPos3;
	Vector4f r0 = dm3.GetRow(0);
	Vector4f r1 = dm3.GetRow(1);
	Vector4f r2 = dm3.GetRow(2);
	Vector4f r3 = dm3.GetRow(3);

	txtPos0.Format("[%7.4f,%7.4f,%7.4f]", r0.x, r0.y, r0.z);
	txtPos1.Format("[%7.4f,%7.4f,%7.4f]", r1.x, r1.y, r1.z);
	txtPos2.Format("[%7.4f,%7.4f,%7.4f]", r2.x, r2.y, r2.z);
	txtPos3.Format("[%7.4f,%7.4f,%7.4f]", r3.x, r3.y, r3.z);

	txtFormat.Format("%%-%ds | %%s %%s %%s %%s\n", max(60 - log.get_Indent()*2, 1) );
	log.Indent();
	log.PrintF(txtFormat, name, txtPos0.c_str(), txtPos1.c_str(), txtPos2.c_str(), txtPos3.c_str());

	return true;
}