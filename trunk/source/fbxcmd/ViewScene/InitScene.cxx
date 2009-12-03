/**************************************************************************************

 Copyright (C) 2001 - 2009 Autodesk, Inc. and/or its licensors.
 All Rights Reserved.

 The coded instructions, statements, computer programs, and/or related material 
 (collectively the "Data") in these files contain unpublished information 
 proprietary to Autodesk, Inc. and/or its licensors, which is protected by 
 Canada and United States of America federal copyright law and by international 
 treaties. 
 
 The Data may not be disclosed or distributed to third parties, in whole or in
 part, without the prior written consent of Autodesk, Inc. ("Autodesk").

 THE DATA IS PROVIDED "AS IS" AND WITHOUT WARRANTY.
 ALL WARRANTIES ARE EXPRESSLY EXCLUDED AND DISCLAIMED. AUTODESK MAKES NO
 WARRANTY OF ANY KIND WITH RESPECT TO THE DATA, EXPRESS, IMPLIED OR ARISING
 BY CUSTOM OR TRADE USAGE, AND DISCLAIMS ANY IMPLIED WARRANTIES OF TITLE, 
 NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE OR USE. 
 WITHOUT LIMITING THE FOREGOING, AUTODESK DOES NOT WARRANT THAT THE OPERATION
 OF THE DATA WILL BE UNINTERRUPTED OR ERROR FREE. 
 
 IN NO EVENT SHALL AUTODESK, ITS AFFILIATES, PARENT COMPANIES, LICENSORS
 OR SUPPLIERS ("AUTODESK GROUP") BE LIABLE FOR ANY LOSSES, DAMAGES OR EXPENSES
 OF ANY KIND (INCLUDING WITHOUT LIMITATION PUNITIVE OR MULTIPLE DAMAGES OR OTHER
 SPECIAL, DIRECT, INDIRECT, EXEMPLARY, INCIDENTAL, LOSS OF PROFITS, REVENUE
 OR DATA, COST OF COVER OR CONSEQUENTIAL LOSSES OR DAMAGES OF ANY KIND),
 HOWEVER CAUSED, AND REGARDLESS OF THE THEORY OF LIABILITY, WHETHER DERIVED
 FROM CONTRACT, TORT (INCLUDING, BUT NOT LIMITED TO, NEGLIGENCE), OR OTHERWISE,
 ARISING OUT OF OR RELATING TO THE DATA OR ITS USE OR ANY OTHER PERFORMANCE,
 WHETHER OR NOT AUTODESK HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH LOSS
 OR DAMAGE. 

**************************************************************************************/

/////////////////////////////////////////////////////////////////////////
//
// This file contains the useful functions to: 
// 1) convert the nurbs and patch attribute types of a scene into mesh 
//    node attributes;
// 2) get the list of all the cameras in the scene.
//
/////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <fbxsdk.h>

#include <fbxfilesdk/fbxfilesdk_nsuse.h>

#include "InitScene.h"
#include "Texture.h"
#include "texloaders.h"

void ConvertNurbsAndPatchRecursive(KFbxSdkManager* pSdkManager, 
                                   KFbxNode* pNode);
void FillCameraArrayRecursive(KFbxNode* pNode, 
                              KArrayTemplate<KFbxNode*>& pCameraArray);
void LoadSupportedTexturesRecursive(KFbxNode* pNode, KArrayTemplate<VSTexture*>& pTextureArray);
void LoadTexture(KFbxTexture* pTexture, KArrayTemplate<VSTexture*>& pTextureArray);


void ConvertNurbsAndPatch(KFbxSdkManager* pSdkManager, KFbxScene* pScene)
{
    ConvertNurbsAndPatchRecursive(pSdkManager, pScene->GetRootNode());
}


void ConvertNurbsAndPatchRecursive(KFbxSdkManager* pSdkManager, KFbxNode* pNode)
{
    KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

    if (lNodeAttribute)
    {
        if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eNURB ||
            lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::ePATCH)
        {
            KFbxGeometryConverter lConverter(pSdkManager);
            lConverter.TriangulateInPlace(pNode);
        }
    }

    int i, lCount = pNode->GetChildCount();

    for (i = 0; i < lCount; i++)
    {
        ConvertNurbsAndPatchRecursive(pSdkManager, pNode->GetChild(i));
    }
}


void FillCameraArray(KFbxScene* pScene, KArrayTemplate<KFbxNode*>& pCameraArray)
{
    pCameraArray.Clear();

    FillCameraArrayRecursive(pScene->GetRootNode(), pCameraArray);
}


void FillCameraArrayRecursive(KFbxNode* pNode, KArrayTemplate<KFbxNode*>& pCameraArray)
{
    int i, lCount;

    if (pNode)
    {
        if (pNode->GetNodeAttribute())
        {
            if (pNode->GetNodeAttribute()->GetAttributeType() == KFbxNodeAttribute::eCAMERA)
            {
                pCameraArray.Add(pNode);
            }
        }

        lCount = pNode->GetChildCount();

        for (i = 0; i < lCount; i++)
        {
            FillCameraArrayRecursive(pNode->GetChild(i), pCameraArray);
        }
    }
}


void FillPoseArray(KFbxScene* pScene, KArrayTemplate<KFbxPose*>& pPoseArray)
{
    int      i, lPoseCount;

    for (i=0, lPoseCount = pScene->GetPoseCount(); i < lPoseCount; i++)
    {
        pPoseArray.Add(pScene->GetPose(i));
    }
}

void LoadSupportedTextures(KFbxScene* pScene, KArrayTemplate<VSTexture*>& pTextureArray)
{
    pTextureArray.Clear();

    LoadSupportedTexturesRecursive(pScene->GetRootNode(), pTextureArray);
}

void LoadSupportedTexturesRecursive(KFbxNode* pNode, KArrayTemplate<VSTexture*>& pTextureArray)
{
    if (pNode)
    {
        int i, lCount;
        KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

        if (lNodeAttribute)
        {
            KFbxLayerContainer* lLayerContainer = NULL;

            switch (lNodeAttribute->GetAttributeType())
            {
            case KFbxNodeAttribute::eNURB:
                lLayerContainer = pNode->GetNurb();
                break;

            case KFbxNodeAttribute::ePATCH:
                lLayerContainer = pNode->GetPatch();
                break;

            case KFbxNodeAttribute::eMESH:
                lLayerContainer = pNode->GetMesh();
                break;
            }

            if (lLayerContainer){
                int lNbMat = pNode->GetSrcObjectCount(KFbxSurfaceMaterial::ClassId);
                for (int lMaterialIndex = 0; lMaterialIndex < lNbMat; lMaterialIndex++){
                    KFbxSurfaceMaterial *lMaterial = KFbxCast <KFbxSurfaceMaterial>(pNode->GetSrcObject(KFbxSurfaceMaterial::ClassId, lMaterialIndex));
                    if(lMaterial){                                                         
						//{
						//	for ( KFbxProperty lProperty = lMaterial->GetFirstProperty()
						//		; lProperty.IsValid()
						//		; lProperty = lMaterial->GetNextProperty(lProperty) )
						//	{
						//		KString kName = lProperty.GetName();
						//		KString kHName = lProperty.GetHierarchicalName();
						//		KFbxDataType dType = lProperty.GetPropertyDataType();;
						//		dType = dType;
						//	}
						//}
                        KFbxProperty lProperty = lMaterial->FindProperty(KFbxSurfaceMaterial::sDiffuse);
                        if(lProperty.IsValid()){
                            int lNbTex = lProperty.GetSrcObjectCount(KFbxTexture::ClassId);
                            for (int lTextureIndex = 0; lTextureIndex < lNbTex; lTextureIndex++){
                                KFbxTexture* lTexture = KFbxCast <KFbxTexture> (lProperty.GetSrcObject(KFbxTexture::ClassId, lTextureIndex)); 
                                if(lTexture)
                                    LoadTexture(lTexture, pTextureArray);
                            }
                        }
                    }
                }
            } 

        }

        lCount = pNode->GetChildCount();

        for (i = 0; i < lCount; i++)
        {
            LoadSupportedTexturesRecursive(pNode->GetChild(i), pTextureArray);
        }
    }
}

void LoadTexture(KFbxTexture* pTexture, KArrayTemplate<VSTexture*>& pTextureArray)
{
    // First find if the texture is already loaded
    int i, lCount = pTextureArray.GetCount();

    for (i = 0; i < lCount; i++)
    {
        if (pTextureArray[i]->mRefTexture == pTexture) return;
    }

    // Right now, only Targa textures are loaded by this sample
    KString lFileName = pTexture->GetFileName();

	VSTexture* lTexture = new VSTexture();

	KError lError;

	if ( LoadTexture(lFileName, lTexture, lError) ) 
	{
		lTexture->mRefTexture = pTexture;
		pTextureArray.Add(lTexture);
	}
	else
	{
		delete lTexture;
	}
}

void PreparePointCacheData(KFbxScene* pScene)
{
    // This function show how to cycle thru scene elements in a linear way.
    int lIndex, lNodeCount = KFbxGetSrcCount<KFbxNode>(pScene);

    for (lIndex=0; lIndex<lNodeCount; lIndex++)
    {
        KFbxNode* lNode = KFbxGetSrc<KFbxNode>(pScene, lIndex);

        if (lNode->GetGeometry()) 
        {
            int i, lVertexCacheDeformerCount = lNode->GetGeometry()->GetDeformerCount(KFbxDeformer::eVERTEX_CACHE);

            // There should be a maximum of 1 Vertex Cache Deformer for the moment
            lVertexCacheDeformerCount = lVertexCacheDeformerCount > 0 ? 1 : 0;

            for (i=0; i<lVertexCacheDeformerCount; ++i )
            {
                // Get the Point Cache object
                KFbxVertexCacheDeformer* lDeformer = static_cast<KFbxVertexCacheDeformer*>(lNode->GetGeometry()->GetDeformer(i, KFbxDeformer::eVERTEX_CACHE));
                if( !lDeformer ) continue;
                KFbxCache* lCache = lDeformer->GetCache();
                if( !lCache ) continue;

                // Process the point cache data only if the constraint is active
                if (lDeformer->IsActive())
                {
                    if (lCache->GetCacheFileFormat() == KFbxCache::ePC2)
                    {
                        // This code show how to convert from PC2 to MC point cache format
                        // turn it on if you need it.
#if 0 
                        if (!lCache->ConvertFromPC2ToMC(KFbxCache::eMC_ONE_FILE, 
                            KTime::GetFrameRate(pScene->GetGlobalTimeSettings().GetTimeMode())))
                        {
                            // Conversion failed, retrieve the error here
                            KString lTheErrorIs = lCache->GetError().GetLastErrorString();
                        }
#endif
                    }
                    else if (lCache->GetCacheFileFormat() == KFbxCache::eMC)
                    {
                        // This code show how to convert from MC to PC2 point cache format
                        // turn it on if you need it.
                        //#if 0 
                        if (!lCache->ConvertFromMCToPC2(KTime::GetFrameRate(pScene->GetGlobalTimeSettings().GetTimeMode()), 0))
                        {
                            // Conversion failed, retrieve the error here
                            KString lTheErrorIs = lCache->GetError().GetLastErrorString();
                        }
                        //#endif
                    }


                    // Now open the cache file to read from it
                    if (!lCache->OpenFileForRead())
                    {
                        // Cannot open file 
                        KString lTheErrorIs = lCache->GetError().GetLastErrorString();

                        // Set the deformer inactive so we don't play it back
                        lDeformer->SetActive(false);
                    }
                }
            }
        }
    }
}
