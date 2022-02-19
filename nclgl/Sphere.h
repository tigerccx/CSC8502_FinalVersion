#pragma once

#include "Vector3.h"
#include "Matrix4.h"

class Sphere
{
public:
	Sphere() : center(Vector3::zero), radius(0) {}
	Sphere(const Vector3& center, float radius) : center(center), radius(radius) {}
	Sphere(const Sphere& other) = default;
	Sphere& operator=(const Sphere& other) = default;
	virtual ~Sphere() = default;

	Vector3 center;
	float radius;
};

