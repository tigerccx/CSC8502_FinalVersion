#pragma once

#include <array>

#include "Vector3.h"
#include "Matrix4.h"

using namespace std;

enum BoxVertex {
	BoxVertex_RBN, //Right Bottom Near
	BoxVertex_LBN, //Left Bottom Near
	BoxVertex_LTN, //Left Top Near
	BoxVertex_RTN, //Left Top Near
	BoxVertex_RBF, //Right Bottom Far
	BoxVertex_LBF, //Left Bottom Far
	BoxVertex_LTF, //Left Top Far
	BoxVertex_RTF, //Left Top Far
};

class Box
{
public:
	Box() : vertexMin(Vector3::zero), vertexMax(Vector3::zero){}
	Box(const Vector3& vertexMin, const Vector3& vertexMax) : vertexMin(vertexMin), vertexMax(vertexMax) {}
	Box(const Box& other) = default;
	Box& operator=(const Box& other) = default;
	virtual ~Box() = default;
	
	Box operator|(const Box& other) {
		Box boxReturn;
		boxReturn.vertexMin = Vector3::Min(this->vertexMin, other.vertexMin);
		boxReturn.vertexMax = Vector3::Max(this->vertexMax, other.vertexMax);
		return boxReturn;
	}
	Box operator&(const Box& other) {
		Box boxReturn;
		Vector3 vMin = Vector3::Max(this->vertexMin, other.vertexMin);
		Vector3 vMax = Vector3::Min(this->vertexMax, other.vertexMax);
		Vector3 vecLength = vMax - vMin;
		if (vecLength.x > 0 && vecLength.y > 0 && vecLength.z > 0) {
			boxReturn.vertexMin = vMin;
			boxReturn.vertexMax = vMax;
		}
		return boxReturn;
	}

	inline bool IsAllZero() const { Vector3 vecLen = GetLengthXYZ(); return (vecLen.x == 0 && vecLen.y == 0 && vecLen.z == 0); }
	inline bool IsAnyZero() const { Vector3 vecLen = GetLengthXYZ(); return (vecLen.x == 0 || vecLen.y == 0 || vecLen.z == 0); }
	inline Vector3 GetLengthXYZ() const { return vertexMax - vertexMin; }
	inline Vector3 GetCenter() const { return (vertexMax + vertexMin) / 2.0f; }
	inline void GetVertices(array<Vector3, 8>& vertices) const {
		Vector3 vecLength = GetLengthXYZ();
		Vector3 vecX, vecY;
		vecX = vecY = Vector3::zero;
		vecX.x = vecLength.x;
		vecY.y = vecLength.y;
		vertices[BoxVertex_RBN] = vertexMin + vecX;
		vertices[BoxVertex_LBN] = vertexMin;
		vertices[BoxVertex_LTN] = vertexMin + vecY;
		vertices[BoxVertex_RTN] = vertexMin + vecY + vecX;
		vertices[BoxVertex_RBF] = vertexMax - vecY;
		vertices[BoxVertex_LBF] = vertexMax - vecY - vecX;
		vertices[BoxVertex_LTF] = vertexMax - vecX;
		vertices[BoxVertex_RTF] = vertexMin;
	}
	inline void SetBoxMinMax(const Vector3& vertexMin, const Vector3& vertexMax) { this->vertexMin = vertexMin; this->vertexMax = vertexMax; }
	inline void SetBoxMinLength(const Vector3& vertexMin, const Vector3& lengthXYZ) { this->vertexMin = vertexMin; this->vertexMax = vertexMin+lengthXYZ; }
	inline void SetBoxCentLength(const Vector3& vertexCent, const Vector3& lengthXYZ) { Vector3 halfLengthXYZ = lengthXYZ / 2.0f; this->vertexMin = vertexCent - halfLengthXYZ; this->vertexMax = vertexCent + halfLengthXYZ; }

	static Box ModifyByModelMatrix(const Box& box, const Matrix4& modelMatrix) {
		array<Vector3, 8> vertices;
		box.GetVertices(vertices);
		Vector3 vertexMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
		Vector3 vertexMax = Vector3(FLT_MIN, FLT_MIN, FLT_MIN);
		for (int i = 0; i < 8; ++i) {
			vertices[i] = modelMatrix * vertices[i];
			vertexMin = Vector3::Min(vertexMin, vertices[i]);
			vertexMax = Vector3::Max(vertexMax, vertices[i]);
		}
		return Box(vertexMin, vertexMax);
	}

	Vector3 vertexMin;
	Vector3 vertexMax;
};

