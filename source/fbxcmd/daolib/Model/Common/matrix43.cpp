/* Copyright (c) 2006, NIF File Format Library and Tools
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above
     copyright notice, this list of conditions and the following
     disclaimer in the documentation and/or other materials provided
     with the distribution.

   * Neither the name of the NIF File Format Library and Tools
     project nor the names of its contributors may be used to endorse
     or promote products derived from this software without specific
     prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE. */



#include "stdafx.h"
#include "DAOFormat.h"
#include "vector2f.h"
#include "vector3f.h"
#include "vector4f.h"
#include "color4.h"
#include "quaternion.h"
#include "matrix33.h"
#include "matrix43.h"
#include "matrix44.h"

const Matrix43 Matrix43::IDENTITY( 1.0f, 0.0f, 0.0f,
								  0.0f, 1.0f, 0.0f,
								  0.0f, 0.0f, 1.0f,
								  0.0f, 0.0f, 0.0f );


Matrix43::Matrix43(  float m11, float m12, float m13 
				   , float m21, float m22, float m23 
				   , float m31, float m32, float m33 
				   , float m41, float m42, float m43 )
{
	m[0][0] = m11; m[0][1] = m12; m[0][2] = m13; 
	m[1][0] = m21; m[1][1] = m22; m[1][2] = m23; 
	m[2][0] = m31; m[2][1] = m32; m[2][2] = m33; 
	m[3][0] = m41; m[3][1] = m42; m[3][2] = m43; 
}

Matrix43::Matrix43(const Vector3f& pos, const Quaternion& rot, float scale)
{
	Set(pos, rot, scale);
}

Matrix43::Matrix43(const Vector3f& pos, const Quaternion& rot, const Vector3f& scale)
{
	Set(pos, rot, scale);
}


Matrix43::Matrix43(const Vector4f& pos, const Quaternion& rot, float scale)
{
	Set(pos, rot, scale);
}

Matrix43::Matrix43(const Vector4f& pos, const Quaternion& rot, const Vector3f& scale)
{
	Set(pos, rot, scale);
}

Matrix43::Matrix43( const Matrix33& r )
{
	m[0][0] = r[0][0]; m[0][1] = r[0][1]; m[0][2] = r[0][2]; 
	m[1][0] = r[1][0]; m[1][1] = r[1][1]; m[1][2] = r[1][2]; 
	m[2][0] = r[2][0]; m[2][1] = r[2][1]; m[2][2] = r[2][2]; 
}

Matrix43& Matrix43::Set(const Vector3f& pos, const Quaternion& rot, float scale)
{
	Matrix43 rt;
	rot.MakeMatrix(rt);
	rt.SetRow(3, pos);
	if (scale == 1.0)
	{
		*this = rt;
	}
	else
	{
		Matrix43 s(true);
		s[0][0] = s[1][1] = s[2][2] = 1.0;
		*this = s * rt;
	}
	return *this;
}

Matrix43& Matrix43::Set(const Vector3f& pos, const Quaternion& rot, const Vector3f& scale)
{
	Matrix43 rt;
	rot.MakeMatrix(rt);
	rt.SetRow(3, pos);
	if (scale[0] == 1.0 && scale[1] == 1.0 && scale[2] == 1.0)
	{
		*this = rt;
	}
	else
	{
		Matrix43 s(true);
		s[0][0] = scale.x;
		s[1][1] = scale.y;
		s[2][2] = scale.z;
		*this = s * rt;
	}
	return *this;
}


Matrix43& Matrix43::Set(const Vector4f& pos, const Quaternion& rot, float scale)
{
	Matrix43 rt;
	rot.MakeMatrix(rt);
	rt.SetRow(3, pos);
	if (scale == 1.0)
	{
		*this = rt;
	}
	else
	{
		Matrix43 s(true);
		s[0][0] = s[1][1] = s[2][2] = 1.0;
		*this = s * rt;
	}
	return *this;
}

Matrix43& Matrix43::Set(const Vector4f& pos, const Quaternion& rot, const Vector3f& scale)
{
	Matrix43 rt;
	rot.MakeMatrix(rt);
	rt.SetRow(3, pos);
	if (scale[0] == 1.0 && scale[1] == 1.0 && scale[2] == 1.0)
	{
		*this = rt;
	}
	else
	{
		Matrix43 s(true);
		s[0][0] = scale.x;
		s[1][1] = scale.y;
		s[2][2] = scale.z;
		*this = s * rt;
	}
	return *this;
}

Matrix43 Matrix43::operator*( const Matrix43 & rh ) const {
	return Matrix43(*this) *= rh;
}

Matrix43 & Matrix43::operator*=( const Matrix43 & rh ) {
	Matrix43 r;
	Matrix43 & lh = *this;
	float t;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			t = 0.0f;
			for (int k = 0; k < 3; k++) {
				t += lh[i][k] * rh[k][j];
			}
			r[i][j] = t;
		}
	}

	*this = r;
	return *this;
}

Matrix43 Matrix43::operator*( float rh ) const {
	return Matrix43(*this) *= rh;
}

Matrix43 & Matrix43::operator*=( float rh ) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			(*this)[i][j] *= rh;
		}
	}
	return *this;
}

Vector3f Matrix43::operator*( const Vector3f & rh ) const {
	const Matrix43 & t = *this;
	Vector3f v;
	//Multiply, ignoring w
	v.x = rh.x * t[0][0] + rh.y * t[1][0] + rh.z * t[2][0] + t[3][0];
	v.y = rh.x * t[0][1] + rh.y * t[1][1] + rh.z * t[2][1] + t[3][1];
	v.z = rh.x * t[0][2] + rh.y * t[1][2] + rh.z * t[2][2] + t[3][2];
	//answer[3] = rh[0] * t(0,3) + rh[1] * t(1,3) + rh[2] * t(2,3) + t(3,3);

	return v;
}

Matrix43 Matrix43::operator+( const Matrix43 & rh ) const {
	return Matrix43(*this) += rh;
} 

Matrix43 & Matrix43::operator+=( const Matrix43 & rh ) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			(*this)[i][j] += rh[i][j];
		}
	}
	return *this;
}

Matrix43 Matrix43::operator-( const Matrix43 & rh ) const {
	return Matrix43(*this) -= rh;
} 

Matrix43 & Matrix43::operator-=( const Matrix43 & rh ) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			(*this)[i][j] -= rh[i][j];
		}
	}
	return *this;
}

Matrix43 & Matrix43::operator=( const Matrix43 & rh ) {
	memcpy(m, rh.m, 4 * 3 * sizeof(float));
	return *this;
}

bool Matrix43::operator==( const Matrix43 & rh ) const {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			if ( (*this)[i][j] != rh[i][j] )
				return false;
		}
	}
	return true;
}

bool Matrix43::operator!=( const Matrix43 & rh ) const {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			if ( (*this)[i][j] != rh[i][j] )
				return true;
		}
	}
	return false;
}

//
//void Matrix43::Decompose( Vector3f & translate, Matrix33 & rotation, float & scale ) const {
//	translate = Vector3f( (*this)[3][0], (*this)[3][1], (*this)[3][2] );
//	Matrix33 rotT;
//	for ( int i = 0; i < 3; i++ ){
//		for ( int j = 0; j < 3; j++ ){
//			rotation[i][j] = (*this)[i][j];
//			rotT[j][i] = (*this)[i][j];
//		}
//	}
//	Matrix33 mtx = rotation * rotT;
//	Vector3f scale3( sqrt(mtx[0][0]), sqrt(mtx[1][1]), sqrt(mtx[2][2]) );
//	for ( int i = 0; i < 3; i++ ){
//		for ( int j = 0; j < 3; j++ ){
//			rotation[i][j] /= scale3[i];
//		}
//	}
//	scale = (scale3[0] + scale3[1] + scale3[2]) / 3.0f;
//}


//////////////////////////////////////////////////////////////////////////

Quaternion AsQuaternion(const Matrix43& m) {
	Quaternion quat;
	float tr, s, q[4];
	int i, j, k;

	int nxt[3] = {1, 2, 0};

	// compute the trace of the matrix
	tr = m[0][0] + m[1][1] + m[2][2];

	// check if the trace is positive or negative
	if (tr > 0.0) {
		s = sqrt (tr + 1.0f);
		quat.w = s / 2.0f;
		s = 0.5f / s;
		quat.x = (m[1][2] - m[2][1]) * s;
		quat.y = (m[2][0] - m[0][2]) * s;
		quat.z = (m[0][1] - m[1][0]) * s;
	}
	else {
		// trace is negative
		i = 0;
		if ( m[1][1] > m[0][0])
			i = 1;
		if ( m[2][2] > m[i][i] )
			i = 2;
		j = nxt[i];
		k = nxt[j];

		s = sqrt( ( m[i][i] - (m[j][j] + m[k][k]) ) + 1.0f );
		q[i] = s * 0.5f;
		if (s != 0.0f) s = 0.5f / s;
		q[3] = (m[j][k] - m[k][j]) * s;
		q[j] = (m[i][j] + m[j][i]) * s;
		q[k] = (m[i][k] + m[k][i]) * s;
		quat.x = q[0];
		quat.y= q[1];
		quat.z = q[2];
		quat.w = q[3];
	}

	return quat;
}


Quaternion Matrix43::GetRot() const { return AsQuaternion(*this); }
