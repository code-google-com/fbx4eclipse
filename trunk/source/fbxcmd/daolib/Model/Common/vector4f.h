#pragma once
#include <vector>

class Vector4f {
public:
	float x,y,z,w;
   static const Vector4f Origin;

	// Constructors
	Vector4f(){ x=y=z=w=0.0f; }
	Vector4f(float X, float Y, float Z, float W=0.0f)  { x = X; y = Y; z = Z; w = W; }
	Vector4f(double X, double Y, double Z, double W=0.0f) { x = (float)X; y = (float)Y; z = (float)Z; w = (float)W; }
	Vector4f(int X, int Y, int Z, int W=0) { x = (float)X; y = (float)Y; z = (float)Z; w = (float)W; }
	Vector4f(const Vector3f& a, float W=0.0f) { x = a.x; y = a.y; z = a.z; w = W; } 
	Vector4f(const Vector4f& a) { x = a.x; y = a.y; z = a.z; w = a.w; } 
	Vector4f(float af[4]) { x = af[0]; y = af[1]; z = af[2]; w = af[3]; }

   float& operator[](int i) { return (&x)[i]; }     
   const float& operator[](int i) const { return (&x)[i]; }  

   void Set(float X, float Y, float Z, float W=0.0f)  { x = X; y = Y; z = Z; w = W; }
   //void Set(double X, double Y, double Z, double W=0.0f) { x = (float)X; y = (float)Y; z = (float)Z; w = (float)W; }
   void Set(const Vector3f& a, float W=0.0f) { x = a.x; y = a.y; z = a.z; w = W; } 
   void Set(const Vector4f& a) { x = a.x; y = a.y; z = a.z; w = a.w; } 
   void Set(float af[4]) { x = af[0]; y = af[1]; z = af[2]; w = af[3]; }

   float Length() { return sqrt(x*x+y*y+z*z+w*w); }
};


typedef std::vector<Vector4f> Vector4fArray;
