/**********************************************************************
*<
FILE: DAOFormat.h

DESCRIPTION:	DA:O File Format

HISTORY:
*>	Copyright (c) 2007, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include "DAOFormat.h"

using namespace std;

Matrix43 TOMATRIX(Quaternion const& q)
{
	Matrix43 m(true);
	float w = q.w, x = q.x, y = q.y, z = q.z;
	float w2 = w * w, x2 = x * x, y2 = y * y, z2 = z * z;
	m.SetRow(0, Vector3f(w2 + x2- y2 - z2, 2.0f*x*y - 2.0f*w*z, 2.0f*x*z + 2.0f*w*y));
	m.SetRow(1, Vector3f(2.0f*x*y + 2.0f*w*z, w2 - x2 + y*y - z2, 2.0f*y*z - 2.0f*w*x));
	m.SetRow(2, Vector3f(2.0f*x*z - 2.0f*w*y, 2.0f*y*z + 2.0f*w*x, w2 - x2 - y2 + z2));
	return m;
}

Vector3f TOEULER(const Matrix43 &m)
{
	Vector3f rv(0.0f, 0.0f, 0.0f);
	if ( m.GetRow(2)[0] < 1.0 )
	{
		if ( m.GetRow(2)[0] > - 1.0 )
		{
			rv[2] = atan2( - m.GetRow(1)[0], m.GetRow(0)[0] );
			rv[1] = asin( m.GetRow(2)[0] );
			rv[0] = atan2( - m.GetRow(2)[1], m.GetRow(2)[2] );
		}
		else
		{
			rv[2] = - atan2( - m.GetRow(1)[2], m.GetRow(1)[1] );
			rv[1] = - float(M_PI) / 2.0f;
			rv[0] = 0.0;
		}
	}
	else
	{
		rv[2] = atan2( m.GetRow(1)[2], m.GetRow(1)[1] );
		rv[1] = float(M_PI) / 2.0f;
		rv[0] = 0.0;
	}
	return rv;
}

template<> void Dump(IDAODumpStream& out, LPCTSTR name, char const & val)
{
	char buf[33];
	sprintf(buf, "%d", (unsigned int)val);
	Dump(out, name, buf);
}

template<> void Dump(IDAODumpStream& out, LPCTSTR name, unsigned char const & val)
{
	char buf[33];
	sprintf(buf, "%d", (unsigned int)val);
	Dump(out, name, buf);
}

template<> void Dump(IDAODumpStream& out, LPCTSTR name, float const & val)
{
	char buf[33];
	sprintf(buf, "%g", (double)val);
	Dump(out, name, buf);
}

template<> void Dump(IDAODumpStream& out, LPCTSTR name, Vector2f const & val)
{
	char buffer[256];
	sprintf(buffer, "[%f, %f]", val.x, val.y);
	Dump(out, name, buffer);
}

template<> void Dump(IDAODumpStream& out, LPCTSTR name, Vector3f const & val)
{
	char buffer[256];
	sprintf(buffer, "[%7.4f,%7.4f,%7.4f]", val.x, val.y, val.z);
	Dump(out, name, buffer);
}

template<> void Dump(IDAODumpStream& out, LPCTSTR name, Vector4f const & val)
{
	char buffer[256];
	sprintf(buffer, "[%7.4f,%7.4f,%7.4f,%7.4f]", val.x, val.y, val.z, val.w);
	Dump(out, name, buffer);
}

template<> void Dump(IDAODumpStream& out, LPCTSTR name, Quaternion const & val)
{
	char buffer[256];
	sprintf(buffer, "[%7.4f,%7.4f,%7.4f,%7.4f]", val.x, val.y, val.z, val.w);
	Dump(out, name, buffer);
}

template<> void Dump(IDAODumpStream& out, LPCTSTR name, ColorRef const & val)
{
	char buffer[256];
	sprintf(buffer, "[%d, %d, %d, %d]", (int)val.a, (int)val.r, (int)val.b, (int)val.g);
	Dump(out, name, buffer);
}

template<> void Dump(IDAODumpStream& out, LPCTSTR name, Color4 const & val)
{
	char buffer[256];
	sprintf(buffer, "[%7.4f,%7.4f,%7.4f]", (int)val.r, (int)val.b, (int)val.g);
	Dump(out, name, buffer);
}

void Init(const VARIABLE* defmap, LPVOID val) {
	for ( ; defmap->ShortName; ++defmap)
		memcpy((LPVOID)((LPBYTE)val + defmap->MemberAddr), defmap->DefaultValue, defmap->ValueSize);
}

bool Recall(const VARIABLE* defmap, IDAOStream& in, LPVOID val) { 
	int cur = in.Tell();
	cur = cur;
	for ( ; defmap->ShortName; ++defmap) {
		if (defmap->SizeType != NULL) {
			uint size = defmap->SizeType->toSize((LPVOID)((LPBYTE)val + defmap->SizeAddr));
			defmap->MemberType->resize(in, (LPVOID)((LPBYTE)val + defmap->MemberAddr), size);
		}
		int mcur = in.Tell();
		if (!defmap->MemberType->recall(in, (LPVOID)((LPBYTE)val + defmap->MemberAddr))) 
			return false;
	}
	return true;
} 
bool Store(const VARIABLE* defmap, IDAOStream& out, LPCVOID val){ 
	bool ok = true;
	for ( ; ok && defmap->ShortName; ++defmap) {
		uint size = defmap->MemberType->toSize((LPVOID)((LPBYTE)val + defmap->MemberAddr));
		defmap->SizeType->resize(out, (LPVOID)((LPBYTE)val + defmap->SizeAddr), size);
	}
	for ( ; ok && defmap->ShortName; ++defmap) 
		ok |= defmap->MemberType->store(out, (LPCVOID)((LPCBYTE)val + defmap->MemberAddr)); 
	return ok; 
} 

void Dump(const VARIABLE* defmap, LPCTSTR type, IDAODumpStream& out, LPCTSTR name, LPCVOID val){ 
	out.Indent();
	out.PrintF(_T("%-30s\t%s\n"), type, name);
	out.IncreaseIndent();
	for ( ; defmap->ShortName; ++defmap) 
		defmap->MemberType->dump(out, defmap->ShortName, (LPCVOID)((LPCBYTE)val + defmap->MemberAddr)); 
	out.DecreaseIndent();
} 

uint SizeOf(const VARIABLE* defmap, IDAOStream& in, LPCVOID val) {
	uint size = 0;
	for ( ; defmap->ShortName; ++defmap) 
		size += defmap->MemberType->SizeOf(in, (LPCVOID)((LPCBYTE)val + defmap->MemberAddr)); 
	return size; 
}
