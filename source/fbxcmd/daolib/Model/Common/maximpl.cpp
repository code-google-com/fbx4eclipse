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

//#include "strclass.h"

const Vector2f Vector2f::Origin(0.0f, 0.0f);
const Vector3f Vector3f::Origin(0.0f, 0.0f, 0.0f);
const Vector4f Vector4f::Origin(0.0f, 0.0f, 0.0f, 0.0f);



Matrix33 AsMatrix(const Quaternion& q) {
	Matrix33 m(true);

	float w2 = q.w * q.w;
	float x2 = q.x * q.x;
	float y2 = q.y * q.y;
	float z2 = q.z * q.z;

	m[0][0] = w2 + x2- y2 - z2;
	m[0][1] = 2.0f*q.x*q.y - 2.0f*q.w*q.z;
	m[0][2] = 2.0f*q.x*q.z + 2.0f*q.w*q.y;

	m[1][0] = 2.0f*q.x*q.y + 2.0f*q.w*q.z;
	m[1][1] = w2 - x2 + y2 - z2;
	m[1][2] = 2.0f*q.y*q.z - 2.0f*q.w*q.x;

	m[2][0] = 2.0f*q.x*q.z - 2.0f*q.w*q.y;
	m[2][1] = 2.0f*q.y*q.z + 2.0f*q.w*q.x;
	m[2][2] = w2 - x2 - y2 + z2;

	return m;
}


Vector3f AsEulerYawPitchRoll(const Quaternion& q) {
	float yaw, pitch, roll;

	if ( q.x*q.y + q.z*q.w == 0.5 ) {
		//North Pole
		yaw = 2 * atan2(q.x,q.w);
		pitch = asin(2*q.x*q.y + 2*q.z*q.w);
		roll = 0.0f;
	}
	else if ( q.x*q.y + q.z*q.w == -0.5 ) {
		//South Pole
		yaw =  -2 * atan2(q.x,q.w);
		pitch = asin(2*q.x*q.y + 2*q.z*q.w);
		roll = 0.0f;
	}
	else {
		yaw = atan2(2*q.y*q.w-2*q.x*q.z , 1 - 2*q.y*q.y - 2*q.z*q.z);
		pitch = asin(2*q.x*q.y + 2*q.z*q.w);
		roll = atan2(2*q.x*q.w-2*q.y*q.z , 1 - 2*q.x*q.x - 2*q.z*q.z);
	}

	return Vector3f( yaw, pitch, roll );
}


//////////////////////////////////////////////////////////////////////////

void Quaternion::MakeMatrix(Matrix33& m) const
{
	m = ::AsMatrix(*this);
}

void Quaternion::MakeMatrix(Matrix43& m) const
{
	m = Matrix43(::AsMatrix(*this));
}

void Quaternion::MakeMatrix(Matrix44& m) const
{
	m = Matrix44(::AsMatrix(*this));
}

Vector3f Quaternion::AsEulerYawPitchRoll() const
{
	return ::AsEulerYawPitchRoll(*this);
}

//////////////////////////////////////////////////////////////////////////

extern int singles2halfp(void *target, void *source, int numel);
extern int doubles2halfp(void *target, void *source, int numel);
extern int halfp2singles(void *target, void *source, int numel);
extern int halfp2doubles(void *target, void *source, int numel);

float16::float16() : val(0) {}

float float16::asFloat32() const
{
	float result;
	halfp2singles(&result, const_cast<unsigned short*>(&val), 1);
	return result;
	//if (0 < e && e < 31)
	//	return (s ? -1.0f : 1.0f) * powf(2.f, (e - 15.f)) * (1.0f + m / 1024.f);
	//else if (m == 0)
	//	return (s ? -0.0f : 0.0f);
	//else if (m != 0)
	//	return (s ? -1.0f : 1.0f) * powf(2.f, -14.f) * (m / 1024.f);
	//else if (e == 31 && m == 0)
	//	return s ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity();
	//else if (e == 31 && m != 0)
	//	return std::numeric_limits<float>::quiet_NaN();
	//return 0.0f;
}

//////////////////////////////////////////////////////////////////////////


Signed_10_10_10::Signed_10_10_10() 
	: x(0), y(0), z(0), w(0) 
	{}

int Signed_10_10_10::operator[](int idx) const {
	switch (idx){
		case 0: return x;
		case 1: return y;
		case 2: return z;
	}
	return 0;
}


Unsigned_10_10_10::Unsigned_10_10_10() 
	: x(0), y(0), z(0), w(0) 
{}

unsigned Unsigned_10_10_10::operator[](int idx) const {
	switch (idx){
		case 0: return x;
		case 1: return y;
		case 2: return z;
	}
	return 0;
}
