#pragma once
#include <math.h>
#include <float.h>
#include <limits>

#pragma pack(push, 1)

struct Signed_10_10_10
{
	Signed_10_10_10();

	int x : 10;
	int y : 10;
	int z : 10;
	int w : 2;

	int operator[](int idx) const;
};
struct Unsigned_10_10_10
{
	Unsigned_10_10_10();

	unsigned x : 10;
	unsigned y : 10;
	unsigned z : 10;
	unsigned w : 2;

	unsigned operator[](int idx) const;
};

struct float16
{
	float16();

	unsigned short val;

	float asFloat32() const; 
	operator float() const { return asFloat32(); }
};


#pragma region array
/*! Used to enable static arrays to be members of vectors */
template<int size, class T>
struct array {
	array() {
		for ( uint i = 0; i < size; ++i ) {
			data[i] = T();
		}
	}
	~array() {}
	T & operator[]( unsigned int index ) {
		return data[index];
	}
	const T & operator[]( unsigned int index ) const {
		return data[index];
	}

	int count() const 
	{
		return int(size);
	}

private:
	T data[size];
	static const int _size = size;
};
#pragma endregion

typedef array<4, byte> ARGB4;
typedef array<4, byte> UByte4;
typedef array<2, short> Short2;
typedef array<3, short> Short3;
typedef array<4, short> Short4;
typedef array<2, unsigned short> UShort2;
typedef array<3, unsigned short> UShort3;
typedef array<4, unsigned short> UShort4;
typedef array<2, float16> Float16_2;
typedef array<4, float16> Float16_4;


#pragma pack(pop)
