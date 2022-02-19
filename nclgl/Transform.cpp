#include "Transform.h"

Transform Transform::root = Transform(nullptr);

void Transform::SetForward(Vector3 vecTarg)
{
	throw NotImplementedException();

	float xRad = 0;
	float yRad = 0;
	bool isFound = false;

	float a = vecTarg.x;
	float b = vecTarg.y;
	float c = vecTarg.z;

	float xRad0 = asin(b);

	float yRad00 = acos(-c / cos(xRad0));
	float yRad01 = -yRad00;

	if (MyMaths::EpsEqual(-sin(yRad00) * cos(xRad0), a)) {
		xRad = xRad0;
		yRad = yRad00;
		isFound = true;
	}
	else if (MyMaths::EpsEqual(-sin(yRad01) * cos(xRad0), a))
	{
		xRad = xRad0;
		yRad = yRad01;
		isFound = true;
	}

	if (isFound) {
		SetRotationWorld(RadToDeg(xRad), RadToDeg(yRad), RadToDeg(rotationQuaternion.ToEuler().z));
	}
	else {
		auto d = -sin(yRad00) * cos(xRad0);
		auto e = -sin(yRad01) * cos(xRad0);
		throw NotImplementedException();
	}
}

//NOTE: Works on most occasions, only extreme small y rotation angle can lead to issues (due to float value calculation precision)..
void Transform::SetReflectionForMirror(Transform* transformRef, Vector3 normalPlane, Vector3 posPlane)
{
	//Set position
	Vector3 ref2Plane = posPlane - transformRef->GetPositionWorld();
	float len = Vector3::Dot(-ref2Plane, normalPlane);
	Vector3 plane2Self = -ref2Plane - normalPlane * len * 2;
	Vector3 posSelf = posPlane + plane2Self;

	SetPositionWorld(posSelf);

	//Set rotation
	Vector4 axisAngle = transformRef->GetRotationWorld().ToAxisAndEuler();
	Vector3 axis = axisAngle.ToVector3();
	float euler = axisAngle.w;

	float len0 = Vector3::Dot(axis, normalPlane);
	Vector3 axisSelf = axis - normalPlane * len0 * 2;

	
	Vector3 fowardRef = Vector3(0,0,-1);
	float cosInnerAngle = Vector3::Dot(fowardRef, -normalPlane);
	Vector3 rotAxis;
	if (!MyMaths::EpsEqual(cosInnerAngle,0)) {
		if (MyMaths::EpsEqual(abs(cosInnerAngle), 1))
			rotAxis = Vector3(0, 1, 0);
		else
			rotAxis = (fowardRef / cosInnerAngle + normalPlane).Normalised();
		SetRotationWorld(Quaternion::FromAxisAngle(axisSelf, -euler) * Quaternion::FromAxisAngle(rotAxis, 180));
	}
	else{
		SetRotationWorld(Quaternion::FromAxisAngle(axisSelf, -euler));
	}
}
