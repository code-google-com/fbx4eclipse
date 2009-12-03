#pragma once

#include "vector3f.h"
#include "vector4f.h"

//Flags
#define POS_IDENT  1
#define ROT_IDENT  2
#define SCL_IDENT  4
#define MAT_IDENT (POS_IDENT|ROT_IDENT|SCL_IDENT)

typedef float MRow[3];

class Quaternion;

class Matrix43 {
	friend class Quaternion;
	static const Matrix43 IDENTITY;
	float m[4][3];
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
	Matrix43(){ flags = 0; }	 // NO INITIALIZATION done in this constructor!! 				 
	Matrix43(BOOL init) {flags=0; IdentityMatrix();} // RB: An option to initialize
    Matrix43(const Vector3f& U, const Vector3f& V, const Vector3f& N, const Vector3f& T) {
        flags = 0; SetRow(0, U); SetRow(1, V); SetRow(2, N); SetRow(3, T);
        ValidateFlags(); }

    Matrix43& Set(const Vector3f& U, const Vector3f& V, const Vector3f& N, const Vector3f& T) {
        flags = 0; SetRow(0, U); SetRow(1, V); SetRow(2, N); SetRow(3, T);
        ValidateFlags(); return *this; }

	Matrix43( const Matrix33& rot );
	Matrix43( float m11, float m12, float m13 
			, float m21, float m22, float m23 
			, float m31, float m32, float m33 
			, float m41, float m42, float m43 );

	Matrix43(const Vector3f& pos, const Quaternion& rot, float scale);
	Matrix43(const Vector3f& pos, const Quaternion& rot, const Vector3f& scale);
	Matrix43(const Vector4f& pos, const Quaternion& rot, float scale);
	Matrix43(const Vector4f& pos, const Quaternion& rot, const Vector3f& scale);

	Matrix43& Set(const Vector3f& pos, const Quaternion& rot, float scale);
	Matrix43& Set(const Vector3f& pos, const Quaternion& rot, const Vector3f& scale);
	Matrix43& Set(const Vector4f& pos, const Quaternion& rot, float scale);
	Matrix43& Set(const Vector4f& pos, const Quaternion& rot, const Vector3f& scale);
    // Data member
    //static const Matrix43 Identity;

	Vector3f GetRow(int i) const { return (*this)[i]; }	

	// Access the translation row
	void SetTrans(const Vector3f p) { (*this)[3] = p; }
	void SetTrans(const Vector4f p) { ((Vector3f&)(*m[3])).Set(p.x, p.y, p.z); }
	void SetTrans(int i, float v) { (*this)[3][i] = v; }
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
	void SetRow(int i,	Vector4f p) {
		m[i][0] = p.x; m[i][1] = p.y; m[i][2] = p.z;
	}
	void ValidateFlags() {}

	//////////////////////////////////////////////////////////////////////////
	Matrix43 operator*( const Matrix43 & rh ) const;
	Matrix43 & operator*=( const Matrix43 & rh );
	Matrix43 operator*( float rh ) const;
	Matrix43 & operator*=( float rh );
	Vector3f operator*( const Vector3f & rh ) const;
	Vector4f operator*( const Vector4f & rh ) const;
	Matrix43 operator+( const Matrix43 & rh ) const;
	Matrix43 & operator+=( const Matrix43 & rh );
	Matrix43 operator-( const Matrix43 & rh ) const;
	Matrix43 & operator-=( const Matrix43 & rh );
	Matrix43 & operator=( const Matrix43 & rh );
	bool operator==( const Matrix43 & rh ) const;
	bool operator!=( const Matrix43 & rh ) const;

	//void Decompose( Vector3f & translate, Matrix33 & rotation, float & scale ) const;
};
