#pragma once
class Matrix33;
class Matrix43;
class Matrix44;

class Quaternion 
{
public:
	float x,y,z,w;

	// Constructors
	Quaternion(): x(0.0f),y(0.0f),z(0.0f),w(1.0f) {}
	Quaternion(float X, float Y, float Z, float W)  { x = X; y = Y; z = Z; w = W; }
	Quaternion(double X, double Y, double Z, double W)  { 
		x = (float)X; y = (float)Y; z = (float)Z; w = (float)W; 
		}
	Quaternion(const Quaternion& a) { x = a.x; y = a.y; z = a.z; w = a.w; } 
	Quaternion(float af[4]) { x = af[0]; y = af[1]; z = af[2]; w = af[3]; }

	// Access operators
	float& operator[](int i) { return (&x)[i]; }     
	const float& operator[](int i) const { return (&x)[i]; }
    
	float Scalar() { return w; }
	Vector3f Vector() { return Vector3f(x, y, z); }

	// Conversion function
	operator float*() { return(&x); }

	// Unary operators
	Quaternion operator-() const { return(Quaternion(-x,-y,-z,-w)); } 
	Quaternion operator+() const { return *this; }
    
	Quaternion& Set(float X, float Y, float Z, float W)
		{ x = X; y = Y; z = Z; w = W; return *this; }
	Quaternion& Set(double X, double Y, double Z, double W)
		{ x = (float)X; y = (float)Y; z = (float)Z; w = (float)W;
		return *this; }
	Quaternion& Set(const Vector3f& V, float W)
		{ x = V.x; y = V.y; z = V.z; w = W; return *this; } 

	void Identity() { x = y = z = (float)0.0; w = (float) 1.0; }

	void MakeMatrix(Matrix33& m) const;
	void MakeMatrix(Matrix43& m) const;
	void MakeMatrix(Matrix44& m) const;

	Vector3f AsEulerYawPitchRoll() const;

	Quaternion Quaternion::Inverse() const {
		return Quaternion(-x, -y, -z, w);
	}

};

