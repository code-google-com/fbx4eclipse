#pragma once

#include "vector3f.h"
#include "vector4f.h"
class Color4 {
public:
	float r,g,b,a;

	// Constructors
	Color4()  { r = g = b = a = 1.0f; }
	Color4(float R, float G, float B, float A=1.0f)  { r = R; g = G; b = B; a=A;  }
	Color4(double R, double G, double B, double A=1.0) { r = (float)R; g = (float)G; b = (float)B; a = (float)A; }
	Color4(int R, int G, int B, int A) { r = (float)R; g = (float)G; b = (float)B; a = (float)A; }
	Color4(const Color4& v) { r = v.r; g = v.g; b = v.b; a = v.a; } 
	explicit Color4(DWORD rgb);  // from Windows RGB value
	Color4(Vector3f p) { r = p.x; g = p.y; b = p.z; }
	Color4(Vector4f p) { r = p.x; g = p.y; b = p.z; a = p.w; }
	Color4(float af[], int len = 3) { r = af[0]; g = af[1]; b = af[2]; a = len > 3 ? af[3] : 1.0f; }
	// Access operators
	float& operator[](int i) { return (&r)[i]; }     
	const float& operator[](int i) const { return (&r)[i]; }  

   // Convert to Vector3f
	operator Vector3f() { return Vector3f(r,g,b); }
	operator Vector4f() { return Vector4f(r,g,b,a); }


	void Set(float R, float G, float B, float A=1.0f)  { r = R; g = G; b = B; a=A;  }
	void Set(const Color4& v) { r = v.r; g = v.g; b = v.b; a = v.a; } 
	void Set(DWORD rgb);  // from Windows RGB value
	void Set(const Vector3f& p) { r = p.x; g = p.y; b = p.z; }
	void Set(const Vector4f& p) { r = p.x; g = p.y; b = p.z; a = p.w; }
	void Set(float af[], int len = 3) { r = af[0]; g = af[1]; b = af[2]; a = len > 3 ? af[3] : 1.0f; }

};




inline BYTE GetR(COLORREF argb) { return ((BYTE)(argb)); }
inline BYTE GetG(COLORREF argb) { return (LOBYTE(((WORD)(argb)) >> 8)); }
inline BYTE GetB(COLORREF argb) { return (LOBYTE((argb)>>16)); }
inline BYTE GetA(COLORREF argb) { return ((BYTE)((argb)>>24)); }

inline COLORREF ToARGB(BYTE a, BYTE r, BYTE g, BYTE b)
{
	return ((COLORREF)((((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16))|(((DWORD)(BYTE)(a))<<24)));
}

#pragma region ColorRef
typedef struct ColorRef
{
	BYTE a,b,g,r;

	// Constructors
	ColorRef(){ a=0xFF, b=0, g=0, r=0; }
	ColorRef(COLORREF argb)  { a=GetA(argb), r=GetR(argb), g=GetG(argb), b=GetB(argb); }
	ColorRef(BYTE R, BYTE G, BYTE B, BYTE A=0xFF)  { a=A, r=R, g=G, b=B; }
	ColorRef(float R, float G, float B, float A=1.0f)  { a=ToByte(A), r=ToByte(R), g=ToByte(G), b=ToByte(B); }
	ColorRef(const ColorRef& c) { a = c.a; r = c.r; g = c.g; b = c.b; } 
	ColorRef(float af[3]) { a = 0xFF, r=ToByte(af[0]), g=ToByte(af[1]), b=ToByte(af[2]); }
	ColorRef(const Vector3f& pt) { a = 0xFF, r=ToByte(pt.x), g=ToByte(pt.y), b=ToByte(pt.z); }
	//ColorRef(const Color4& c) { a = 0xFF, r=ToByte(c.r), g=ToByte(c.g), b=ToByte(c.b); }

	static inline BYTE ToByte(float val) { return BYTE(val * 255.0f); }
	static inline float ToFloat(BYTE val) { return float(val) / 255.0f; }
	static inline BYTE GetR(COLORREF argb) { return ((BYTE)(argb)); }
	static inline BYTE GetG(COLORREF argb) { return (LOBYTE(((WORD)(argb)) >> 8)); }
	static inline BYTE GetB(COLORREF argb) { return (LOBYTE((argb)>>16)); }
	static inline BYTE GetA(COLORREF argb) { return ((BYTE)((argb)>>24)); }
	static inline COLORREF ToARGB(BYTE a, BYTE r, BYTE g, BYTE b) {
		return ((COLORREF)((((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16))|(((DWORD)(BYTE)(a))<<24)));
	}

	// Unary operators
	ColorRef operator-() const { return(ColorRef(a,-r,-g,-b)); } 
	ColorRef operator+() const { return *this; }

	// Relational operators
	int operator==(const ColorRef& c) const { return ((a == c.a) && (r == c.r) && (g == c.g) && (b == c.b)); }
	int operator!=(const ColorRef& c) const { return ((a != c.a) || (r != c.r) || (g != c.g) || (b != c.b)); }

	operator Vector3f() { return Vector3f(ToFloat(r), ToFloat(g), ToFloat(b)); }
	//operator Color4() { return Color4(ToFloat(r), ToFloat(g), ToFloat(b)); }
} ColorRef;
#pragma endregion