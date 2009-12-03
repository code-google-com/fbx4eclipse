#pragma once
#include <vector>
class Vector3f 
{
public:
	float x,y,z;
   static const Vector3f Origin;

	Vector3f(){x=y=z=0.0f;}
	Vector3f(float X, float Y, float Z)  { x = X; y = Y; z = Z;  }
	Vector3f(double X, double Y, double Z) { x = (float)X; y = (float)Y; z = (float)Z; }
	Vector3f(int X, int Y, int Z) { x = (float)X; y = (float)Y; z = (float)Z; }
	Vector3f(const Vector3f& a) { x = a.x; y = a.y; z = a.z; } 
    float& operator[](int i) { return (&x)[i]; }     
    const float& operator[](int i) const { return (&x)[i]; }  

	void Set(float X, float Y, float Z)  { x = X; y = Y; z = Z;  }
	//void Set(double X, double Y, double Z) { x = (float)X; y = (float)Y; z = (float)Z; }
	//void Set(int X, int Y, int Z) { x = (float)X; y = (float)Y; z = (float)Z; }
	void Set(const Vector3f& a) { x = a.x; y = a.y; z = a.z; } 

	float Length() { return sqrt(x*x+y*y+z*z); }
};

typedef std::vector<Vector3f> Vector3fArray;
