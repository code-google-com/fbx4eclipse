/**********************************************************************
*<
FILE: maxconvert.h

DESCRIPTION:	Miscellaneous Utilities

HISTORY: 

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

// Max Headers
#include <Max.h>
#include <color.h>
//#include "DataHeader.h"

#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof((x)[0]))
#endif


// Simple conversion helpers
//static inline float TODEG(float x) { return x * 180.0f / PI; }
//static inline float TORAD(float x) { return x * PI / 180.0f; }

static inline Color TOCOLOR(const KFbxVector4& c3) {
	return Color(c3[0], c3[1], c3[2]);
}

static inline KFbxVector4 TOCOLOR3(const Color& c3) {
	return KFbxVector4(c3.r, c3.g, c3.b);
}

static inline Point3 TOPOINT3(const KFbxVector4& v){
	return Point3(v[0], v[1], v[2]);
}

static inline KFbxVector4 TOVECTOR3(const Point3& v){
	return KFbxVector4(v.x, v.y, v.z, 1.0);
}

static inline KFbxVector4 TOVECTOR4(const Point3& v, float w = 0.0){
	return KFbxVector4(v.x, v.y, v.z, w);
}


static inline KFbxQuaternion TOQUAT(const Quat& q, bool inverse = false){
	KFbxQuaternion qt(q.x, q.y, q.z, q.w);
	if (inverse) qt.Conjugate();
	return qt;
}

static inline Quat TOQUAT(const KFbxQuaternion& q, bool inverse = false){
	//return (inverse && q.w != FloatNegINF) ? TOQUAT(q.Inverse(), false) : Quat(q.w, q.x, q.y, q.z);
	Quat qt(q[0], q[1], q[2], q[3]);
	return inverse ? Inverse(qt) : qt;
}

static inline Matrix3 TOMATRIX3(const KFbxXMatrix &tm, bool invert = false){
	Matrix3 m(TOPOINT3(tm.GetRow(0)), TOPOINT3(tm.GetRow(1)), TOPOINT3(tm.GetRow(2)), TOPOINT3(tm.GetRow(3)));
	if (invert) m.Invert();
	return m;
}

static inline KFbxXMatrix TOMATRIX4(const Matrix3 &tm, bool invert = false){
	KFbxXMatrix mx;
	Point3& r0 = tm.GetRow(0), &r1 = tm.GetRow(1), &r2 = tm.GetRow(2), &r3 = tm.GetRow(3);
	mx[0][0] = r0[0]; mx[0][1] = r0[1]; mx[0][2] = r0[2]; mx[0][3] = r0[3];
	mx[1][0] = r1[0]; mx[1][1] = r1[1]; mx[1][2] = r1[2]; mx[1][3] = r1[3];
	mx[2][0] = r2[0]; mx[2][1] = r2[1]; mx[2][2] = r2[2]; mx[2][3] = r2[3];
	mx[3][0] = r3[0]; mx[3][1] = r3[1]; mx[3][2] = r3[2]; mx[3][3] = r3[3];
	return mx;
}

static inline Matrix3 TOMATRIX3(const KFbxVector4& trans, const Quat& quat, float scale){
	Matrix3 tm;
	KFbxQuaternion q(quat.x, quat.y, quat.z, quat.w);
	KFbxXMatrix qm; qm.SetQ(q);
	tm.SetTranslate(TOPOINT3(trans));
	tm *= TOMATRIX3(qm);
	tm *= ScaleMatrix(Point3(scale, scale, scale));
	return tm;
}

static inline Point3 GetScale(const Matrix3& mtx){
	return Point3( fabs(mtx.GetRow(0)[0]), fabs(mtx.GetRow(1)[1]), fabs(mtx.GetRow(2)[2]) );
}

static inline float Average(const Point3& val) {
	return (val[0] + val[1] + val[2]) / 3.0f;
}

static inline float Average(const KFbxVector4& val) {
	return (val[0] + val[1] + val[2]) / 3.0f;
}

inline Matrix3 ToMatrix3(const KFbxXMatrix& mx)
{
	Point3 u(mx[0][0], mx[0][1], mx[0][2]);
	Point3 v(mx[1][0], mx[1][1], mx[1][2]);
	Point3 n(mx[2][0], mx[2][1], mx[2][2]);
	Point3 t(mx[3][0], mx[3][1], mx[3][2]);
	return Matrix3(u, v, n, t);
}

inline KFbxXMatrix ToMatrix44(const Matrix3& mx)
{
	KFbxXMatrix fm;
	fm[0][0] = mx[0][0]; fm[1][0] = mx[1][0]; fm[2][0] = mx[2][0]; fm[3][0] = mx[3][0];
	fm[0][1] = mx[0][1]; fm[1][1] = mx[1][1]; fm[2][1] = mx[2][1]; fm[3][1] = mx[3][1];
	fm[0][2] = mx[0][2]; fm[1][2] = mx[1][2]; fm[2][2] = mx[2][2]; fm[3][2] = mx[3][2];
	fm[0][3] = fm[1][3] = fm[2][3] = 0.0f; fm[3][3] = 1.0f;
	return fm;
}
