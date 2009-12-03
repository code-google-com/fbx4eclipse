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


const Matrix33 Matrix33::IDENTITY( 1.0f, 0.0f, 0.0f,
								  0.0f, 1.0f, 0.0f,
								  0.0f, 0.0f, 1.0f );

Matrix33::Matrix33() {
	*this = Matrix33::IDENTITY;
}

Matrix33::Matrix33(  float m11, float m12, float m13
				   , float m21, float m22, float m23
				   , float m31, float m32, float m33 )
{
	m[0][0] = m11; m[0][1] = m12; m[0][2] = m13;
	m[1][0] = m21; m[1][1] = m22; m[1][2] = m23;
	m[2][0] = m31; m[2][1] = m32; m[2][2] = m33;
}

Quaternion Matrix33::AsQuaternion() {
	Quaternion quat;
	float tr, s, q[4];
	int i, j, k;

	int nxt[3] = {1, 2, 0};

	Matrix33 & m = *this;

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

float Matrix33::Determinant() const {
	return  (*this)[0][0] * ( (*this)[1][1] * (*this)[2][2] - (*this)[1][2] * (*this)[2][1] )
		- (*this)[0][1] * ( (*this)[1][0] * (*this)[2][2] - (*this)[1][2] * (*this)[2][0] )
		+ (*this)[0][2] * ( (*this)[1][0] * (*this)[2][1] - (*this)[1][1] * (*this)[2][0] );
}

Matrix33 Matrix33::operator*( const Matrix33 & m ) const
{
	Matrix33 m3;
	for ( int r = 0; r < 3; r++ ){
		for ( int c = 0; c < 3; c++ ){
			m3[r][c] = (*this)[r][0]*m[0][c] + (*this)[r][1]*m[1][c] + (*this)[r][2]*m[2][c];
		}
	}
	return m3;
}