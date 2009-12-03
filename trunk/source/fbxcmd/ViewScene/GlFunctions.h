#ifndef _GL_FUNCTIONS_H
#define _GL_FUNCTIONS_H

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

void GlGetWindowSize(int& pWidth, int& pHeight);

void GlSetCameraPerspective(double pFieldOfViewY,
							double pAspect,
							double pNearPlane,
							double pFarPlane,
							KFbxVector4& pEye,
							KFbxVector4& pCenter,
							KFbxVector4& pUp);
void GlSetCameraOrthogonal(double pLeftPlane,
						   double pRightPlane,
						   double pBottomPlane,
						   double pTopPlane,
						   double pNearPlane,
						   double pFarPlane,
						   KFbxVector4& pEye,
						   KFbxVector4& pCenter,
                           KFbxVector4& pUp);

void GlDrawMarker(KFbxXMatrix& pGlobalPosition);
void GlDrawLimbNode(KFbxXMatrix& pGlobalBasePosition, 
					KFbxXMatrix& pGlobalEndPosition);
void GlDrawMesh(KFbxXMatrix &pGlobalPosition, 
				KFbxMesh* pMesh, 
				KFbxVector4* pVertexArray,
				int pDrawMode);
void GlDrawCamera(KFbxXMatrix& pGlobalPosition, 
				  double pRoll);
void GlDrawLight(KFbxXMatrix& pGlobalPosition, 
				 KFbxLight* pLight, 
				 KFbxColor& pColor, 
				 double pConeAngle);
void GlDrawCrossHair(KFbxXMatrix& pGlobalPosition);
void GlDrawGrid(KFbxXMatrix& pGlobalPosition);

#define DRAW_MODE_WIREFRAME 0
#define DRAW_MODE_LIGHTED   1
#define DRAW_MODE_TEXTURED  2

#endif // #ifndef _GL_FUNCTIONS_H


