#pragma once

#include "Vector4.h"
#include "Vector3.h"
#include "Sphere.h"

class Plane
{
public:
	Plane() : normal(Vector3::up), distance(0) {};
	Plane(const Vector3& normal, float distance, bool toNormalise = false) : distance(distance) { this->normal = toNormalise?normal.Normalised():normal; };
	Plane(const Vector4& normalDis, bool toNormalise = false) : distance(normalDis.w) { Vector3 newNormal = Vector3(normalDis.x, normalDis.y, normalDis.z); 
																						this->normal = toNormalise?newNormal.Normalised(): newNormal; };
	Plane(const Plane& other) = default;
	Plane& operator=(const Plane& other) = default;
	virtual ~Plane() = default;

	Vector3 normal;
	float distance;
};

