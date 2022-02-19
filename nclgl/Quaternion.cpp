/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "Quaternion.h"
#include "Matrix4.h"
#include "Matrix3.h"
#include "Vector3.h"
#include "Vector4.h"
#include "common.h"

#include <algorithm>
#include <cmath>

Quaternion::Quaternion(void)
{
	x = y = z = 0.0f;
	w = 1.0f;
}

Quaternion::Quaternion(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Quaternion::Quaternion(const Vector3& vector, float w) {
	this->x = vector.x;
	this->y = vector.y;
	this->z = vector.z;
	this->w = w;
}

Quaternion::Quaternion(const Matrix4& m) {
	w = sqrt(max(0.0f, (1.0f + m.values[0] + m.values[5] + m.values[10]))) * 0.5f;

	if (abs(w) < 0.0001f) {
		x = sqrt(max(0.0f, (1.0f + m.values[0] - m.values[5] - m.values[10]))) / 2.0f;
		y = sqrt(max(0.0f, (1.0f - m.values[0] + m.values[5] - m.values[10]))) / 2.0f;
		z = sqrt(max(0.0f, (1.0f - m.values[0] - m.values[5] + m.values[10]))) / 2.0f;

		x = (float)copysign(x, m.values[9] - m.values[6]);
		y = (float)copysign(y, m.values[2] - m.values[8]);
		z = (float)copysign(z, m.values[4] - m.values[1]);
	}
	else {
		float qrFour = 4.0f * w;
		float qrFourRecip = 1.0f / qrFour;

		x = (m.values[6] - m.values[9]) * qrFourRecip;
		y = (m.values[8] - m.values[2]) * qrFourRecip;
		z = (m.values[1] - m.values[4]) * qrFourRecip;
	}
}

Quaternion::Quaternion(const Matrix3& m) {
	w = sqrt(max(0.0f, (1.0f + m.values[0] + m.values[4] + m.values[8]))) * 0.5f;

	float qrFour = 4.0f * w;
	float qrFourRecip = 1.0f / qrFour;

	x = (m.values[5] - m.values[7]) * qrFourRecip;
	y = (m.values[6] - m.values[2]) * qrFourRecip;
	z = (m.values[1] - m.values[3]) * qrFourRecip;
}

Quaternion::~Quaternion(void)
{
}

float Quaternion::Dot(const Quaternion& a, const Quaternion& b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

void Quaternion::Normalise() {
	float magnitude = sqrt(x * x + y * y + z * z + w * w);

	if (magnitude > 0.0f) {
		float t = 1.0f / magnitude;

		x *= t;
		y *= t;
		z *= t;
		w *= t;
	}
}

void Quaternion::CalculateW() {
	w = 1.0f - (x * x) - (y * y) - (z * z);
	if (w < 0.0f) {
		w = 0.0f;
	}
	else {
		w = -sqrt(w);
	}
}

Quaternion Quaternion::Conjugate() const
{
	return Quaternion(-x, -y, -z, w);
}

Quaternion Quaternion::Lerp(const Quaternion& from, const Quaternion& to, float by) {
	Quaternion temp = to;

	float dot = Quaternion::Dot(from, to);

	if (dot < 0.0f) {
		temp = -to;
	}

	return (from * (1.0f - by)) + (temp * by);
}

Quaternion Quaternion::Slerp(const Quaternion& from, const Quaternion& to, float by) {
	Quaternion temp = to;

	float dot = Quaternion::Dot(from, to);

	if (dot < 0.0f) {
		temp = -to;
	}

	return (from * (cos(by))) + (to * (1.0f - cos(by)));
}

//DEPRECTATED COMMENT
//http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
//Verified! Different values to above, due to difference between x/z being 'forward'
//DEPRECATED COMMENT END
//
//By the rotation order of Z, X, Y
Vector3 Quaternion::ToEuler(float eps) const {

	float xx2 = 2 * x * x; 
	float yy2 = 2 * y * y; 
	float zz2 = 2 * z * z; 
	float xy2 = 2 * x * y; 
	float wz2 = 2 * w * z; 
	float xz2 = 2 * x * z;
	float wy2 = 2 * w * y;
	float yz2 = 2 * y * z; 
	float wx2 = 2 * w * x; 

	float matrix9 = MyMaths::Clamp(yz2 - wx2, -1, 1);

	float thetaX = asin(-matrix9);
	float thetaY, thetaZ;

	if (-matrix9 > (1 - eps)) { //degreeX=90
		float matrix4 = xy2 - wz2;
		float matrix6 = yz2 + wx2;

		thetaY = atan2(matrix4, matrix6);
		thetaZ = 0;
	}
	else if (-matrix9 < (-1 + eps)) { //degreeX=-90
		float matrix4 = xy2 - wz2;
		float matrix6 = yz2 + wx2;

		thetaY = atan2(-matrix4, -matrix6);
		thetaZ = 0;
	}
	else {
		float matrix1 = xy2 + wz2;
		float matrix5 = 1 - xx2 - zz2;
		float matrix8 = xz2 + wy2;
		float matrix10 = 1 - xx2 - yy2;

		thetaY = atan2(matrix8, matrix10);
		thetaZ = atan2(matrix1, matrix5);
	}

	return Vector3(RadToDeg(thetaX), RadToDeg(thetaY), RadToDeg(thetaZ));
}

Vector4 Quaternion::ToAxisAndEuler() const
{
	float theta = acos(w) * 2.0f;
	float result = sqrt(1 - w * w);
	result = max(result, 1e-5f);
	Vector4 vec = Vector4(x / result, y / result, z / result, RadToDeg(theta));
	return vec;
}

Quaternion Quaternion::FromEulerAngles(const Vector3& rotationEuler)
{
	return Quaternion::FromEulerAngles(rotationEuler.x, rotationEuler.y, rotationEuler.z);
}

//DEPRECATED COMMENT
//http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/
//VERIFIED AS CORRECT - Pitch and roll are changed around as the above uses x as 'forward', whereas we use -z
//DEPRECATED COMMENT END
//
//By the rotation order of Z, X, Y
Quaternion Quaternion::FromEulerAngles(float degreeX, float degreeY, float degreeZ) {
	Quaternion q = Quaternion::FromAxisAngle(Vector3(0, 1, 0), degreeY) *
		(Quaternion::FromAxisAngle(Vector3(1, 0, 0), degreeX) * Quaternion::FromAxisAngle(Vector3(0, 0, 1), degreeZ));

	return q;
};

Quaternion Quaternion::FromAxisAngle(const Vector3& vector, float degrees) {
	float theta  = (float)DegToRad(degrees);
	float result = (float)sin(theta / 2.0f);

	return Quaternion((float)(vector.x * result), (float)(vector.y * result), (float)(vector.z * result), (float)cos(theta / 2.0f));
}


Vector3		Quaternion::operator *(const Vector3& a)	const {
	Quaternion newVec = *this * Quaternion(a.x, a.y, a.z, 0.0f) * Conjugate();
	return Vector3(newVec.x, newVec.y, newVec.z);
}