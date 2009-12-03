/**********************************************************************
*<
FILE: fbxconvert.h

DESCRIPTION:	Miscellaneous Utilities

HISTORY: 

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

// Max Headers
#include <fbxsdk.h>
#include "DataHeader.h"

#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof((x)[0]))
#endif


// Simple conversion helpers
//static inline float TODEG(float x) { return x * 180.0f / PI; }
//static inline float TORAD(float x) { return x * PI / 180.0f; }

static inline KFbxColor TOCOLOR(const Color4& c3) {
	return KFbxColor(c3.r, c3.g, c3.b, c3.a);
}

static inline Color4 TOCOLOR3(const KFbxColor& c3) {
	return Color4(c3.mRed, c3.mGreen, c3.mBlue, c3.mAlpha);
}

static inline Color4 TOCOLOR3(const KFbxVector4& c3) {
	return Color4(c3[0], c3[1], c3[2], c3[3]);
}

static inline KFbxVector4 TOVECTOR4(const Color4& c3){
	return KFbxVector4(c3.r, c3.g, c3.b);
}

static inline KFbxVector4 TOVECTOR4(const Vector3f& v){
	return KFbxVector4(v.x, v.y, v.z);
}

static inline KFbxVector4 TOVECTOR4(const Vector4f& v){
	return KFbxVector4(v.x, v.y, v.z);
}

static inline Vector3f TOVECTOR3(const KFbxVector4& v){
	return Vector3f(v[0], v[1], v[2]);
}

static inline Vector3f TOVECTOR3(const Vector4f& v){
	return Vector3f(v.x, v.y, v.z);
}

static inline Vector4f TOVECTOR4(const KFbxVector4& v){
	return Vector4f(v[0], v[1], v[2], v[3]);
}


static inline KFbxQuaternion TOQUAT(const Quaternion& q, bool inverse = false){
	KFbxQuaternion qt(q.x, q.y, q.z, q.w);
	if (inverse) {
		KFbxXMatrix m4; m4.SetQ(qt); 
		return m4.Inverse().GetQ();
	} else { 
		return qt;
	}
}

static inline Quaternion TOQUAT(const KFbxQuaternion& q, bool inverse = false){
	if (inverse) {
		KFbxXMatrix m4; m4.SetQ(q);
		KFbxQuaternion qt = m4.Inverse().GetQ();
		return Quaternion(qt[0], qt[1], qt[2], qt[3]);
	} else {
		return Quaternion(q[0], q[1], q[2], q[3]);
	}
}
//
//static inline KFbxXMatrix TOMATRIX3(const Matrix44 &tm, bool invert = false){
//	Vector3f pos; Matrix43 rot; float scale;
//	tm.Decompose(pos, rot, scale);
//	KFbxXMatrix m(rot.rows[0].data, rot.rows[1].data, rot.rows[2].data, KFbxVector4());
//	if (invert) m.Invert();
//	m.Scale(KFbxVector4(scale, scale, scale));
//	m.SetTrans(KFbxVector4(pos.x, pos.y, pos.z));
//	return m;
//}

static inline Matrix43 TOMATRIX33(const KFbxXMatrix &tm, bool invert = false){
	if (invert) {
		return TOMATRIX33(tm.Inverse(), false);
	} else {
		return Matrix43(TOVECTOR3(tm.GetRow(0)),TOVECTOR3(tm.GetRow(1)),TOVECTOR3(tm.GetRow(2)),TOVECTOR3(tm.GetRow(3)));
	}
}

static inline KFbxXMatrix TOMATRIX3(const Vector4f& trans, const Quaternion& quat, float scale) {
	KFbxXMatrix tm; tm.SetTQS( TOVECTOR4(trans), TOQUAT(quat), KFbxVector4(scale, scale, scale, 1.0f) );
	return tm;
}

static inline KFbxXMatrix TOMATRIX3(const Vector3f& trans, const Quaternion& quat, float scale) {
	KFbxXMatrix tm; tm.SetTQS( TOVECTOR4(trans), TOQUAT(quat), KFbxVector4(scale, scale, scale, 1.0f) );
	return tm;
}

static inline Matrix44 TOMATRIX4(const KFbxXMatrix &tm, bool invert = false){
	if (invert) {
		return TOMATRIX4(tm.Inverse());
	} else {
		return Matrix44(TOVECTOR4(tm.GetRow(0)),TOVECTOR4(tm.GetRow(1)),TOVECTOR4(tm.GetRow(2)),TOVECTOR4(tm.GetRow(3)));
	}
}

static inline KFbxVector4 GetScale(const KFbxXMatrix& mtx){
	return KFbxVector4( fabs(mtx.GetRow(0)[0]), fabs(mtx.GetRow(1)[1]), fabs(mtx.GetRow(2)[2]), 1.0f );
}

static inline float Average(const KFbxVector4& val) {
	return (val[0] + val[1] + val[2]) / 3.0f;
}

static inline float Average(const Vector3f& val) {
	return (val.x + val.y + val.z) / 3.0f;
}

enum PosRotScale
{
	prsPos = 0x1,
	prsRot = 0x2,
	prsScale = 0x4,
	prsDefault = prsPos | prsRot | prsScale,
};
void PosRotScaleNode(KFbxNode *n, KFbxVector4 & p, KFbxQuaternion& q, float s, PosRotScale prs = prsDefault);
void PosRotScaleNode(KFbxNode *n, KFbxVector4 & p, KFbxQuaternion& q, KFbxVector4 & s, PosRotScale prs = prsDefault);
void PosRotScaleNode(KFbxNode *n, KFbxXMatrix& m3, PosRotScale prs = prsDefault);

static inline KFbxQuaternion ComposeSphericalXYZ(KFbxVector4 lR)
{
	KFbxVector4 lT(0,0,0,0);
	KFbxVector4 lS(1,1,1,0);
	KFbxXMatrix m(lT, lR, lS);
	Matrix44 m4 = TOMATRIX4(m);
	Vector3f pos; Quaternion rot; Vector3f scl;
	m4.Decompose(pos, rot, scl);
	return KFbxQuaternion(rot.x, rot.y, rot.z, rot.w);
}

static inline KFbxXMatrix GetLocalTransformFromDefaultTake( KFbxNode * lNode )
{
	KFbxNode * lParent = lNode->GetParent();
	KFbxXMatrix wm = lNode->GetGlobalFromDefaultTake();
	if (lParent == NULL)
	{
		return wm;
	}
	else
	{
		KFbxXMatrix pm = lParent->GetGlobalFromDefaultTake();
		wm = pm.Inverse() * wm;
		return wm;
	}
}

static inline KFbxXMatrix GetLocalTransform( KFbxNode * lNode )
{
	KFbxVector4 lT(0,0,0,0), lR(0,0,0,0), lS(1,1,1,0);
	return KFbxXMatrix ( lNode->GetDefaultT(lT), lNode->GetDefaultR(lR), lNode->GetDefaultS(lS) );
}


static inline KFbxVector4 CalculateCenter(KFbxGeometryBase* pMesh)
{
	pMesh->ComputeBBox();
	fbxDouble3 min = pMesh->BBoxMin.Get();
	fbxDouble3 max = pMesh->BBoxMax.Get();
	return KFbxVector4((min[0]+max[0])/2.0, (min[1]+max[1])/2.0, (min[2]+max[2])/2.0, 1.0);
}
