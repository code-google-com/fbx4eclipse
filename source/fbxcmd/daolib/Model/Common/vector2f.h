#pragma once
#include <vector>
class Vector2f 
{
public:
	float x,y;
   static const Vector2f Origin;

	Vector2f() { x=y=0.0f; }
	Vector2f(float X, float Y)  { x = X; y = Y;  }
	Vector2f(double X, double Y)  { x = (float)X; y = (float)Y;  }
	Vector2f(const Vector2f& a) { x = a.x; y = a.y; } 
	Vector2f(float af[2]) { x = af[0]; y = af[1]; }

   float& operator[](int i) { return (&x)[i]; }     
   const float& operator[](int i) const { return (&x)[i]; }  

   void Set(float X, float Y)  { x = X; y = Y;  }
   //void Set(double X, double Y)  { x = (float)X; y = (float)Y;  }
   void Set(const Vector2f& a) { x = a.x; y = a.y; } 
   void Set(float af[2]) { x = af[0]; y = af[1]; }

};

typedef std::vector<Vector2f> Vector2fArray;
