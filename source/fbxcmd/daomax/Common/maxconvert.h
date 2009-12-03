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
#include "DataHeader.h"

#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof((x)[0]))
#endif


// Simple conversion helpers
//static inline float TODEG(float x) { return x * 180.0f / PI; }
//static inline float TORAD(float x) { return x * PI / 180.0f; }

static inline Color TOCOLOR(const Color4& c3) {
	return Color(c3.r, c3.g, c3.b);
}

static inline Color4 TOCOLOR3(const Color& c3) {
	return Color4(c3.r, c3.g, c3.b);
}

static inline Color4 TOCOLOR3(const Point3& c3) {
	return Color4(c3.x, c3.y, c3.z);
}

static inline Point3 TOPOINT3(const Color4& c3){
	return Point3(c3.r, c3.g, c3.b);
}

static inline Point3 TOPOINT3(const Vector3f& v){
	return Point3(v.x, v.y, v.z);
}

static inline Point3 TOPOINT3(const Vector4f& v){
	return Point3(v.x, v.y, v.z);
}

static inline Vector3f TOVECTOR3(const Point3& v){
	return Vector3f(v.x, v.y, v.z);
}

static inline Vector3f TOVECTOR3(const Vector4f& v){
	return Vector3f(v.x, v.y, v.z);
}

static inline Vector4f TOVECTOR4(const Point3& v, float w = 0.0){
	return Vector4f(v.x, v.y, v.z, w);
}


static inline Quat TOQUAT(const Quaternion& q, bool inverse = false){
	Quat qt(q.x, q.y, q.z, q.w);
	return (inverse) ? qt.Inverse() : qt;
}

static inline Quaternion TOQUAT(const Quat& q, bool inverse = false){
	//return (inverse && q.w != FloatNegINF) ? TOQUAT(q.Inverse(), false) : Quaternion(q.w, q.x, q.y, q.z);
	return Quaternion(q.x, q.y, q.z, q.w);
}

static inline Matrix3 TOMATRIX3(const Matrix44 &tm, bool invert = false){
	Matrix3 m(TOPOINT3(tm.GetRow(0)), TOPOINT3(tm.GetRow(1)), TOPOINT3(tm.GetRow(2)), TOPOINT3(tm.GetRow(3)));
	if (invert) m.Invert();
	return m;
}
static inline Matrix3 TOMATRIX3(const Matrix43 &tm, bool invert = false){
	Matrix3 m(TOPOINT3(tm.GetRow(0)), TOPOINT3(tm.GetRow(1)), TOPOINT3(tm.GetRow(2)), TOPOINT3(tm.GetRow(3)));
	if (invert) m.Invert();
	return m;
}

static inline Matrix43 TOMATRIX33(const Matrix3 &tm, bool invert = false){
	return Matrix43(TOVECTOR3(tm.GetRow(0)),TOVECTOR3(tm.GetRow(1)),TOVECTOR3(tm.GetRow(2)),TOVECTOR3(tm.GetRow(3)));
}

static inline Matrix3 TOMATRIX3(const Vector4f& trans, const Quaternion& quat, float scale){
	Matrix3 tm;
	Quat q(quat.x, quat.y, quat.z, quat.w);
	Matrix43 qm(true); quat.MakeMatrix(qm);
	tm.SetTranslate(TOPOINT3(trans));
	tm *= TOMATRIX3(qm);
	tm *= ScaleMatrix(Point3(scale, scale, scale));
	return tm;
}
static inline Matrix3 TOMATRIX3(const Vector3f& trans, const Quaternion& quat, float scale){
	Matrix3 tm, qm;
	Quat q(quat.x, quat.y, quat.z, quat.w);
	q.MakeMatrix(qm);
	tm.SetTranslate(TOPOINT3(trans));
	tm *= qm;
	tm *= ScaleMatrix(Point3(scale, scale, scale));
	return tm;
}

static inline Matrix44 TOMATRIX4(const Matrix3 &tm, bool invert = false){
	return Matrix44(TOVECTOR4(tm.GetRow(0)),TOVECTOR4(tm.GetRow(1)),TOVECTOR4(tm.GetRow(2)),TOVECTOR4(tm.GetRow(3)));
}

static inline Point3 GetScale(const Matrix3& mtx){
	return Point3( fabs(mtx.GetRow(0)[0]), fabs(mtx.GetRow(1)[1]), fabs(mtx.GetRow(2)[2]) );
}

static inline float Average(const Point3& val) {
	return (val[0] + val[1] + val[2]) / 3.0f;
}

static inline float Average(const Vector3f& val) {
	return (val.x + val.y + val.z) / 3.0f;
}

#if 0
inline Matrix3 ToMatrix3(const Matrix44& mx)
{
	Point3 u(mx[0][0], mx[0][1], mx[0][2]);
	Point3 v(mx[1][0], mx[1][1], mx[1][2]);
	Point3 n(mx[2][0], mx[2][1], mx[2][2]);
	Point3 t(mx[3][0], mx[3][1], mx[3][2]);
	return Matrix3(u, v, n, t);
}

inline Matrix44 ToMatrix44(const Matrix3& mx)
{
	Matrix44 fm;
	fm[0][0] = mx[0][0]; fm[1][0] = mx[1][0]; fm[2][0] = mx[2][0]; fm[3][0] = mx[3][0];
	fm[0][1] = mx[0][1]; fm[1][1] = mx[1][1]; fm[2][1] = mx[2][1]; fm[3][1] = mx[3][1];
	fm[0][2] = mx[0][2]; fm[1][2] = mx[1][2]; fm[2][2] = mx[2][2]; fm[3][2] = mx[3][2];
	fm[0][3] = fm[1][3] = fm[2][3] = 0.0f; fm[3][3] = 1.0f;
	return fm;
}

inline Point3 ToPoint3(const Vector3f& p) { return Point3(p.x, p.y, p.z); }
inline Point4 ToPoint4(const Vector4f& p) { return Point4(p.x, p.y, p.z, p.w); }
inline Color ToColor(const Vector3f& p) { return Color(p.x, p.y, p.z); }
inline Color ToColor(const Vector4f& p) { return Color(p.x, p.y, p.z); }
inline Color4 ToColor4(const Vector4f& p) { return Color4(p.x, p.y, p.z, p.w); }

inline bool IsEquivalent(const Vector3f& p1, const Point3& p2) { return IsEquivalent(p1.x, p2.x) && IsEquivalent(p1.y, p2.y) && IsEquivalent(p1.z, p2.z); }

inline Vector3f ToVector3(const Point3& p) { return Vector3f(p.x, p.y, p.z); }
inline Vector3f ToVector3(const Point4& p) { return Vector3f(p.x, p.y, p.z); }
inline Vector3f ToVector3(const Color& c) { return Vector3f(c.r, c.g, c.b); }
inline Vector4f ToVector4(const Point4& p) { return Vector4f(p.x, p.y, p.z, p.w); }
inline Vector4f ToVector4(const Color& c) { return Vector4f(c.r, c.g, c.b, 1.0f); }
inline Vector4f ToVector4(const Color4& c) { return Vector4f(c.r, c.g, c.b, c.a); }
inline Vector4f ToVector4(const Point3& p) { return Vector4f(p.x, p.y, p.z, 1.0f); }

#endif