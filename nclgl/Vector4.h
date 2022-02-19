/*
Class:Vector4
Implements:
Author:Rich Davison
Description:VERY simple Vector4 class. Students are encouraged to modify this as necessary!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/
#pragma once

#include "Vector3.h"

using namespace std;

class Vector4	{
public:
	Vector4(void) {
		x = y = z = w = 1.0f;
	}
	Vector4(float x, float y, float z, float w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
	Vector4(const Vector3& vec3, float w) {
		this->x = vec3.x;
		this->y = vec3.y;
		this->z = vec3.z;
		this->w = w;
	}

	Vector3 ToVector3() {
		return Vector3(x, y, z);
	}

	inline Vector4 operator+(const Vector4& other) {
		return Vector4(x + other.x,
					y + other.y,
					z + other.z,
					w + other.w);
	}

	inline Vector4 operator-(const Vector4& other) {
		return Vector4(x - other.x,
					y - other.y,
					z - other.z,
					w - other.w);
	}

	inline void operator+=(const Vector4& a) {
		x += a.x;
		y += a.y;
		z += a.z;
		w += a.w;
	}

	inline void operator-=(const Vector4& a) {
		x -= a.x;
		y -= a.y;
		z -= a.z;
		w -= a.w;
	}

	Vector4 Normalised() const {
		Vector4 n = *this;
		n.Normalise();
		return n;
	}

	void	Normalise() {
		float length = Length();

		if (length != 0.0f) {
			length = 1.0f / length;
			x = x * length;
			y = y * length;
			z = z * length;
			w = w * length;
		}
	}

	float			Length() const {
		return sqrt((x * x) + (y * y) + (z * z) + (w * w));
	}

	static Vector4	Max(const Vector4& a, const Vector4& b) {
		return Vector4(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
	}

	static Vector4	Min(const Vector4& a, const Vector4& b) {
		return Vector4(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
	}

	inline Vector4  operator*(const float a) const {
		return Vector4(x * a, y * a, z * a, w * a);
	}
	inline void operator*=(const float a) {
		x *= a; y *= a; z *= a; w *= a;
	}
	/*inline Vector4  operator*(const Vector4& a) const {
		return Vector4(x * a.x, y * a.y, z * a.z, w * a.w);
	}*/

	~Vector4(void){}

	static const Vector4 zero;
	static const Vector4 one;

	float x;
	float y;
	float z;
	float w;
};
