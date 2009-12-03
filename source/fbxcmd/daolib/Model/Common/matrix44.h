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
class Matrix44 {
	friend class Quaternion;
	static const Matrix44 IDENTITY;
	float m[4][4];
	// Access i-th row as Vector4f for read or assignment:
	DWORD flags;

public:
	Vector4f& operator[](int i) { return((Vector4f&)(*m[i]));  }
	const Vector4f& operator[](int i) const { return((Vector4f&)(*m[i])); }
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
	Matrix44();
	Matrix44(BOOL init) {flags=0; IdentityMatrix();} // RB: An option to initialize
	Matrix44(
		float m11, float m12, float m13, float m14,
		float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34,
		float m41, float m42, float m43, float m44
		);

    Matrix44(const Vector4f& U, const Vector4f& V, const Vector4f& N, const Vector4f& T) {
        flags = 0; SetRow(0, U); SetRow(1, V); SetRow(2, N); SetRow(3, T);
        ValidateFlags(); }

    Matrix44& Set(const Vector4f& U, const Vector4f& V, const Vector4f& N, const Vector4f& T) {
        flags = 0; SetRow(0, U); SetRow(1, V); SetRow(2, N); SetRow(3, T);
        ValidateFlags(); return *this; }

	Matrix44( const Matrix33 & r );
	Matrix44( const Vector3f & t, const Matrix33 & r, float scale );
	Matrix44(const Vector3f& pos, const Quaternion& rot, float scale);
	Matrix44(const Vector3f& pos, const Quaternion& rot, const Vector3f& scale);
	Matrix44(const Vector4f& pos, const Quaternion& rot, float scale);
	Matrix44(const Vector4f& pos, const Quaternion& rot, const Vector3f& scale);

	Matrix44& Set(const Vector3f& pos, const Quaternion& rot, float scale);
	Matrix44& Set(const Vector3f& pos, const Quaternion& rot, const Vector3f& scale);
	Matrix44& Set(const Vector4f& pos, const Quaternion& rot, float scale);
	Matrix44& Set(const Vector4f& pos, const Quaternion& rot, const Vector3f& scale);
    // Data member
    //static const Matrix44 Identity;

	Vector4f GetRow(int i) const { return (*this)[i]; }	

	// Access the translation row
	void SetTrans(const Vector3f p) { ((Vector4f&)(*m[3])).Set(p.x, p.y, p.z, 1.0); }
	void SetTrans(const Vector4f p) { (*this)[3] = p;  flags &= ~POS_IDENT; }
	void SetTrans(int i, float v) { (*this)[3][i] = v; flags &= ~POS_IDENT; }

	void IdentityMatrix() {
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
		m[1][0] = m[2][0] = m[3][0] = 0.0f;
		m[0][1] = m[2][1] = m[3][1] = 0.0f;
		m[0][2] = m[1][2] = m[3][2] = 0.0f;
		m[0][3] = m[1][3] = m[2][3] = 0.0f;
	}
	void SetRow(int i,	Vector3f p) {
		m[i][0] = p.x; m[i][1] = p.y; m[i][2] = p.z; m[i][3] = 1.0;
	}
	void SetRow(int i,	Vector4f p) {
		m[i][0] = p.x; m[i][1] = p.y; m[i][2] = p.z; m[i][3] = p.w;
	}
	void ValidateFlags() {}

	//////////////////////////////////////////////////////////////////////////
	Matrix44 operator*( const Matrix44 & rh ) const;
	Matrix44 & operator*=( const Matrix44 & rh );
	Matrix44 operator*( float rh ) const;
	Matrix44 & operator*=( float rh );
	Vector3f operator*( const Vector3f & rh ) const;
	Vector4f operator*( const Vector4f & rh ) const;
	Matrix44 operator+( const Matrix44 & rh ) const;
	Matrix44 & operator+=( const Matrix44 & rh );
	Matrix44 operator-( const Matrix44 & rh ) const;
	Matrix44 & operator-=( const Matrix44 & rh );
	Matrix44 & operator=( const Matrix44 & rh );
	bool operator==( const Matrix44 & rh ) const;
	bool operator!=( const Matrix44 & rh ) const;

	Matrix44 Transpose() const;
	float Determinant() const;
	Matrix44 Inverse() const;
	Matrix33 Submatrix( int skip_r, int skip_c ) const;
	float Adjoint( int skip_r, int skip_c ) const;

    void Decompose( Vector3f & translate, Matrix33 & rotation, float & scale ) const;
	void Decompose( Vector3f & translate, Matrix33 & rotation, Vector3f & scale ) const;

	void Decompose( Vector3f & translate, Quaternion & rotation, float & scale ) const;
	void Decompose( Vector3f & translate, Quaternion & rotation, Vector3f & scale ) const;

	Vector3f GetTranslation() const;
	Matrix33 GetRotation() const;
	float GetScale() const;

	Vector4f GetTrans() const { return (*this)[3]; }  
	Quaternion GetRot() const;
	float GetUniformScale() const;

};

