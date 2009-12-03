#pragma once

#include "vector3f.h"

//Flags
#define POS_IDENT  1
#define ROT_IDENT  2
#define SCL_IDENT  4
#define MAT_IDENT (POS_IDENT|ROT_IDENT|SCL_IDENT)

typedef float MRow[3];

class Quaternion;

class Matrix33 {
	friend class Quaternion;
	static const Matrix33 IDENTITY;
	float m[3][3];
	// Access i-th row as Vector3f for read or assignment:
	DWORD flags;

public:
	Vector3f& operator[](int i) { return((Vector3f&)(*m[i]));  }
	const Vector3f& operator[](int i) const { return((Vector3f&)(*m[i])); }
	// if change any components directly via GetAddr, must call this
	void SetNotIdent() { flags &= ~MAT_IDENT; }
	void SetIdentFlags(DWORD f) { flags &= ~MAT_IDENT; flags |= f; }
	DWORD GetIdentFlags() const { return flags; }
	void ClearIdentFlag(DWORD f) { flags &= ~f; }
	BOOL IsIdentity() const { return ((flags&MAT_IDENT)==MAT_IDENT); }

	// CAUTION: if you change the matrix via this pointer, you MUST clear the
	// proper IDENT flags !!!
	MRow* GetAddr() { return (MRow *)(m); }
	const MRow* GetAddr() const { return (MRow *)(m); }

	// Constructors
	Matrix33();
	Matrix33(BOOL init) {flags=0; IdentityMatrix();} // RB: An option to initialize
    Matrix33(const Vector3f& U, const Vector3f& V, const Vector3f& N, const Vector3f& T) {
        flags = 0; SetRow(0, U); SetRow(1, V); SetRow(2, N); SetRow(3, T);
        ValidateFlags(); }

	Matrix33(
		float m11, float m12, float m13,
		float m21, float m22, float m23,
		float m31, float m32, float m33
		);


    Matrix33& Set(const Vector3f& U, const Vector3f& V, const Vector3f& N, const Vector3f& T) {
        flags = 0; SetRow(0, U); SetRow(1, V); SetRow(2, N); SetRow(3, T);
        ValidateFlags(); return *this; }

	Matrix33(const Vector3f& pos, const Quaternion& rot, float scale);
	Matrix33(const Vector3f& pos, const Quaternion& rot, const Vector3f& scale);
	Matrix33& Set(const Vector3f& pos, const Quaternion& rot, float scale);
	Matrix33& Set(const Vector3f& pos, const Quaternion& rot, const Vector3f& scale);

    // Data member
    //static const Matrix33 Identity;

	Vector3f GetRow(int i) const { return (*this)[i]; }	

	// Access the translation row
	void SetTrans(const Vector3f p) { (*this)[3] = p;  flags &= ~POS_IDENT; }
	void SetTrans(int i, float v) { (*this)[3][i] = v; flags &= ~POS_IDENT; }
	Vector3f GetTrans() const { return (*this)[3]; }  
	Quaternion GetRot() const;
	float GetUniformScale() const;

	void IdentityMatrix() {
		m[0][0] = m[1][1] = m[2][2] = 1.0f;
		m[1][0] = m[2][0] = m[0][1] = m[2][1] = m[0][2] = m[1][2] = 0.0f;
	}
	void SetRow(int i,	Vector3f p) {
		m[i][0] = p.x; m[i][1] = p.y; m[i][2] = p.z;
	}
	void ValidateFlags() {}

	//////////////////////////////////////////////////////////////////////////
	Quaternion AsQuaternion();
	float Determinant() const;
    Matrix33 operator*( const Matrix33 & m ) const;
};
