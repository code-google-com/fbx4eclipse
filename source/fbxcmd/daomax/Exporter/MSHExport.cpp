/**********************************************************************
*<
FILE: ExporterCore.cpp

DESCRIPTION:	Core Export helper routines

HISTORY: 

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include "DAOExport.h"
#include "MSHExport.h"
#include "MSHFormat.h"
#if VERSION_3DSMAX > ((5000<<16)+(15<<8)+0) // Version 5
#  include "MeshNormalSpec.h"
#endif
#include <limits.h>

extern ClassDesc2* GetDAOExportDesc();

using namespace DAO;
static const Class_ID GNORMAL_CLASS_ID(0x243e22c6, 0x63f6a014);

#if 0
struct VertexGroup
{
   int idx;
   Point3 vert;
   Point3 norm;
   Point3 uv;
};

inline bool equal(const Point3 &a, const Point3 &b, float thresh) {
   return a.Equals(b, thresh);
}
inline bool equal(const Point3 &a, const Point3 &b) {
   return (a == b);
}
inline int compare(float a, float b, float thresh) {
   //float d = b-a;
   //return (fabsf(d) < thresh) ? 0 : ( std::less<float>()(a, b) ? -1 : 1 );
   if (a == b) return 0;
   return std::less<float>()(a, b) ? -1 : 1;
}
inline int compare(const Point3 &a, const Point3 &b, float thresh) {
   int d;
   if ((d = compare(a.x,b.x,thresh)) != 0) return d;
   if ((d = compare(a.y,b.y,thresh)) != 0) return d;
   if ((d = compare(a.z,b.z,thresh)) != 0) return d;
   return 0;
}
inline int compare(const VertexGroup& lhs, const VertexGroup& rhs, float thresh) {
   int d;
   if ((d = compare(lhs.vert,rhs.vert,thresh)) != 0) return d;
   if ((d = compare(lhs.norm,rhs.norm,thresh)) != 0) return d;
   if ((d = compare(lhs.uv,rhs.uv,thresh)) != 0) return d;
   return 0;
}

inline bool TMNegParity(const Matrix3 &m)
{
   return (DotProd(CrossProd(m.GetRow(0),m.GetRow(1)),m.GetRow(2))<0.0)?true:false;
}

struct FaceGroup
{
   NWArray<VertexGroup> verts;
   NWArray<DAO::TRI> faces;
   NWArray<int> vidx;
#ifdef _DEBUG
   void debug();
#endif
};
typedef std::map<int, FaceGroup> FaceGroups;

struct VertexCompare
{
   VertexCompare(FaceGroup& g, float t) : grp(g), thresh(t) {}

   inline bool operator()(const VertexGroup& lhs, const VertexGroup& rhs) const {
      return compare(lhs, rhs) < 0;
   }
   inline bool operator()(const int& lhs, const VertexGroup& rhs) const {
      return compare(grp.verts[lhs], rhs) < 0;
   }
   inline bool operator()(const VertexGroup& lhs, const int& rhs) const {
      return compare(lhs, grp.verts[rhs]) < 0;
   }
   inline bool operator()(const int& lhs, const int& rhs) const {
      return compare(grp.verts[lhs], grp.verts[rhs]) < 0;
   }
   inline int compare(const VertexGroup& lhs, const VertexGroup& rhs) const {
      return ::compare(lhs, rhs, thresh);
   }
   inline int compare(const int& lhs, const VertexGroup& rhs) const {
      return ::compare(grp.verts[lhs], rhs, thresh);
   }
   inline int compare(const VertexGroup& lhs, const int& rhs) const {
      return ::compare(lhs, grp.verts[rhs], thresh);
   }
   inline int compare(const int& lhs, const int& rhs) const {
      return ::compare(grp.verts[lhs], grp.verts[rhs], thresh);
   }

   FaceGroup& grp;
   float thresh;
};

typedef std::pair< std::vector<int>::iterator, std::vector<int>::iterator > IntRange;


struct COLSITMCompare
{
   inline bool operator()(const COLSITM& lhs, const COLSITM& rhs) const {
      return lhs.index < rhs.index;
   }
};

static inline Point3 CalcUVW(Point3 uv, Point2 offset, Point2 size, bool flipuv)
{
   if (flipuv)
   {
      float y = ((1.0f-uv.y) * size.y) + offset.y;
      return Point3((uv.x * size.x) + offset.x, y, uv.z);
   }
   else
   {
      return Point3((uv.x * size.x) + offset.x, (uv.y * size.y) + offset.y, uv.z);

   }
}

typedef NWArray<IMESH*> IMeshList;
struct MSHExportImpl
{
   MSHExportImpl(MSHExporter *owner) 
      : o(owner)
      , mExportHidden(true)
   {

      ParamBlockDesc2 *params = GetDAOExportDesc()->GetParamBlockDescByName("parameters");
      col2Match = params->GetStr(EXP_PB_COL2MATCH);
      col3Match = params->GetStr(EXP_PB_COL3MATCH);
      walkMatch = params->GetStr(EXP_PB_WALKMATCH);
      hookMatch = params->GetStr(EXP_PB_HOOKMATCH);
      hairMatch = params->GetStr(EXP_PB_HAIRMATCH);
      helmMatch = params->GetStr(EXP_PB_HELMMATCH);
      colsMatch = params->GetStr(EXP_PB_COLSMATCH);
      defMaterialUI = params->GetStr(EXP_PB_DEFAULT_MATERIAL_FLAG);
      flipUV = params->GetInt(EXP_PB_FLIPUV) ? true : false;
      mExportHidden = params->GetInt(EXP_PB_EXPORTHIDDEN) ? true : false;

      weldThresh = 1.0e-6f;
      scaleFactor = params->GetFloat(EXP_PB_SCALE_FACTOR) / 100.0f;
      tangentSpaceAlgorithm = params->GetInt(EXP_PB_TANGENT_ALGORITHM);
      enableSimpleExport = params->GetInt(EXP_PB_ENABLE_SIMPLE_EXPORT);
      colSpheres = NULL;

      memset(progressCounters, 0, sizeof(progressCounters));
      memset(progressMax, 0, sizeof(progressMax));
      ++progressMax[Geometry];  // Always add one to list
   }
   ~MSHExportImpl()
   {
      if (colSpheres) { delete colSpheres; colSpheres = NULL; }
   }

   bool BuildObjectList();
   bool BuildObjectList(INode *node);
   bool FillMaterial(StdMat2 *stdmat, DAO::Material& mtl);
   void BuildRIGD(INode *node, Mesh& mesh);
   void BuildSKIN(INode *node, Mesh& mesh);
   void BuildCOL2(INode *node, Mesh& mesh);
   void BuildCOL3(INode *node, Mesh& mesh);
   void BuildWALK(INode *node, Mesh& mesh);
   void BuildHOOK(INode *node);
   void BuildHAIR(INode *node);
   void BuildHELM(INode *node);
   void BuildMESH(SignatureID type, IMeshList& meshes, INode *node, Mesh& mesh);
   void BuildCOLS(INode *node, Object* obj);
   void UpdateTangentSpace(IMESH *mesh);

   // Split Mesh helpers
   Point3 getVertexNormal(Mesh* mesh, int faceNo, RVertex* rv);
   Matrix3 getTextureMatrix(Mtl *mtl);
   int addVertex(FaceGroup &grp, int face, int vi, Mesh& mesh, const Matrix3 &texm);
   BitmapTex *getTexture(Mtl *mtl);
   void SplitMesh(FaceGroups& grps, Mesh& mesh, Matrix3& mtx, Mtl *mtl);
   void BuildMESH(SignatureID type, IMeshList& rv, Mesh& mesh, LPCTSTR name, Matrix3 mtx, Mtl *mtl);
   void BuildSubMESH(IMESH& imesh, FaceGroup& group, Mtl* submtl, const Matrix3& mtx);

   MaterialFlag GetMaterialFlag(INode* node, MaterialFlag uiFlags=mfNone);

   Matrix3 GetObjTMAfterWSM(INode *node, TimeValue t=0)
   {
      Matrix3 mtx = node->GetObjTMAfterWSM(0);
      if (scaleFactor != 1.0f)
         mtx *= ScaleMatrix(Point3(scaleFactor, scaleFactor, scaleFactor));
      return mtx;
   }

   enum ProgressSection
   {
      Geometry,
      Animation,
      Collision,
      Skin,
      ProgressSectionCount
   };
   int progressCounters[ProgressSectionCount];
   int progressMax[ProgressSectionCount];
   bool CalculateProgress(INode *node);
   void ProgressUpdate(ProgressSection section, const TCHAR *s);


   MSHExporter *o;
   MSHHeader hdr;
   bool mExportHidden;
   TSTR col2Match, col3Match, walkMatch, hookMatch, hairMatch, helmMatch, colsMatch;
   TSTR defMaterialUI;
   
   bool flipUV;
   float weldThresh;
   float scaleFactor;
   int tangentSpaceAlgorithm;
   int enableSimpleExport;
   COLS* colSpheres;
};

#endif

MSHExporter::MSHExporter(const TCHAR *Name,ExpInterface *I,Interface *GI, BOOL SuppressPrompts, DWORD options)
   : BaseExporter()
{
   BaseInit(Name,I,GI,SuppressPrompts, options);
}

MSHExporter::MSHExporter()
{
}

void MSHExporter::Initialize()
{
}

bool MSHExporter::DoExport()
{
   //MSHExportImpl impl(this);

   //NWStream out(name.c_str(), false);
   //impl.CalculateProgress( gi->GetRootNode() );
   //impl.BuildObjectList();
   //if (!Store(out, impl.hdr)) {
   //   throw runtime_error(FormatString("Failed to store file: %s", name.c_str()));
   //}
   return true;
}

#if 0
bool MSHExportImpl::BuildObjectList()
{
   bool ok = BuildObjectList( o->gi->GetRootNode() );
   if (colSpheres != NULL)
   {
      std::sort(colSpheres->items.begin(), colSpheres->items.end(), COLSITMCompare());
      hdr.Packets.push_back(colSpheres);
      colSpheres = NULL;
   }
   return ok;
}

// Primary recursive decent routine
bool MSHExportImpl::BuildObjectList(INode *node)
{
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
         ;
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
            ;//
         } 
         else if (os.obj->ClassID() == Class_ID(SPHERE_CLASS_ID, 0) && wildmatch(colsMatch, node->GetName()))
         {
            BuildCOLS(node, os.obj);
         }
         else
         {
            if (TriObject *tri = (TriObject *)os.obj->ConvertToType(t, Class_ID(TRIOBJ_CLASS_ID, 0)))
            {
               TSTR skeleton;
               Mesh& mesh = tri->GetMesh();
               if (wildmatch(col2Match, node->GetName()))
               {
                  BuildCOL2(node, mesh);
               }
               else if (wildmatch(col3Match, node->GetName()))
               {
                  BuildCOL3(node, mesh);
               }
               else if (wildmatch(walkMatch, node->GetName()))
               {
                  BuildWALK(node, mesh);
               }
               else if ( (node->GetUserPropString("Skeleton", skeleton) || node->GetUserPropString("skelfile", skeleton))
			         && NULL != GetSkin(node))
               {
                  BuildSKIN(node, mesh);
               }
               else
               {
                  BuildRIGD(node, mesh);
               }
            }
         }
      }
      else if (os.obj && os.obj->SuperClassID()==HELPER_CLASS_ID)
      {
         if (os.obj->ClassID() == Class_ID(POINTHELP_CLASS_ID,0))
         {
            if (wildmatch(hookMatch, node->GetName())) {
               BuildHOOK(node);
            } else if (wildmatch(hairMatch, node->GetName())) {
               BuildHAIR(node);
            } else if (wildmatch(helmMatch, node->GetName())) {
               BuildHELM(node);
            }
         }
      }
   }
   bool ok = true;
   for (int i=0; i<node->NumberOfChildren() && ok; i++) {
      ok = BuildObjectList(node->GetChildNode(i));
   }
   return true;
}

void MSHExportImpl::BuildMESH(SignatureID type, IMeshList& meshes, INode *node, Mesh& mesh)
{
   BuildMESH(type, meshes, mesh, node->GetName(), GetObjTMAfterWSM(node), node->GetMtl());
}

BitmapTex *MSHExportImpl::getTexture(Mtl *mtl)
{
   if (!mtl) return NULL;

   for (int i=0, texMaps = mtl->NumSubTexmaps(); i<texMaps; i++) {
      Texmap *texMap = mtl->GetSubTexmap(i);
      if (texMap && texMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
         return (BitmapTex*)texMap;
      } else if (texMap && texMap->ClassID() == GNORMAL_CLASS_ID) {
         texMap = texMap->GetSubTexmap(0);
         if (texMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
            return ((BitmapTex*)texMap);
         }
      }
   }
   return NULL;
}

Matrix3 MSHExportImpl::getTextureMatrix(Mtl *mtl)
{
   Matrix3 tm;
   if (BitmapTex *tex = getTexture(mtl))
      tex->GetUVTransform(tm);
   else
      tm.IdentityMatrix();
   return tm;
}

Point3 MSHExportImpl::getVertexNormal(Mesh* mesh, int faceNo, RVertex* rv)
{
   Face* f = &mesh->faces[faceNo];
   DWORD smGroup = f->smGroup;
   int numNormals;
   Point3 vertexNormal;

   // Is normal specified
   // SPCIFIED is not currently used, but may be used in future versions.
   if (rv->rFlags & SPECIFIED_NORMAL) {
      vertexNormal = rv->rn.getNormal();
   } else {
      // If normal is not specified it's only available if the face belongs
      // to a smoothing group
      if ((numNormals = rv->rFlags & NORCT_MASK) && smGroup) {
         // If there is only one vertex is found in the rn member.
         if (numNormals == 1) {
            vertexNormal = rv->rn.getNormal();
         } else {
            // If two or more vertices are there you need to step through them
            // and find the vertex with the same smoothing group as the current face.
            // You will find multiple normals in the ern member.
            for (int i = 0; i < numNormals; i++) {
               if (rv->ern[i].getSmGroup() & smGroup) 
                  vertexNormal = rv->ern[i].getNormal();
            }
         }
      } else {
         // Get the normal from the Face if no smoothing groups are there
         vertexNormal = mesh->getFaceNormal(faceNo);
      }
   }
   return vertexNormal;
}

void MSHExportImpl::SplitMesh(FaceGroups& grps, Mesh& mesh, Matrix3& mtx, Mtl *mtl)
{
   // Order of the vertices. Get 'em counter clockwise if the objects is
   // negatively scaled.
   int vi[3];
   if (TMNegParity(mtx)) {
      vi[0] = 2, vi[1] = 1, vi[2] = 0;
   } else {
      vi[0] = 0, vi[1] = 1, vi[2] = 2;
   }
   Matrix3 flip;
   flip.IdentityMatrix();
   flip.Scale(Point3(1, 1, 1));

#if VERSION_3DSMAX <= ((5000<<16)+(15<<8)+0) // Version 5
   mesh.checkNormals(TRUE);
#else
   MeshNormalSpec *specNorms = mesh.GetSpecifiedNormals ();
   if (NULL != specNorms) {
      specNorms->CheckNormals();
      if (specNorms->GetNumNormals() == 0)
         mesh.checkNormals(TRUE);
   } else {
      mesh.checkNormals(TRUE);
   }
#endif

   int nv = mesh.getNumVerts();
   int nf = mesh.getNumFaces();

   if (enableSimpleExport == 1)
   {
      // Dont split the mesh at all.  For debugging purposes.
      FaceGroup& grp = grps[0];
      grp.vidx.resize(nv);
      grp.verts.resize(nv);
      grp.faces.resize(nf);
      for (int i=0; i<nv; ++i)
         grp.verts[i].idx = -1;

      for (int face=0; face<nf; ++face) {
         for (int vi=0; vi<3; ++vi) {
            int idx = mesh.faces[face].getVert(vi);
            grp.faces[face][vi] = idx;

            // Calculate normal
            Point3 norm;
#if VERSION_3DSMAX <= ((5000<<16)+(15<<8)+0) // Version 5
            norm = getVertexNormal(&mesh, face, mesh.getRVertPtr(idx));
#else
            MeshNormalSpec *specNorms = mesh.GetSpecifiedNormals ();
            if (NULL != specNorms && specNorms->GetNumNormals() != 0)
               norm = specNorms->GetNormal(face, vi);
            else
               norm = getVertexNormal(&mesh, face, mesh.getRVertPtr(idx));
#endif
            VertexGroup& vg = grp.verts[idx];
            if (vg.idx == idx){
               ASSERT(vg.vert == mesh.getVert(idx));
               //ASSERT(vg.norm == norm);
               Point3 uv = mesh.getTVert(idx);
               if (mesh.getNumTVerts() > 0)
               {
                  uv.y = -uv.y;
                  ASSERT(vg.uv == uv);
               }
            } else {
               vg.idx = idx;
               vg.vert = mesh.getVert(idx);
               vg.uv = (mesh.getNumTVerts() > 0) ? mesh.getTVert(idx) : Point3(0.0f,0.0f,0.0f);
               vg.uv.y = -vg.uv.y;
               vg.norm = norm;
            }
         }
      }
      for (int i=0; i<nv; ++i) {
         ASSERT(grp.verts[i].idx != -1);
      }
   }
   else
   {
      int numSubMtls = mtl ? mtl->NumSubMtls() : 0;
      for (int i=0, n = mesh.getNumFaces(); i<n; i++) 
      {
         int mtlID = (numSubMtls != 0) ? (mesh.faces[i].getMatID() % numSubMtls) : 0;
         Mtl *submtl = (mtl && mtl->NumSubMtls() > 0) ? mtl->GetSubMtl(mtlID % mtl->NumSubMtls()) : mtl;
         Matrix3 texm = getTextureMatrix(submtl) * flip;
         FaceGroup& grp = grps[mtlID];
         if (nv > int(grp.verts.capacity()))
            grp.verts.reserve(nv);
         if (nf > int(grp.faces.capacity()))
            grp.faces.reserve(nf);

         DAO::TRI tri;
         for (int j=0; j<3; ++j) {
            tri[j] = WORD(addVertex(grp, i, vi[j], mesh, texm));
         }
         grp.faces.push_back(tri);
      }
   }
}

int MSHExportImpl::addVertex(FaceGroup &grp, int face, int vi, Mesh& mesh, const Matrix3 &texm)
{
   VertexGroup vg;
   vg.idx = mesh.faces[ face ].v[ vi ];
   vg.vert = mesh.verts[ vg.idx ];
   bool hasUVW = (mesh.tVerts && mesh.tvFace);

#if VERSION_3DSMAX <= ((5000<<16)+(15<<8)+0) // Version 5
   vg.norm = getVertexNormal(&mesh, face, mesh.getRVertPtr(vg.idx));
#else
   MeshNormalSpec *specNorms = mesh.GetSpecifiedNormals ();
   if (NULL != specNorms && specNorms->GetNumNormals() != 0)
      vg.norm = specNorms->GetNormal(face, vi);
   else
      vg.norm = getVertexNormal(&mesh, face, mesh.getRVertPtr(vg.idx));
#endif

   // Flip the UV coordinates
   if (hasUVW) {
      vg.uv = mesh.tVerts[ mesh.tvFace[ face ].t[ vi ]] * texm;
   }

   //Binary search is O(ln(n)) rather than O(n^2) which would happen if we did a brute force search
   VertexCompare vc(grp, weldThresh);
   int n = grp.verts.size();
   IntRange range = std::equal_range(grp.vidx.begin(), grp.vidx.end(), vg, vc);
   if (range.first != range.second)
   {
#ifdef _DEBUG
      int c = vc.compare(*range.first, vg);
      if (c == 0)
      {
         return (*range.first);
      }
      else if (c < 0)
      {
         ASSERT(!"Invalid sorting order");
         grp.vidx.insert(range.first, n);
         grp.verts.push_back(vg);
         return n;
      }
      else //if (c > 0)
      {
         ASSERT(!"Invalid sorting order");
         grp.vidx.insert(range.second, n);
         grp.verts.push_back(vg);
         return n;
      }
#else
      return (*range.first);
#endif
   }
   else
   {
      grp.vidx.insert(range.first, n);
      grp.verts.push_back(vg);
      return n;
   }
}

void MSHExportImpl::BuildMESH(SignatureID type, IMeshList& rv, Mesh& mesh, LPCTSTR name, Matrix3 mtx, Mtl *mtl)
{
   FaceGroups grps;
   SplitMesh(grps, mesh, mtx, mtl);

   TSTR format = (grps.size() > 1) ? "%s%c" : "%s";
   char c = 'A';
   for (FaceGroups::iterator itr=grps.begin(); itr!=grps.end(); ++itr, ++c)
   {
      MtlID mtlid = itr->first;
      FaceGroup& grp = itr->second;
      Mtl *submtl = (mtl && mtl->NumSubMtls() > 0) ? mtl->GetSubMtl(mtlid % mtl->NumSubMtls()) : mtl;
      IMESH* imesh = CreateMESH(type);
      imesh->getName() = FormatText(format, name, c);
      BuildSubMESH(*imesh, grp, submtl, mtx);
      rv.push_back(imesh);
   }
}

void MSHExportImpl::BuildSubMESH(IMESH& imesh, FaceGroup& group, Mtl* submtl, const Matrix3& mtx)
{
   int n = group.verts.size();
   if (n > USHRT_MAX) {
      throw runtime_error(FormatString("Mesh '%s' has more than %d vertices and cannot be exported.", imesh.getName().c_str(), USHRT_MAX));
   }
   int cropPlace = 0;
   Point2 uvoff(0.0f, 0.0f), uvsize(1.0f, 1.0f);

   if(submtl && submtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0) ) {
      StdMat2 *m = (StdMat2*)submtl;
      BitmapTex* bmTex = NULL;
      if (m->GetMapState(ID_DI) == 2) {
         if (Texmap *texMap = m->GetSubTexmap(ID_DI)) {
            if (texMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
               bmTex = ((BitmapTex*)texMap);
            }
         }
      } else if (m->GetMapState(ID_AM) == 2) {
         if (Texmap *texMap = m->GetSubTexmap(ID_AM)) {
            if (texMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
               bmTex = ((BitmapTex*)texMap);
            }
         }
      }
#if VERSION_3DSMAX >= ((5000<<16)+(15<<8)+0) // Version 5
      if (NULL != bmTex)
      {
         getMAXScriptValue(bmTex, "cropPlace", 0, cropPlace);
         if (0 == cropPlace)
         {
            getMAXScriptValue(bmTex, "clipu", 0, uvoff.x);
            getMAXScriptValue(bmTex, "clipv", 0, uvoff.y);
            getMAXScriptValue(bmTex, "clipw", 0, uvsize.x);
            getMAXScriptValue(bmTex, "cliph", 0, uvsize.y);
         }
      }
#endif
   }

   // Apply collapse world transforms into mesh on export
   Matrix3 rmtx = mtx;
   rmtx.NoScale();
   rmtx.NoTrans();

   imesh.setNVerts(n);
   for ( int i = 0; i < n; ++i ) {
      IVERT& vert = imesh.getVert(i);
      VertexGroup& vg = group.verts[i];
      vert.getPosition() = vg.vert * mtx;
      vert.getNormal() = (vg.norm * rmtx).Normalize();
      vert.getUVW() = CalcUVW(vg.uv, uvoff, uvsize, flipUV);
   }
   imesh.getFaces() = group.faces;
   if(submtl && submtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0) ) {
      FillMaterial((StdMat2*)submtl, imesh.getMaterial());
   }
}

void MSHExportImpl::UpdateTangentSpace(IMESH *smesh)
{

   // Handle Tangents and Binormals
   int nt = smesh->getFaces().size();
   if (tangentSpaceAlgorithm == 0) // Obsidian Algorithm
   {
      for ( int faceNo = 0; faceNo < nt; ++faceNo )   // for each face
      {
         DAO::TRI& t = smesh->getFaces()[faceNo];  // get face
         int i0 = t[0], i1 = t[1], i2 = t[2];		// get vertex numbers
         Point3 side_0 = smesh->getVert(i0).getPosition() - smesh->getVert(i1).getPosition();
         Point3 side_1 = smesh->getVert(i2).getPosition() - smesh->getVert(i1).getPosition();

         float delta_U_0 = smesh->getVert(i0).getUVW().x - smesh->getVert(i1).getUVW().x;
         float delta_U_1 = smesh->getVert(i2).getUVW().x - smesh->getVert(i1).getUVW().x;
         float delta_V_0 = smesh->getVert(i0).getUVW().y - smesh->getVert(i1).getUVW().y;
         float delta_V_1 = smesh->getVert(i2).getUVW().y - smesh->getVert(i1).getUVW().y;

         Point3 face_tangent = Normalize( delta_V_1 * side_0 - delta_V_0 * side_1 );
         Point3 face_bi_tangent = Normalize( delta_U_1 * side_0 - delta_U_0 * side_1 );
         Point3 face_normal = Normalize( side_0 ^ side_1 );
         float face_winding = 1.0f;
         if( ( (face_tangent ^ face_bi_tangent) % face_normal ) < 0 ) {
            face_bi_tangent = -face_bi_tangent;
            face_winding = 1.0f;
         }
         else {
            face_winding = -1.0f;
         }
         smesh->getVert(i0).getUVW().z = smesh->getVert(i1).getUVW().z = smesh->getVert(i2).getUVW().z = face_winding;

         // no duplication, just smoothing
         for ( int j = 0; j <= 2; j++ ) {	
            int i = t[j];
            smesh->getVert(i).getTangent() += face_tangent;
            smesh->getVert(i).getBinormal() += face_bi_tangent;
         }
      }

      // for each.getPosition(), normalize the Tangent and Binormal
      for ( int i = 0; i < smesh->getNVerts(); i++ ) {	
         DAO::IVERT& v = smesh->getVert(i);
         v.getBinormal() = Normalize(v.getBinormal());
         v.getTangent() = Normalize(v.getTangent());
      }
   }
   else if (tangentSpaceAlgorithm == 1) // m4444x Algorithm
   {
      // Handle Tangents and Binormals
      for ( int faceNo = 0; faceNo < nt; ++faceNo ) 
      {
         DAO::TRI& t = smesh->getFaces()[faceNo];
         int i1 = t[0], i2 = t[1], i3 = t[2];
         Point3 v1 = smesh->getVert(i1).getPosition();
         Point3 v2 = smesh->getVert(i2).getPosition();
         Point3 v3 = smesh->getVert(i3).getPosition();
         Point3 v2v1 = v2 - v1;
         Point3 v3v1 = v3 - v1;

         Point3 w1 = smesh->getVert(i1).getUVW();
         Point3 w2 = smesh->getVert(i2).getUVW();
         Point3 w3 = smesh->getVert(i3).getUVW();
         Point3 w2w1 = w2 - w1;
         Point3 w3w1 = w3 - w1;

         float r = w2w1.x * w3w1.y - w3w1.x * w2w1.y;
         if ( abs( r ) <= 10e-5 ){
            continue;
         }
         r = 1.0f / r;
         Point3 sdir ( Normalize( Point3( 
            ( w3w1.y * v2v1.x - w2w1.y * v3v1.x ) * r,
            ( w3w1.y * v2v1.y - w2w1.y * v3v1.y ) * r,
            ( w3w1.y * v2v1.z - w2w1.y * v3v1.z ) * r
            ) ) );
         Point3 tdir( Normalize( Point3(
            ( w2w1.x * v3v1.x - w3w1.x * v2v1.x ) * r,
            ( w2w1.x * v3v1.y - w3w1.x * v2v1.y ) * r,
            ( w2w1.x * v3v1.z - w3w1.x * v2v1.z ) * r
            ) ) );
         // no duplication, just smoothing
         for ( int j = 0; j <= 2; j++ ) {	
            smesh->getVert(t[j]).getTangent() += sdir;
            smesh->getVert(t[j]).getBinormal() += tdir;
         }
      }

      // for each.getPosition() calculate tangent and binormal
      for ( int i = 0; i < smesh->getNVerts(); i++ ) {	
         DAO::IVERT& v = smesh->getVert(i);
         const Point3 & n = v.getNormal();
         const Point3 zero(0.0f, 0.0f, 0.0f);
         Point3 & t = v.getTangent();
         Point3 & b = v.getBinormal();
         Point3 & uv = v.getUVW();
         if ( t == zero || b == zero ) {
            t.x = n.y, t.y = n.z, t.z = n.x;
            b = Normalize( n ^ t ); // Cross
         } else {
            t = Normalize( t - n * (n % t) );
            b = Normalize( b - n * (n % b) );
         }
         float face_winding = ( (t ^ b) % n );
         if( face_winding < 0.0f ) {
            uv.z = 1.0f;
         }
         else {
            t = -t;
            uv.z = -1.0f;
         }
      }
   }
   else if (tangentSpaceAlgorithm == 2)  // Zylch000
   {
      // Handle Tangents and Binormals
      int nt = smesh->getFaces().size();
      for ( int faceNo = 0; faceNo < nt; ++faceNo )   // for each face
      {
         DAO::TRI& t = smesh->getFaces()[faceNo];  // get face
         int i0 = t[0], i1 = t[1], i2 = t[2];		// get vertex numbers
         Point3 v0 = smesh->getVert(i0).getPosition();
         Point3 v1 = smesh->getVert(i1).getPosition();
         Point3 v2 = smesh->getVert(i2).getPosition();
         Point3 side_0 = v0 - v1;
         Point3 side_1 = v2 - v1;

         float delta_U_0 = smesh->getVert(i0).getUVW().x - smesh->getVert(i1).getUVW().x;
         float delta_U_1 = smesh->getVert(i2).getUVW().x - smesh->getVert(i1).getUVW().x;

         float delta_V_0 = smesh->getVert(i0).getUVW().y - smesh->getVert(i1).getUVW().y;
         float delta_V_1 = smesh->getVert(i2).getUVW().y - smesh->getVert(i1).getUVW().y;

         Point3 face_tangent = Normalize( delta_V_1 * side_0 - delta_V_0 * side_1 );
         Point3 face_binormal = Normalize( delta_U_1 * side_0 - delta_U_0 * side_1 );

         // no duplication, just smoothing
         for ( int j = 0; j <= 2; j++ ) {	
            int i = t[j];
            // TODO this can definitely be optimized...
            // find verts that share same location (includes vert in question)
            for( int vn = 0; vn < smesh->getNVerts(); vn++ ) {
               if( smesh->getVert(vn).getPosition() == smesh->getVert(i).getPosition() ) {
                  smesh->getVert(vn).getTangent() += face_tangent;
                  smesh->getVert(vn).getBinormal() += face_binormal;
               }
            }
         }
      }

      // for each.getPosition(), normalize the Tangent and Binormal
      for ( int i = 0; i < smesh->getNVerts(); i++ ) {	
         DAO::IVERT& v = smesh->getVert(i);
         v.getBinormal() = Normalize(v.getBinormal());
         v.getTangent() = Normalize(v.getTangent());
         float face_winding = (v.getTangent() ^ v.getBinormal()) % v.getNormal();
         if( face_winding > 0 ) {
            v.getBinormal() = -v.getBinormal();
            //v.getUVW().z = 1.0f;
         }
         else {
            //v.getUVW().z = -1.0f;
         }
      }
   }
}

MaterialFlag MSHExportImpl::GetMaterialFlag(INode* node, MaterialFlag uiFlags)
{
   if (node->UserPropExists("UIFlag"))
   {
      TSTR uiFlagStr;
      node->GetUserPropString("UIFlag", uiFlagStr);
      uiFlags = MaterialFlag(StringToFlags(uiFlagStr, MaterialFlags));
   }
   if (node->UserPropExists("trans"))
   {
      int ival = 0;
      if (node->GetUserPropInt("trans", ival) && ival)
         uiFlags = MaterialFlag(uiFlags | DAO_ALPHA_TEST);
      else
         uiFlags = MaterialFlag(uiFlags & ~DAO_ALPHA_TEST);
   }
   if (node->UserPropExists("alphablend"))
   {
      int ival = 0;
      if (node->GetUserPropInt("alphablend", ival) && ival)
         uiFlags = MaterialFlag(uiFlags | DAO_ALPHA_BLEND);
      else
         uiFlags = MaterialFlag(uiFlags & ~DAO_ALPHA_BLEND);
   }
   if (node->UserPropExists("addblend"))
   {
      int ival = 0;
      if (node->GetUserPropInt("addblend", ival) && ival)
         uiFlags = MaterialFlag(uiFlags | DAO_ADDITIVE_BLEND);
      else
         uiFlags = MaterialFlag(uiFlags & ~DAO_ADDITIVE_BLEND);
   }

   if (node->UserPropExists("envmap"))
   {
      int ival = 0;
      if (node->GetUserPropInt("envmap", ival) && ival)
         uiFlags = MaterialFlag(uiFlags | DAO_ENVIRONMENT_MAPPED);
      else
         uiFlags = MaterialFlag(uiFlags & ~DAO_ENVIRONMENT_MAPPED);
   }

   if (node->UserPropExists("head"))
   {
      int ival = 0;
      if (node->GetUserPropInt("head", ival) && ival)
         uiFlags = MaterialFlag(uiFlags | DAO_CUTSCENE_MESH);
      else
         uiFlags = MaterialFlag(uiFlags & ~DAO_CUTSCENE_MESH);
   }

   if (node->UserPropExists("nolight"))
   {
      int ival = 0;
      if (node->GetUserPropInt("nolight", ival) && ival)
         uiFlags = MaterialFlag(uiFlags | DAO_GLOW);
      else
         uiFlags = MaterialFlag(uiFlags & ~DAO_GLOW);
   }

   if (node->UserPropExists("nocast"))
   {
      int ival = 0;
      if (node->GetUserPropInt("nocast", ival) && ival)
         uiFlags = MaterialFlag(uiFlags | DAO_NO_CAST_SHADOWS);
      else
         uiFlags = MaterialFlag(uiFlags & ~DAO_NO_CAST_SHADOWS);
   }
   if (node->UserPropExists("projtexture"))
   {
      int ival = 0;
      if (node->GetUserPropInt("projtexture", ival) && ival)
         uiFlags = MaterialFlag(uiFlags | DAO_PROJECTED_TEXTURES);
      else
         uiFlags = MaterialFlag(uiFlags & ~DAO_PROJECTED_TEXTURES);
   }
   return uiFlags;
}

void MSHExportImpl::BuildRIGD(INode *node, Mesh& mesh)
{
   SignatureID sig("RIGD");
   ProgressUpdate(Geometry, FormatText("Exporting '%s' as %s", node->GetName(), sig.c_str()));

   MaterialFlag uiFlags = GetMaterialFlag(node, MaterialFlag(StringToFlags(defMaterialUI, MaterialFlags)));

   IMeshList meshes;
   BuildMESH(sig, meshes, node, mesh);
   for ( int i=0; i<meshes.size(); i++)
   {
      RIGD* smesh = (RIGD*)meshes[i];
      smesh->mtl.uiFlags = uiFlags;
      UpdateTangentSpace(smesh);

      hdr.Packets.push_back(smesh);
   }
}

void MSHExportImpl::BuildSKIN(INode *node, Mesh& mesh)
{
   SignatureID sig("SKIN");
   ProgressUpdate(Geometry, FormatText("Exporting '%s' as %s", node->GetName(), sig.c_str()));

   //get the skin modifier
   Modifier *mod = GetSkin(node);
   ISkin *skin = (mod ? (ISkin *) mod->GetInterface(I_SKIN) : NULL);
   ISkinContextData *skinData = (skin ? skin->GetContextInterface(node) : NULL);

   MaterialFlag uiFlags = GetMaterialFlag(node, MaterialFlag(StringToFlags(defMaterialUI, MaterialFlags)));

   LPCTSTR name = node->GetName();
   Matrix3 mtx = GetObjTMAfterWSM(node);
   Mtl *mtl = node->GetMtl();
   FaceGroups grps;
   SplitMesh(grps, mesh, mtx, mtl);

   TSTR format = (grps.size() > 1) ? "%s%c" : "%s";
   char c = 'A';
   for (FaceGroups::iterator itr=grps.begin(); itr!=grps.end(); ++itr, ++c)
   {
      MtlID mtlid = itr->first;
      FaceGroup& grp = itr->second;
      Mtl *submtl = (mtl && mtl->NumSubMtls() > 0) ? mtl->GetSubMtl(mtlid % mtl->NumSubMtls()) : mtl;
      SKIN* smesh = (SKIN*)CreateMESH(sig);

      smesh->getName() = FormatText(format, name, c);

      TSTR skeleton;
      if (node->GetUserPropString("skelfile", skeleton))
         smesh->Skeleton = skeleton;
      else if (node->GetUserPropString("Skeleton", skeleton))
         smesh->Skeleton = skeleton;

      BuildSubMESH(*smesh, grp, submtl, mtx);
      hdr.Packets.push_back(smesh);

      // override uiflags
      smesh->mtl.uiFlags = uiFlags;
      UpdateTangentSpace(smesh);

      // Handle SKIN data
      if (skinData)
      {
         for ( int i = 0, n = smesh->getNVerts(); i < n; ++i ) 
         {
            VertexGroup& vg = grp.verts[i];
            SVERT& sv = smesh->verts[i];

            sv.BoneCount = 4.0f;
            int vi = vg.idx;
            int npoints = skinData->GetNumPoints();
            if (vi < npoints)
            {
               int nbones = skinData->GetNumAssignedBones(vi);
               if (nbones <= 4)
               {
                  for (int j=0; j<nbones; ++j)
                  {
                     int boneIndex = skinData->GetAssignedBone(vi,j);
                     INode *bone = skin->GetBone(boneIndex);
                     if (!bone->GetUserPropInt("MSHBoneIndex", boneIndex))
                        boneIndex = 0;
                     sv.BoneIndex[j] = boneIndex;
                     sv.BoneWeights[j] = skinData->GetBoneWeight(vi,j);
                  }
               }
               else
               {
                  // Deal with case where too many bones are weighted.  Take the strongest weights and discard the rest.
                  int boneIndices[4];
                  float boneWeights[4];
                  for (int j=0; j<4; ++j)
                     boneIndices[j] = 0, boneWeights[j] = 0.0f;
                  for (int j=0; j<nbones; ++j)
                  {
                     int boneIndex = skinData->GetAssignedBone(vi,j);
                     INode *bone = skin->GetBone(boneIndex);
                     if (!bone->GetUserPropInt("MSHBoneIndex", boneIndex))
                        boneIndex = 0;
                     float boneWeight = skinData->GetBoneWeight(vi,j);
                     for (int k=0; k<4; ++k)
                     {
                        if (boneWeights[k] < boneWeight)
                        {
                           memmove(boneIndices+k+1, boneIndices+k, sizeof(int)*(4-k-1));
                           memmove(boneWeights+k+1, boneWeights+k, sizeof(float)*(4-k-1));
                           boneIndices[k] = boneIndex;
                           boneWeights[k] = boneWeight;
                           break;
                        }
                     }
                  }
                  float sum = 0.0f;
                  for (int j=0; j<4; ++j)
                     sum += boneWeights[j];
                  for (int j=0; j<4; ++j) {
                     sv.BoneIndex[j] = boneIndices[j];
                     sv.BoneWeights[j] = boneWeights[j]/sum;
                  }
               }
               for (int j=nbones; j<4; ++j) {
                  sv.BoneIndex[j] = 0;
                  sv.BoneWeights[j] = 0.0f;
               }
            }
            else
            {
               vi = vi;
            }
         }
      }
   }
}

void MSHExportImpl::BuildCOL2(INode *node, Mesh& mesh)
{
   SignatureID sig("COL2");
   ProgressUpdate(Geometry, FormatText("Exporting '%s' as %s", node->GetName(), sig.c_str()));

   MaterialFlag uiFlags = GetMaterialFlag(node, MaterialFlag(StringToFlags(defMaterialUI, MaterialFlags)));

   IMeshList meshes;
   BuildMESH(sig, meshes, node, mesh);
   for ( int i=0; i<meshes.size(); i++) {
      COL2* c2 = (COL2*)meshes[i];
      c2->mtl.uiFlags = uiFlags;
      hdr.Packets.push_back(c2);
   }
}

void MSHExportImpl::BuildCOL3(INode *node, Mesh& mesh)
{
   SignatureID sig("COL3");
   //o->gi->ReplacePrompt(FormatText("Exporting '%s' as %s", node->GetName(), sig.c_str()));
   ProgressUpdate(Geometry, FormatText("Exporting '%s' as %s", node->GetName(), sig.c_str()));

   MaterialFlag uiFlags = GetMaterialFlag(node, MaterialFlag(StringToFlags(defMaterialUI, MaterialFlags)));

   IMeshList meshes;
   BuildMESH(sig, meshes, node, mesh);
   for ( int i=0; i<meshes.size(); i++) {
      COL3* c3 = (COL3*)meshes[i];
      c3->mtl.uiFlags = uiFlags;
      hdr.Packets.push_back((COL3*)meshes[i]);
   }
}

void MSHExportImpl::BuildWALK(INode *node, Mesh& mesh)
{
   static int mtlMap[] = { 0, 0x9, 0x11, 0x21, 0x41, 0x81, 0x101, 0x201, 0x401, 0x801, 0x1001, 0x2001 };

   SignatureID sig("WALK");
   //o->gi->ReplacePrompt(FormatText("Exporting '%s' as %s", node->GetName(), sig.c_str()));
   ProgressUpdate(Geometry, FormatText("Exporting '%s' as %s", node->GetName(), sig.c_str()));

   WALK* walk = new WALK();
   walk->Name = node->GetName();
   
   MaterialFlag uiFlags = GetMaterialFlag(node, mfNone);
   walk->uiFlags = uiFlags;

   TimeValue t = 0;
   Matrix3 mtx = GetObjTMAfterWSM(node);

   ParamBlockDesc2 *params = GetDAOExportDesc()->GetParamBlockDescByName("parameters");
   float limit = params->GetInt(EXP_PB_TRIM_WALKMESH) ? params->GetFloat(EXP_PB_WALKMESH_LIMIT) : -1000000.0f;

   walk->Name = node->GetName();
   int n = mesh.getNumVerts();
   walk->verts.resize(n);
   for ( int i = 0; i < n; ++i ) {
      walk->verts[i] = (mtx * mesh.getVert(i));
      if (walk->verts[i].z <= limit)
         walk->verts[i].z = -1000000.0f;
   }
   int nt = mesh.getNumFaces();
   NWArray<DAO::WTRI>& faces = walk->faces;
   faces.resize(nt);

   Mtl* mtl = node->GetMtl();
   MultiMtl* mmtl = (mtl && mtl->IsMultiMtl()) ? (MultiMtl*)mtl : NULL;

   for ( int i = 0; i < nt; ++i ) {
      faces[i].a = WORD(mesh.faces[i].v[0]);
      faces[i].b = WORD(mesh.faces[i].v[1]);
      faces[i].c = WORD(mesh.faces[i].v[2]);

      if (NULL == mmtl)
      {
         faces[i].flags = 0;
      }
      else
      {
         MtlID mid = mesh.getFaceMtlIndex(i);
         TSTR name;
         mmtl->GetSubMtlName(mid, name);
         if (wildmatch("0x*", name)) // Backward compatibility check
         {
            if (mid == 1) mid = 0; // Special case since you cannot have a mid of 0
            faces[i].flags = mid;
         }
         else
         {
            if (mid < 0 || mid > _countof(mtlMap))
               mid = 0;
            faces[i].flags = mtlMap[mid];
         }
      }
   }

   hdr.Packets.push_back(walk);
}


bool MSHExportImpl::FillMaterial(StdMat2 *m, DAO::Material& mtl)
{
   mtl.Kd = m->GetDiffuse(0);
   mtl.Ks = m->GetSpecular(0);
   mtl.uiFlags = MaterialFlag(StringToFlags(defMaterialUI, MaterialFlags));
   mtl.specularValue = m->GetShinStr(0);
   mtl.specularPower = m->GetShininess(0) * 100.0f;

   TSTR diffuseStr;
   TSTR normalStr;
   TSTR specularStr;
   TSTR glowStr;

   if (m->GetMapState(ID_DI) == 2) {
      diffuseStr = m->GetMapName(ID_DI);
      if (Texmap *texMap = m->GetSubTexmap(ID_DI)) {
         if (texMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
            diffuseStr = ((BitmapTex*)texMap)->GetMapName();
         }
      }
   } else if (m->GetMapState(ID_AM) == 2) {
      diffuseStr = m->GetMapName(ID_AM);
      if (Texmap *texMap = m->GetSubTexmap(ID_AM)) {
         if (texMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
            diffuseStr = ((BitmapTex*)texMap)->GetMapName();
         }
      }
   }
   if (m->GetMapState(ID_BU) == 2) {
      normalStr = m->GetMapName(ID_BU);     
      if (Texmap *texMap = m->GetSubTexmap(ID_BU)) {
         if (texMap->ClassID() == GNORMAL_CLASS_ID) {
            texMap = texMap->GetSubTexmap(0);
            if (texMap && texMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
               normalStr = ((BitmapTex*)texMap)->GetMapName();
            }
         } else if (texMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
            normalStr = ((BitmapTex*)texMap)->GetMapName();
         }
      }
   }
   if (m->GetMapState(ID_SP) == 2) {
      specularStr = m->GetMapName(ID_SP);     
      if (Texmap *texMap = m->GetSubTexmap(ID_SP)) {
         if (texMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
            specularStr = ((BitmapTex*)texMap)->GetMapName();
         }
      }
   }
   if (m->GetMapState(ID_FI) == 2) {
	   specularStr = m->GetMapName(ID_FI);     
	   if (Texmap *texMap = m->GetSubTexmap(ID_FI)) {
		   if (texMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
			   specularStr = ((BitmapTex*)texMap)->GetMapName();
		   }
	   }
   }
   if (m->GetMapState(ID_SI) == 2) {
      glowStr = m->GetMapName(ID_SI);     
      if (Texmap *texMap = m->GetSubTexmap(ID_SI)) {
         if (texMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
            glowStr = ((BitmapTex*)texMap)->GetMapName();
         }
      }
   }
   if (_access(diffuseStr, 0) != -1)
   {
      TSTR dir = diffuseStr;
      TSTR fname = PathFindFileName(dir);
      TSTR ext = PathFindExtension(fname);
      PathRemoveExtension(fname);
      PathRemoveFileSpec(dir);

      TSTR full; full.Resize(MAX_PATH);
      PathCombine(full, dir, FormatText("%s_n%s", fname.data(), ext.data()));
      if (normalStr.isNull() && _access(full, 0) != -1) {
         normalStr = full;
      }
      PathCombine(full, dir, FormatText("%s_t%s", fname.data(), ext.data()));
      if (specularStr.isNull() && _access(full, 0) != -1) {
         specularStr = full;
      }
      PathCombine(full, dir, FormatText("%s_g%s", fname.data(), ext.data()));
      if (glowStr.isNull() && _access(full, 0) != -1) {
         glowStr = full;
      } else {
         PathCombine(full, dir, FormatText("%s_i%s", fname.data(), ext.data()));
         if (glowStr.isNull() && _access(full, 0) != -1) {
            glowStr = full;
         }
      }
   }
   if (!diffuseStr.isNull())
   {
      TSTR name = PathFindFileName(diffuseStr);
      PathRemoveExtension(name);
      mtl.DiffuseMap = name;
   }
   if (!normalStr.isNull())
   {
      TSTR name = PathFindFileName(normalStr);
      PathRemoveExtension(name);
      mtl.NormalMap = name;
   }
   if (!specularStr.isNull())
   {
      TSTR name = PathFindFileName(specularStr);
      PathRemoveExtension(name);
      mtl.TintMap = name;
   }
   if (!glowStr.isNull())
   {
      TSTR name = PathFindFileName(glowStr);
      PathRemoveExtension(name);
      mtl.GlowMap = name;
   }
   return true;
}

void MSHExportImpl::BuildHOOK(INode *node)
{
   SignatureID sig("HOOK");
   //o->gi->ReplacePrompt(FormatText("Exporting '%s' as %s", node->GetName(), sig.c_str()));
   ProgressUpdate(Geometry, FormatText("Exporting '%s' as %s", node->GetName(), sig.c_str()));

   HOOK* hook = new HOOK();
   hook->Name = node->GetName();

   Matrix3 tm = GetObjTMAfterWSM(node);
   hook->Position = tm.GetTrans();
   hook->Orientation.m[0] = tm.GetRow(0);
   hook->Orientation.m[1] = tm.GetRow(1);
   hook->Orientation.m[2] = tm.GetRow(2);
   hdr.Packets.push_back(hook);
}

void MSHExportImpl::BuildHAIR(INode *node)
{
   SignatureID sig("HAIR");
//   o->gi->ReplacePrompt(FormatText("Exporting '%s' as %s", node->GetName(), sig.c_str()));
   ProgressUpdate(Geometry, FormatText("Exporting '%s' as %s", node->GetName(), sig.c_str()));

   HAIR* hair = new HAIR();
   hair->Name = node->GetName();

   Matrix3 tm = GetObjTMAfterWSM(node);
   hair->Position = tm.GetTrans();
   hair->Orientation.m[0] = tm.GetRow(0);
   hair->Orientation.m[1] = tm.GetRow(1);
   hair->Orientation.m[2] = tm.GetRow(2);

   TSTR str; node->GetUserPropString("HairFlag", str);
   hair->eHairFlag = MSHHairShorteningBehavior(StringToEnum(str, MSHHairShorteningBehaviors));

   hdr.Packets.push_back(hair);
}

void MSHExportImpl::BuildHELM(INode *node)
{
   SignatureID sig("HELM");
   //o->gi->ReplacePrompt(FormatText("Exporting '%s' as %s", node->GetName(), sig.c_str()));
   ProgressUpdate(Geometry, FormatText("Exporting '%s' as %s", node->GetName(), sig.c_str()));

   HELM* helm = new HELM();
   helm->Name = node->GetName();

   Matrix3 tm = GetObjTMAfterWSM(node);
   helm->Position = tm.GetTrans();
   helm->Orientation.m[0] = tm.GetRow(0);
   helm->Orientation.m[1] = tm.GetRow(1);
   helm->Orientation.m[2] = tm.GetRow(2);

   TSTR str; node->GetUserPropString("HelmFlag", str);
   helm->eHELMFlag = MSHHelmHairHidingBehavior(StringToEnum(str, MSHHelmHairHidingBehaviors));

   hdr.Packets.push_back(helm);
}

void MSHExportImpl::BuildCOLS(INode *node, Object *obj)
{
   if (NULL == colSpheres)
      colSpheres = new COLS();
   ProgressUpdate(Geometry, FormatText("Exporting '%s' as %s", node->GetName(), (*colSpheres).Signature.data()));

   int index = -1;
   if (!node->GetUserPropInt("MMHIndex", index))
   {
      // Default to MMHBoneIndex if MMHIndex is missing in user prop area
      if (INode *parent = node->GetParentNode())
      {
         parent->GetUserPropInt("MMHBoneIndex", index);
      }
   }
   if (index != -1)
   {
      float radius = 0.0f;
      IParamArray *params = obj->GetParamBlock();
      params->GetValue(obj->GetParamBlockIndex(SPHERE_RADIUS), 0, radius, FOREVER);
      if (radius != 0.0f)
      {
         COLSITM itm;
         itm.index = index;
         itm.radius = radius;
         (*colSpheres).items.push_back(itm);
      }
   }
}

bool MSHExportImpl::CalculateProgress(INode *node)
{
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
         ;
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
            ;//
         } 
         else
         {
            if (os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
            {
               ++progressMax[Geometry];
            }
         }
      }
      else if (os.obj && os.obj->SuperClassID()==HELPER_CLASS_ID)
      {
         if (os.obj->ClassID() == Class_ID(POINTHELP_CLASS_ID,0))
         {
            if (wildmatch(hookMatch, node->GetName())) {
               ++progressMax[Geometry];
            } else if (wildmatch(hairMatch, node->GetName())) {
               ++progressMax[Geometry];
            } else if (wildmatch(helmMatch, node->GetName())) {
               ++progressMax[Geometry];
            }
         }
      }
   }
   bool ok = true;
   for (int i=0; i<node->NumberOfChildren() && ok; i++) {
      ok = CalculateProgress(node->GetChildNode(i));
   }
   return true;
}

void MSHExportImpl::ProgressUpdate(ProgressSection section, const TCHAR *s)
{
   //if (o->gi->GetCancel()){
   //   throw CancelExporterException();
   //}
   int total = 1;
   int percent = 1;
   for (int i=0; i< int(ProgressSectionCount); i++){
      total += progressMax[i];
      if (i < section) {
         percent += progressMax[i];
      } else if (i == section ) {
         percent += (++progressCounters[i]);
      }
   }
   o->gi->ProgressUpdate( (percent * 100) / total , s == NULL ? TRUE : FALSE,  const_cast<TCHAR*>(s));
}
#ifdef _DEBUG
void FaceGroup::debug()
{
   for ( int i = 0, n = verts.size(); i < n; ++i ) {
      VertexGroup& vg = verts[ vidx[i] ];
      OutputDebugString(FormatText("%d: %d [%+#.6g,%+#.6g,%+#.6g], [%+#.6g,%+#.6g,%+#.6g], [%+#.6g,%+#.6g,%+#.6g]\n"
         , i, vidx[i]
         ,(double)vg.vert.x, (double)vg.vert.y, (double)vg.vert.z 
         ,(double)vg.norm.x, (double)vg.norm.y, (double)vg.norm.z
         ,(double)vg.uv.x, (double)vg.uv.y, (double)vg.uv.z
         ));
   }
}
#endif
#endif