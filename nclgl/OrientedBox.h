#pragma once

#include <array>

#include "Vector3.h"

using namespace std;

enum OrientedBoxVertex {
	OrientedBoxVertex_RBN, //Right Bottom Near
	OrientedBoxVertex_LBN, //Left Bottom Near
	OrientedBoxVertex_LTN, //Left Top Near
	OrientedBoxVertex_RTN, //Left Top Near
	OrientedBoxVertex_RBF, //Right Bottom Far
	OrientedBoxVertex_LBF, //Left Bottom Far
	OrientedBoxVertex_LTF, //Left Top Far
	OrientedBoxVertex_RTF, //Left Top Far
};

class OrientedBox
{
public:
	OrientedBox() = default;
	OrientedBox(const OrientedBox& other) = default;
	OrientedBox& operator=(const OrientedBox& other) = default;
	virtual ~OrientedBox() = default;

	array<Vector3, 6> vertices;
};

