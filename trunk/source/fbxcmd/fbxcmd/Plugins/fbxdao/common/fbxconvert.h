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

#if 0
inline KFbxXMatrix ToMatrix3(const Matrix44& mx)
{
	KFbxVector4 u(mx[0][0], mx[0][1], mx[0][2]);
	KFbxVector4 v(mx[1][0], mx[1][1], mx[1][2]);
	KFbxVector4 n(mx[2][0], mx[2][1], mx[2][2]);
	KFbxVector4 t(mx[3][0], mx[3][1], mx[3][2]);
	return KFbxXMatrix(u, v, n, t);
}

inline Matrix44 ToMatrix44(const KFbxXMatrix& mx)
{
	Matrix44 fm;
	fm[0][0] = mx[0][0]; fm[1][0] = mx[1][0]; fm[2][0] = mx[2][0]; fm[3][0] = mx[3][0];
	fm[0][1] = mx[0][1]; fm[1][1] = mx[1][1]; fm[2][1] = mx[2][1]; fm[3][1] = mx[3][1];
	fm[0][2] = mx[0][2]; fm[1][2] = mx[1][2]; fm[2][2] = mx[2][2]; fm[3][2] = mx[3][2];
	fm[0][3] = fm[1][3] = fm[2][3] = 0.0f; fm[3][3] = 1.0f;
	return fm;
}

inline KFbxVector4 ToKFbxVector4(const Vector3f& p) { return KFbxVector4(p.x, p.y, p.z); }
inline Point4 ToPoint4(const Vector4f& p) { return Point4(p.x, p.y, p.z, p.w); }
inline KFbxColor ToColor(const Vector3f& p) { return KFbxColor(p.x, p.y, p.z); }
inline KFbxColor ToColor(const Vector4f& p) { return KFbxColor(p.x, p.y, p.z); }
inline Color4 ToColor4(const Vector4f& p) { return Color4(p.x, p.y, p.z, p.w); }

inline bool IsEquivalent(const Vector3f& p1, const KFbxVector4& p2) { return IsEquivalent(p1.x, p2.x) && IsEquivalent(p1.y, p2.y) && IsEquivalent(p1.z, p2.z); }

inline Vector3f ToVector3(const KFbxVector4& p) { return Vector3f(p.x, p.y, p.z); }
inline Vector3f ToVector3(const Point4& p) { return Vector3f(p.x, p.y, p.z); }
inline Vector3f ToVector3(const KFbxColor& c) { return Vector3f(c.r, c.g, c.b); }
inline Vector4f ToVector4(const Point4& p) { return Vector4f(p.x, p.y, p.z, p.w); }
inline Vector4f ToVector4(const KFbxColor& c) { return Vector4f(c.r, c.g, c.b, 1.0f); }
inline Vector4f ToVector4(const Color4& c) { return Vector4f(c.r, c.g, c.b, c.a); }
inline Vector4f ToVector4(const KFbxVector4& p) { return Vector4f(p.x, p.y, p.z, 1.0f); }

#endif