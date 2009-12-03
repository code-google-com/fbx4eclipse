/**********************************************************************
*<
FILE: ImporterCore.cpp

DESCRIPTION:	Core Import helper routines

HISTORY: 

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include "DAOImport.h"
#include "MSHImport.h"
#include "MMHImport.h"
#include <set>

#include "MSHFormat.h"
#if VERSION_3DSMAX > ((5000<<16)+(15<<8)+0) // Version 5
#  include "MeshNormalSpec.h"
#endif
#include <stdexcept>
using std::runtime_error;

extern ClassDesc2* GetDAOImportDesc();
static const Class_ID GNORMAL_CLASS_ID(0x243e22c6, 0x63f6a014);

using namespace DAO;

typedef std::map<std::string, INodeTab, ltstr> NameNodeMap;
typedef NameNodeMap::iterator NameNodeItr;

struct MSHImportImpl
{
   MSHImportImpl(MSHImporter *owner) : o(owner) {}
   //INode *ImportMESH(IMESH& rigd);
   //INode *ImportSKIN(SKIN& skin);
   //INode *ImportWALK(WALK& walk);
   //INode *ImportHOOK(HOOK& hook);
   //INode *ImportHAIR(HAIR& hair);
   //INode *ImportHELM(HELM& helm);
   //bool ImportCOLS(COLS& cols, INodeTab& spheres);
   //StdMat2* CreateMaterial(DAO::IMESH& mesh);
   //StdMat2* CreateMaterial(DAO::Material& mtl, LPCTSTR name, LPCTSTR diffuse, LPCTSTR normal, LPCTSTR filterColor);
   //Texmap* CreateTexture(LPCTSTR name);
   //Texmap* CreateNormalBump(LPCTSTR name, Texmap* nmap);
   //MultiMtl * CreateWalkMaterial(LPCTSTR name);
   //void BuildMaterialFlags(INode* node, MaterialFlag uiFlags);
   //TSTR lastSkeleton; //! Last known skeleton imported (for COLS)
   //NameNodeMap boneMap; 
   MSHImporter *o;

   BOOL enableColl;
   BOOL enableWalk;
   BOOL enableHook;
   BOOL enableHair;
   BOOL enableBones;
   BOOL enableScale;
   float scaleFactor;
   BOOL normalUnderBump;
   BOOL enableWeld;
   float weldLimit;
};

MSHImporter::MSHImporter(const TCHAR *Name,ImpInterface *I,Interface *GI, BOOL SuppressPrompts)
   : BaseImporter()
{
   BaseInit(Name,I,GI,SuppressPrompts);
}

MSHImporter::MSHImporter()
{
}

void MSHImporter::Initialize()
{
}

bool MSHImporter::DoImport()
{
   MSHImportImpl impl(this);
#if 0
   NWStream in(name.c_str(), true);

   MSHHeader hdr;
   if (!Recall(in, hdr)) 
   {
      if (hdr.Signature != "DAO") {
         throw runtime_error(FormatString("Invalid File Signature for file: %s", name.c_str()));
      }
      if (hdr.MajorVersion != MSHPacketMajorVersionDefault && hdr.MinorVersion != MSHPacketMinorVersionDefault) {
         throw runtime_error(FormatString("Unknown File Version (%d.%d) expecting (%d.%d)",
            hdr.MajorVersion, hdr.MinorVersion,
            MSHPacketMajorVersionDefault, MSHPacketMinorVersionDefault
            ));
      }
      throw runtime_error(FormatString("Invalid File Header for file: %s", name.c_str()));
   ParamBlockDesc2 *params = GetDAOImportDesc()->GetParamBlockDescByName("parameters");
   impl.enableColl = params->GetInt(IMP_PB_ENABLE_COLLISION);
   impl.enableWalk = params->GetInt(IMP_PB_ENABLE_WALKMESH);
   impl.enableHook = params->GetInt(IMP_PB_ENABLE_HOOKS);
   impl.enableHair = params->GetInt(IMP_PB_ENABLE_HAIRHELM);
   impl.enableBones = params->GetInt(IMP_PB_ENABLE_BONES);
   impl.enableScale = params->GetInt(IMP_PB_ENABLE_SCALE);
   impl.scaleFactor = params->GetFloat(IMP_PB_SCALE_FACTOR) / 100.0;
   impl.normalUnderBump = params->GetInt(IMP_PB_NORMALBUMP);
   impl.enableWeld = params->GetInt(IMP_PB_ENABLE_WELD);
   impl.weldLimit = params->GetFloat(IMP_PB_WELD_LIMIT);

   for (int i=0; i<hdr.Packets.count(); ++i)
   {
      MSHPacket* packet = hdr.Packets[i];
      if (packet->Signature == "RIGD")
      {
         INode *node = impl.ImportMESH(*(RIGD*)packet);
         if (node != NULL) {
            node->SetUserPropString("type", packet->Signature.c_str());
         }
      }
      else if (packet->Signature == "SKIN")
      {
         INode *node = impl.ImportSKIN(*(SKIN*)packet);
         if (node != NULL) {
            node->SetUserPropString("type", packet->Signature.c_str());
         }
      }
      else if (impl.enableColl && packet->Signature == "COL2")
      {
         INode *node = impl.ImportMESH((*(COL2*)packet));
         if (node != NULL) {
			   node->SetUserPropString("type", packet->Signature.c_str());
            node->SetRenderable(FALSE);
            node->XRayMtl(TRUE);
         }
      }
      else if (impl.enableColl && packet->Signature == "COL3")
      {
         INode *node = impl.ImportMESH((*(COL3*)packet));
         if (node != NULL) {
            node->SetUserPropString("type", packet->Signature.c_str());
            node->SetRenderable(FALSE);
            node->XRayMtl(TRUE);
         }
      }
      else if (impl.enableColl && packet->Signature == "COLS")
      {
         if (impl.enableBones)
         {
            INodeTab spheres;
            impl.ImportCOLS((*(COLS*)packet), spheres);
         }
      }
      else if (impl.enableWalk && packet->Signature == "WALK")
      {
         INode *node = impl.ImportWALK((*(WALK*)packet));
         if (node != NULL) {
            node->SetUserPropString("type", packet->Signature.c_str());
            node->SetRenderable(FALSE);
            node->XRayMtl(FALSE);
            node->IgnoreExtents(TRUE);
         }
      }
      else if (impl.enableHook && packet->Signature == "HOOK")
      {
         INode *node = impl.ImportHOOK(*(HOOK*)packet);
         if (node != NULL) {
            node->SetUserPropString("type", packet->Signature.c_str());
            node->SetRenderable(FALSE);
            node->IgnoreExtents(TRUE);
         }
      }
      else if (impl.enableHair && packet->Signature == "HAIR")
      {
         INode *node = impl.ImportHAIR(*(HAIR*)packet);
         if (node != NULL) {
            node->SetUserPropString("type", packet->Signature.c_str());
            node->SetRenderable(FALSE);
            node->IgnoreExtents(TRUE);
         }
      }
      else if (impl.enableHair && packet->Signature == "HELM")
      {
         INode *node = impl.ImportHELM(*(HELM*)packet);
         if (node != NULL) {
            node->SetUserPropString("type", packet->Signature.c_str());
            node->SetRenderable(FALSE);
            node->IgnoreExtents(TRUE);
         }
      }
   }
#endif
   return true;
}
#if 0
void MSHImportImpl::BuildMaterialFlags(INode* node, MaterialFlag uiFlags)
{
   node->SetUserPropString("UIFlag", FlagsToString(uiFlags, MaterialFlags));

   node->SetUserPropInt("trans", (uiFlags & DAO_ALPHA_TEST) ? 1 : 0);
   node->SetUserPropInt("alphablend", (uiFlags & DAO_ALPHA_BLEND) ? 1 : 0);
   node->SetUserPropInt("addblend", (uiFlags & DAO_ADDITIVE_BLEND) ? 1 : 0);

   if (uiFlags & DAO_ENVIRONMENT_MAPPED)
      node->SetUserPropInt("envmap", 1);
      
   if (uiFlags & DAO_CUTSCENE_MESH)
      node->SetUserPropInt("head", 1);

   if (uiFlags & DAO_GLOW)
      node->SetUserPropInt("nolight", 1);

   if (uiFlags & DAO_NO_CAST_SHADOWS)
      node->SetUserPropInt("nocast", 1);

   if (uiFlags & DAO_PROJECTED_TEXTURES)
      node->SetUserPropInt("projtexture", 1);
}

INode *MSHImportImpl::ImportMESH(IMESH& imesh)
{
   ImpNode *node = o->i->CreateNode();
   if(!node) throw runtime_error("Unable to create ImpNode");

   ParamBlockDesc2 *params = GetDAOImportDesc()->GetParamBlockDescByName("parameters");

   INode *inode = node->GetINode();
   TriObject *triObject = CreateNewTriObject();
   node->Reference(triObject);
   node->SetName(imesh.getName().c_str());

   Mesh& mesh = triObject->GetMesh();
   int nverts = imesh.getNVerts();
   mesh.setNumVerts(nverts);
   mesh.setNumTVerts(nverts, TRUE);

   bool flipUV = params->GetInt(IMP_PB_FLIPUV) ? true : false;
   for (int i=0; i < nverts; ++i){
      mesh.verts[i] = imesh.getVert(i).getPosition();
	  if (enableScale) mesh.verts[i] *= scaleFactor;

      Point3 uvw = imesh.getVert(i).getUVW();
      if (flipUV) uvw.y = -uvw.y;

      mesh.tVerts[i] = uvw;
   }
   NWArray<DAO::TRI>& faces = imesh.getFaces();
   int nfaces = faces.size();
   mesh.setNumFaces(nfaces);
   mesh.setNumTVFaces(nfaces);
   for (int i=0; i<nfaces; ++i) {
      DAO::TRI& t = faces[i];
      Face& f = mesh.faces[i];
      f.setVerts(t.a, t.b, t.c);
      f.Show();
      f.setEdgeVisFlags(EDGE_VIS, EDGE_VIS, EDGE_VIS);
      TVFace& tf = mesh.tvFace[i];
      tf.setTVerts(t.a, t.b, t.c);
   }
#if VERSION_3DSMAX > ((5000<<16)+(15<<8)+0) // Version 5
   //  mesh.buildNormals();
   mesh.SpecifyNormals();
   MeshNormalSpec *specNorms = mesh.GetSpecifiedNormals();
   if (NULL != specNorms)
   {
      specNorms->ClearAndFree();
      specNorms->SetNumFaces(faces.size());
      specNorms->SetNumNormals(nverts);
      Point3* norms = specNorms->GetNormalArray();
      for (int i=0; i<nverts; i++){
         norms[i] = imesh.getVert(i).getNormal();
      }
      MeshNormalFace* pFaces = specNorms->GetFaceArray();
      for (int i=0; i<faces.size(); i++){
         DAO::TRI& t = faces[i];
         pFaces[i].SpecifyNormalID(0, t.a);
         pFaces[i].SpecifyNormalID(1, t.b);
         pFaces[i].SpecifyNormalID(2, t.c);
      }
      specNorms->SetAllExplicit(true);
      specNorms->CheckNormals();
   }
#endif
   mesh.RemoveDegenerateFaces();
   mesh.RemoveIllegalFaces();

   if (enableWeld)
   {
	   MeshDelta tmd(mesh);
	   BitArray vTempSel;
	   vTempSel.SetSize(nverts);
	   vTempSel.SetAll();
	   float thresh = weldLimit;
	   //if (enableScale) thresh *= scaleFactor;
	   tmd.WeldByThreshold(mesh, vTempSel, thresh);
	   tmd.Apply(mesh);
   }

   if (imesh.hasMaterial())
   {
      BuildMaterialFlags(inode, imesh.getMaterial().uiFlags);


      StdMat2 *m = CreateMaterial(imesh);
      if (m != NULL){
         o->gi->GetMaterialLibrary().Add(m);
         inode->SetMtl(m);
      }
   }

   o->i->AddNodeToScene(node);

   inode = node->GetINode();
   inode->EvalWorldState(0);

   if (params->GetInt(IMP_PB_AUTOSMOOTH)) {
      mesh.AutoSmooth(TORAD(params->GetFloat(IMP_PB_AUTOSMOOTH_ANGLE)), FALSE, FALSE);
   }
   return inode;
}

INode *MSHImportImpl::ImportWALK(WALK& walk)
{
   ImpNode *node = o->i->CreateNode();
   if(!node) throw runtime_error("Unable to create ImpNode");

   ParamBlockDesc2 *params = GetDAOImportDesc()->GetParamBlockDescByName("parameters");
   
   float limit = params->GetInt(IMP_PB_TRIM_WALKMESH) ? params->GetFloat(IMP_PB_WALKMESH_LIMIT) : -1000000.0f;

   INode *inode = node->GetINode();
   TriObject *triObject = CreateNewTriObject();
   node->Reference(triObject);
   node->SetName(walk.Name.c_str());

   BuildMaterialFlags(inode, walk.uiFlags);

   Mesh& mesh = triObject->GetMesh();

   MultiMtl* m = CreateWalkMaterial(walk.Name.c_str());
   if (m != NULL){
      o->gi->GetMaterialLibrary().Add(m);
      inode->SetMtl(m);
   }

   int nverts = walk.verts.size();
   mesh.setNumVerts(nverts);
   for (int i=0; i < nverts; ++i){
      mesh.verts[i] = walk.verts[i];
	  if (enableScale) mesh.verts[i] *= scaleFactor;
      if (mesh.verts[i].z <= -1000000.0f)
         mesh.verts[i].z = limit;
   }
   double log2 = log(2.0);
   NWArray<DAO::WTRI>& faces = walk.faces;
   int nfaces = faces.size();
   mesh.setNumFaces(nfaces*2);
   for (int i=0; i<nfaces; ++i) {
      DAO::WTRI& t = faces[i];
      Face& f = mesh.faces[i];
      f.setVerts(t.a, t.b, t.c);
      f.Show();
      f.setEdgeVisFlags(EDGE_VIS, EDGE_VIS, EDGE_VIS);

      int mtlid = 0;
      if (t.flags != 0)
      {
         mtlid = int(log(double(t.flags >> 3))/log2) + 1;
      }
      f.setMatID(mtlid);
   }
   mesh.RemoveDegenerateFaces();
   mesh.RemoveIllegalFaces();

   o->i->AddNodeToScene(node);

   inode = node->GetINode();
   inode->EvalWorldState(0);
   return inode;
}

INode *MSHImportImpl::ImportSKIN(SKIN& skin)
{
	BOOL enableWeld = this->enableWeld; this->enableWeld = FALSE;

   INode *node = ImportMESH(skin);

   this->enableWeld = enableWeld;
   if (node != NULL && enableBones) 
   {
      lastSkeleton = skin.Skeleton;

      //node->SetUserPropString("Skeleton", TSTR(skin.Skeleton));
      node->SetUserPropString("skelfile", TSTR(skin.Skeleton));

      INodeTab bones;
      NameNodeItr itr = boneMap.find( string(skin.Skeleton) );
      if (itr != boneMap.end())
      {
         // Already imported just reuse
         bones = (*itr).second;
      }
      else
      {
         bool faceMode = (skin.hasMaterial() && skin.getMaterial().uiFlags & DAO_CUTSCENE_MESH);

         stringlist zipFiles = TokenizeString(GetSetting<string>("MMHZip").c_str(), ";");
		 ExpandFileList(zipFiles);
         MMHImporter mmhimp(skin.Skeleton, o->i, o->gi, o->suppressPrompts);
         for (stringlist::iterator itr = zipFiles.begin(), end = zipFiles.end(); itr != end; ++itr ) 
         {
            if (mmhimp.ImportFromZip((*itr).c_str(), bones, true, faceMode))
            {
               boneMap[string(skin.Skeleton)] = bones;
               break;
            }
         }
      }
      if (bones.Count() > 0)
      {
         Modifier *skinMod = GetOrCreateSkin(node);
         TriObject *triObject = GetTriObject(node->GetObjectRef());
         Mesh& m = triObject->GetMesh();
         if (ISkin *iskin = (ISkin *) skinMod->GetInterface(I_SKIN))
         {
            ISkinImportData* iskinImport = (ISkinImportData*) skinMod->GetInterface(I_SKINIMPORTDATA);

            // Set the num weights to 4.  Yes its in the nif but Shon doesn't like to expose those values 
            //   and the value always seems to be 4 anyway.  I'd also this be more dynamic than hard coded numbers
            //   but I cant figure out the correct values to pass the scripting engine from here so I'm giving up.
#if VERSION_3DSMAX >= ((5000<<16)+(15<<8)+0) // Version 5+
            int numWeightsPerVertex = 4;
            IParamBlock2 *params = skinMod->GetParamBlockByID(2/*advanced*/);
            params->SetValue(0x7/*bone_Limit*/, 0, numWeightsPerVertex);
#endif

            // Can get some truly bizarre animations without this in MAX with Civ4 Leaderheads
#if VERSION_3DSMAX > ((5000<<16)+(15<<8)+0) // Version 6+
            BOOL ignore = TRUE;
            params->SetValue(0xE/*ignoreBoneScale*/, 0, ignore);
#endif
            std::set<size_t> blist;
            int nverts = skin.verts.size();
            for (int i=0; i<nverts; ++i)
            {
               SVERT& vert = skin.verts[i];
               int nbones = int(vert.BoneCount);
               for (int j=0; j<nbones; ++j)
               {
                  int index = vert.BoneIndex[j];
                  float weight = vert.BoneWeights[j];
                  if (weight != 0.0f)
                  {
                     if (blist.end() == blist.find(index))
                     {
                        INode *boneRef = bones[index];
                        iskinImport->AddBoneEx(boneRef, FALSE);
                        blist.insert(index);
                     }
                  }
               }
            }
            ObjectState os = node->EvalWorldState(0);

#if 0 //VERSION_3DSMAX < ((5000<<16)+(15<<8)+0) // Version 4
            o->gi->SetCommandPanelTaskMode(TASK_MODE_MODIFY);
            o->gi->SelectNode(node);

            INodeTab boneList;
            Tab<float> weights;
            for (std::set<size_t>::iterator bitr=blist.begin(); bitr!=blist.end(); ++bitr)
            {
               int index = (*bitr);
               INode *boneRef = bones[index];
               boneList.Append(1, &boneRef);
            }
            weights.Resize( boneList.Count());
            float zerof = 0.0f;
            for (int j=0; j<boneList.Count(); ++j)
               weights.Append(1, &zerof);

            for (int i=0; i<nverts; ++i)
            {
               for (int j=0; j<weights.Count(); ++j)
                  weights[j] = 0.0f;

               SVERT& vert = skin.verts[i];
               int nbones = int(vert.BoneCount);
               for (int j=0; j<nbones; ++j)
               {
                  int index = vert.BoneIndex[j];
                  float weight = vert.BoneWeights[j];
                  if (weight != 0.0f)
                  {
                     int idx = std::distance(blist.begin(), blist.find(index));
                     weights[idx] = weight;
                  }
               }
               BOOL add = iskinImport->AddWeights(node, i, boneList, weights);
               add = add;
            }
#else
            for (int i=0; i<nverts; ++i)
            {
               SVERT& vert = skin.verts[i];
               INodeTab boneList;
               Tab<float> weights;
               int nbones = int(vert.BoneCount);
               for (int j=0; j<nbones; ++j)
               {
                  int index = vert.BoneIndex[j];
                  float weight = vert.BoneWeights[j];
                  if (weight != 0.0f)
                  {
                     INode *boneRef = bones[index];
                     boneList.Append(1, &boneRef);
                     weights.Append(1, &weight);
                  }
               }
               BOOL add = iskinImport->AddWeights(node, i, boneList, weights);
               add = add;
            }
#endif
            node->EvalWorldState(0);
         }            
      }
   }
   return node;
}


Texmap* MSHImportImpl::CreateTexture(LPCTSTR name)
{
   BitmapManager *bmpMgr = TheManager;

   TSTR filename = FormatText("%s.dds", name);
   if (bmpMgr->CanImport(filename))
   {
      BitmapTex *bmpTex = NewDefaultBitmapTex();
      bmpTex->SetName(name);
      bmpTex->SetMapName(filename);
      bmpTex->SetAlphaAsMono(TRUE);
      bmpTex->SetAlphaSource(ALPHA_FILE);
      return bmpTex;
   }
   return NULL;
}

Texmap* MSHImportImpl::CreateNormalBump(LPCTSTR name, Texmap* nmap)
{
   Texmap *texmap = (Texmap*)o->gi->CreateInstance(TEXMAP_CLASS_ID, GNORMAL_CLASS_ID);
   if(texmap != NULL)
   {
      texmap->SetName(name);
      texmap->SetSubTexmap(0, nmap);
      return texmap;
   }
   return nmap;
}

StdMat2* MSHImportImpl::CreateMaterial(DAO::IMESH& mesh)
{
   if (!mesh.hasMaterial())
      return NULL;

   DAO::Material& mtl = mesh.getMaterial();
   LPCTSTR name = mesh.getName();
   LPCTSTR diffuse = mtl.DiffuseMap;
   LPCTSTR normal = mtl.NormalMap;
   LPCTSTR filterColor = mtl.TintMap;
   LPCTSTR glow = mtl.GlowMap;

   StdMat2 *m = NewDefaultStdMat();
   m->SetName(name);
   m->SetAmbient(mtl.Kd,0);
   m->SetDiffuse(mtl.Kd,0);
   m->SetSpecular(mtl.Ks,0);

   ParamBlockDesc2 *params = GetDAOImportDesc()->GetParamBlockDescByName("parameters");
   if (params->GetInt(IMP_PB_RENDERTEX)) {
      m->SetMtlFlag(MTL_DISPLAY_ENABLE_FLAGS, TRUE);
   }
   m->SetShinStr(mtl.specularValue,0);
   m->SetShininess(mtl.specularPower/100.0f, 0);

   if (diffuse && *diffuse)
      m->SetSubTexmap(ID_DI, CreateTexture(diffuse));
   if (normal && *normal) {
      //m->SetTexmapAmt(ID_BU, 1.0f, 0); // 100% seems like too much for true normal bumps. Use default value
	   Texmap* tex = CreateTexture(normal);
	   if (normalUnderBump)
		   tex = CreateNormalBump(normal, tex);
      m->SetSubTexmap(ID_BU, tex);
   }
   if (filterColor && *filterColor) {
      m->SetSubTexmap(ID_FI, CreateTexture(filterColor));
   }
   if (glow && *glow)
      m->SetSubTexmap(ID_SI, CreateTexture(glow));

   return m;
}

//bit 0: walk bit ( 1 = walk, 0 = no walk )
//bit 1 - 2: reserved
//bit 3 - 13: material ( used for footstep sounds, etc... I dont know what will happen if more than one of these are set at the same time. I dont recommend it. )...
//bit 3: dirt
//bit 4: grass
//bit 5: stone
//bit 6: wood
//bit 7: carpet
//bit 8: metal
//bit 9: swamp
//bit 10: mud
//bit 11: leaves
//bit 12: water
//bit 13: puddles
MultiMtl * MSHImportImpl::CreateWalkMaterial(LPCTSTR name)
{
   ParamBlockDesc2 *params = GetDAOImportDesc()->GetParamBlockDescByName("parameters");
   float opacity = params->GetFloat(IMP_PB_WALK_OPACITY);
   if (opacity == 0.0f) opacity = 1.0f;
   
   MultiMtl *m = NewDefaultMultiMtl();
   m->SetName(name);
   m->SetNumSubMtls(0);
   if (StdMat2* sm = NewDefaultStdMat())
   {
      sm->SetName("NoWalk");
      sm->SetFaceted(TRUE);
      sm->SetDiffuse(Color(0.8f,0.8f,0.8f), 0);
      sm->SetOpacity(opacity, 0);
      m->SetSubMtlAndName(0, sm, TSTR("NoWalk"));
   }
   if (StdMat2* sm = NewDefaultStdMat())
   {
      sm->SetName("WalkDirt");
      sm->SetFaceted(TRUE);
      sm->SetDiffuse(Color(0.0f,0.0f,1.0f), 0);
      sm->SetOpacity(opacity, 0);
      m->SetSubMtlAndName(1, sm, TSTR("Dirt"));
   }
   if (StdMat2* sm = NewDefaultStdMat())
   {
      sm->SetName("WalkGrass");
      sm->SetFaceted(TRUE);
      sm->SetDiffuse(Color(0.0f,1.0f,0.0f), 0);
      sm->SetOpacity(opacity, 0);
      m->SetSubMtlAndName(2, sm, TSTR("Grass"));
   }
   if (StdMat2* sm = NewDefaultStdMat())
   {
      sm->SetName("WalkStone");
      sm->SetFaceted(TRUE);
      sm->SetDiffuse(Color(1.0f,0.0f,0.0f), 0);
      sm->SetOpacity(opacity, 0);
      m->SetSubMtlAndName(3, sm, TSTR("Stone"));
   }
   if (StdMat2* sm = NewDefaultStdMat())
   {
      sm->SetName("WalkWood");
      sm->SetFaceted(TRUE);
      sm->SetDiffuse(Color(1.0f,0.0f,1.0f), 0);
      sm->SetOpacity(opacity, 0);
      m->SetSubMtlAndName(4, sm, TSTR("Wood"));
   }
   if (StdMat2* sm = NewDefaultStdMat())
   {
      sm->SetName("WalkCarpet");
      sm->SetFaceted(TRUE);
      sm->SetDiffuse(Color(1.0f,1.0f,0.0f), 0);
      sm->SetOpacity(opacity, 0);
      m->SetSubMtlAndName(5, sm, TSTR("Carpet"));
   }
   if (StdMat2* sm = NewDefaultStdMat())
   {
      sm->SetName("WalkMetal");
      sm->SetFaceted(TRUE);
      sm->SetDiffuse(Color(0.0f,1.0f,1.0f), 0);
      sm->SetOpacity(opacity, 0);
      m->SetSubMtlAndName(6, sm, TSTR("Metal"));
   }
   if (StdMat2* sm = NewDefaultStdMat())
   {
      sm->SetName("WalkSwamp");
      sm->SetFaceted(TRUE);
      sm->SetDiffuse(Color(0.8f,0.3f,0.0f), 0);
      sm->SetOpacity(opacity, 0);
      m->SetSubMtlAndName(7, sm, TSTR("Swamp"));
   }
   if (StdMat2* sm = NewDefaultStdMat())
   {
      sm->SetName("WalkMud");
      sm->SetFaceted(TRUE);
      sm->SetDiffuse(Color(0.8f,0.0f,0.3f), 0);
      sm->SetOpacity(opacity, 0);
      m->SetSubMtlAndName(8, sm, TSTR("Mud"));
   }
   if (StdMat2* sm = NewDefaultStdMat())
   {
      sm->SetName("WalkLeaves");
      sm->SetFaceted(TRUE);
      sm->SetDiffuse(Color(0.0f,0.8f,0.3f), 0);
      sm->SetOpacity(opacity, 0);
      m->SetSubMtlAndName(9, sm, TSTR("Leaves"));
   }
   if (StdMat2* sm = NewDefaultStdMat())
   {
      sm->SetName("WalkWater");
      sm->SetFaceted(TRUE);
      sm->SetDiffuse(Color(0.0f,0.3f,0.8f), 0);
      sm->SetOpacity(opacity, 0);
      m->SetSubMtlAndName(10, sm, TSTR("Water"));
   }
   if (StdMat2* sm = NewDefaultStdMat())
   {
      sm->SetName("WalkPuddles");
      sm->SetFaceted(TRUE);
      sm->SetDiffuse(Color(0.3f,0.3f,0.8f), 0);
      sm->SetOpacity(opacity, 0);
      m->SetSubMtlAndName(11, sm, TSTR("Puddles"));
   }
   return m;
}

INode *MSHImportImpl::ImportHOOK(HOOK& hook)
{
   ImpNode *node = o->i->CreateNode();
   if(!node) throw runtime_error("Unable to create ImpNode");

   Object *ob = (Object*)o->gi->CreateInstance(HELPER_CLASS_ID,Class_ID(POINTHELP_CLASS_ID,0));
   if(!ob) throw runtime_error("Unable to create Point Helper");

   INode *inode = node->GetINode();
   node->Reference(ob);
   node->SetName(hook.Name.c_str());

#if VERSION_3DSMAX >= ((5000<<16)+(15<<8)+0) // Version 5+
   IParamBlock2* pb = (IParamBlock2*)ob->SubAnim(pointobj_params);//ob->GetParamBlock(pointobj_params);
   pb->SetValue(pointobj_centermarker, 0, TRUE);
   pb->SetValue(pointobj_axistripod, 0, TRUE);
   pb->SetValue(pointobj_cross, 0, FALSE);
   pb->SetValue(pointobj_size, 0, 1.0f);
#endif
   Matrix3 tm(hook.Orientation.m[0], hook.Orientation.m[1], hook.Orientation.m[2], hook.Position);
   node->SetTransform(0, tm);        

   o->i->AddNodeToScene(node);
   inode = node->GetINode();
   inode->EvalWorldState(0);

   return inode;
}

INode *MSHImportImpl::ImportHAIR(HAIR& hair)
{
   ImpNode *node = o->i->CreateNode();
   if(!node) throw runtime_error("Unable to create ImpNode");

   Object *ob = (Object*)o->gi->CreateInstance(HELPER_CLASS_ID,Class_ID(POINTHELP_CLASS_ID,0));
   if(!ob) throw runtime_error("Unable to create Hair Helper");

   INode *inode = node->GetINode();
   node->Reference(ob);
   node->SetName(hair.Name.c_str());

#if VERSION_3DSMAX >= ((5000<<16)+(15<<8)+0) // Version 5+
   IParamBlock2* pb = (IParamBlock2*)ob->SubAnim(pointobj_params);//ob->GetParamBlock(pointobj_params);
   pb->SetValue(pointobj_centermarker, 0, TRUE);
   pb->SetValue(pointobj_axistripod, 0, TRUE);
   pb->SetValue(pointobj_cross, 0, FALSE);
   pb->SetValue(pointobj_size, 0, 1.0f);
#endif

   Matrix3 tm(hair.Orientation.m[0], hair.Orientation.m[1], hair.Orientation.m[2], hair.Position);
   node->SetTransform(0, tm);
   inode->SetUserPropString("HairFlag", EnumToString(hair.eHairFlag, MSHHairShorteningBehaviors));
   inode->SetUserPropInt("flag", hair.eHairFlag);

   o->i->AddNodeToScene(node);
   inode = node->GetINode();
   inode->EvalWorldState(0);

   return inode;
}

INode *MSHImportImpl::ImportHELM(HELM& helm)
{
   ImpNode *node = o->i->CreateNode();
   if(!node) throw runtime_error("Unable to create ImpNode");

   Object *ob = (Object*)o->gi->CreateInstance(HELPER_CLASS_ID,Class_ID(POINTHELP_CLASS_ID,0));
   if(!ob) throw runtime_error("Unable to create Helm Helper");

   INode *inode = node->GetINode();
   node->Reference(ob);
   node->SetName(helm.Name.c_str());

#if VERSION_3DSMAX >= ((5000<<16)+(15<<8)+0) // Version 5+
   IParamBlock2* pb = (IParamBlock2*)ob->SubAnim(pointobj_params);//ob->GetParamBlock(pointobj_params);
   pb->SetValue(pointobj_centermarker, 0, TRUE);
   pb->SetValue(pointobj_axistripod, 0, TRUE);
   pb->SetValue(pointobj_cross, 0, FALSE);
   pb->SetValue(pointobj_size, 0, 1.0f);
#endif

   Matrix3 tm(helm.Orientation.m[0], helm.Orientation.m[1], helm.Orientation.m[2], helm.Position);
   node->SetTransform(0, tm);
   inode->SetUserPropString("HelmFlag", EnumToString(helm.eHELMFlag, MSHHelmHairHidingBehaviors));
   inode->SetUserPropInt("flag", helm.eHELMFlag);

   o->i->AddNodeToScene(node);
   inode = node->GetINode();
   inode->EvalWorldState(0);

   return inode;
}

bool MSHImportImpl::ImportCOLS(COLS& cols, INodeTab& spheres)
{
   if (lastSkeleton.isNull())
      return false;

   // COLS requires a skeleton that is not altered
   stringlist zipFiles = TokenizeString(GetSetting<string>("MMHZip").c_str(), ";");
   ExpandFileList(zipFiles);
   MMHImporter mmhimp(lastSkeleton, o->i, o->gi, o->suppressPrompts);
   for (stringlist::iterator itr = zipFiles.begin(), end = zipFiles.end(); itr != end; ++itr ) 
   {
      INodeTab bones;
      if (mmhimp.ImportFromZip((*itr).c_str(), bones, false))
      {
         for (size_t i=0, n=cols.items.size(); i<n; ++i)
         {
            COLSITM& citm = cols.items[i];
            TSTR name = FormatText("COLS%.2d", i);

            ImpNode *node = o->i->CreateNode();
            if(!node) throw runtime_error("Unable to create ImpNode");

            Object *ob = (Object*)o->gi->CreateInstance(GEOMOBJECT_CLASS_ID,Class_ID(SPHERE_CLASS_ID,0));
            if(!ob) throw runtime_error("Unable to create Collision Sphere");

            INode *inode = node->GetINode();
            node->Reference(ob);
            node->SetName(name);

            inode->SetUserPropInt("MMHIndex", citm.index);
            inode->SetRenderable(FALSE);
            inode->XRayMtl(TRUE);

            IParamArray *params = ob->GetParamBlock();
			float radius = citm.radius;
			if (enableScale) radius *= scaleFactor;

            params->SetValue(ob->GetParamBlockIndex(SPHERE_RADIUS), 0, radius);

            o->i->AddNodeToScene(node);
            inode = node->GetINode();
            inode->EvalWorldState(0);

            INode *bone = bones[citm.index];
            bone->AttachChild(inode, 0);
         }
         return true;
      }
   }
   return false;
}
#endif