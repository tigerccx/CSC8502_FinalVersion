#include "Frustum.h"
#include "Camera.h"

void CameraFrustum::UpdateFrustum(Matrix4 modelMatrix,
								float fov, float aspectWH, float nearPlane, float farPlane)
{
	Vector3 normalInN = Vector3(0, 0, 1);
	Vector3 normalInF = Vector3(0, 0, -1);
	
	float h = tan(DegToRad(fov / 2.0f));
	Vector3 normalInB = (Vector3(0, -1, 0) + Vector3(0, 0, 1)* h).Normalised();
	Vector3 normalInT = (Vector3(0, 1, 0) + Vector3(0, 0, 1)* h).Normalised();

	float w = h * aspectWH;
	Vector3 normalInL = (Vector3(-1, 0, 0) + Vector3(0, 0, 1) * w).Normalised();
	Vector3 normalInR = (Vector3(1, 0, 0) + Vector3(0, 0, 1) * w).Normalised();

	normalInN = (modelMatrix * Vector4(normalInN, 0)).ToVector3().Normalised();
	normalInF = (modelMatrix * Vector4(normalInF, 0)).ToVector3().Normalised();
	normalInL = (modelMatrix * Vector4(normalInL, 0)).ToVector3().Normalised();
	normalInR = (modelMatrix * Vector4(normalInR, 0)).ToVector3().Normalised();
	normalInB = (modelMatrix * Vector4(normalInB, 0)).ToVector3().Normalised();
	normalInT = (modelMatrix * Vector4(normalInT, 0)).ToVector3().Normalised();

	Vector3 camOrg = (modelMatrix * Vector4(0, 0, 0, 1)).ToVector3();
	Vector3 nearCent = (modelMatrix * Vector4(0, 0, -nearPlane, 1)).ToVector3();
	Vector3 farCent = (modelMatrix * Vector4(0, 0, -farPlane, 1)).ToVector3();

	float disN = -Vector3::Dot(nearCent, normalInN);
	float disF = -Vector3::Dot(farCent, normalInF);
	float disL = -Vector3::Dot(camOrg, normalInL);
	float disR = -Vector3::Dot(camOrg, normalInR);
	float disB = -Vector3::Dot(camOrg, normalInB);
	float disT = -Vector3::Dot(camOrg, normalInT);

	frustumPlanes[0] = Plane(normalInL, disL, true); //L
	frustumPlanes[1] = Plane(normalInR, disR, true); //R
	frustumPlanes[2] = Plane(normalInB, disB, true); //B
	frustumPlanes[3] = Plane(normalInT, disT, true); //T
	frustumPlanes[4] = Plane(normalInN, disN, true); //N
	frustumPlanes[5] = Plane(normalInF, disF, true); //F
}


void CameraFrustum::UpdateOrthographicFrustum(Matrix4 modelMatrixCent,
	float zNear, float zFar, float r, float l, float t, float b) {

	Vector3 normalInN = Vector3(0, 0, 1);
	Vector3 normalInF = Vector3(0, 0, -1);
	Vector3 normalInB = Vector3(0, -1, 0);
	Vector3 normalInT = Vector3(0, 1, 0);
	Vector3 normalInL = Vector3(-1, 0, 0);
	Vector3 normalInR = Vector3(1, 0, 0);

	normalInN = (modelMatrixCent * Vector4(normalInN, 0)).ToVector3().Normalised();
	normalInF = (modelMatrixCent * Vector4(normalInF, 0)).ToVector3().Normalised();
	normalInL = (modelMatrixCent * Vector4(normalInL, 0)).ToVector3().Normalised();
	normalInR = (modelMatrixCent * Vector4(normalInR, 0)).ToVector3().Normalised();
	normalInB = (modelMatrixCent * Vector4(normalInB, 0)).ToVector3().Normalised();
	normalInT = (modelMatrixCent * Vector4(normalInT, 0)).ToVector3().Normalised();

	Vector3 lCent = (modelMatrixCent * Vector4(l, 0, 0, 1)).ToVector3();
	Vector3 rCent = (modelMatrixCent * Vector4(r, 0, 0, 1)).ToVector3();
	Vector3 tCent = (modelMatrixCent * Vector4(0, t, 0, 1)).ToVector3();
	Vector3 bCent = (modelMatrixCent * Vector4(0, b, 0, 1)).ToVector3();
	Vector3 nearCent = (modelMatrixCent * Vector4(0, 0, zNear, 1)).ToVector3();
	Vector3 farCent = (modelMatrixCent * Vector4(0, 0, zFar, 1)).ToVector3();

	float disN = Vector3::Dot(nearCent, normalInN);
	float disF = Vector3::Dot(farCent, normalInF);
	float disL = Vector3::Dot(lCent, normalInL);
	float disR = Vector3::Dot(rCent, normalInR);
	float disB = Vector3::Dot(bCent, normalInB);
	float disT = Vector3::Dot(tCent, normalInT);

	frustumPlanes[0] = Plane(normalInL, disL, true); //L
	frustumPlanes[1] = Plane(normalInR, disR, true); //R
	frustumPlanes[2] = Plane(normalInB, disB, true); //B
	frustumPlanes[3] = Plane(normalInT, disT, true); //T
	frustumPlanes[4] = Plane(normalInN, disN, true); //N
	frustumPlanes[5] = Plane(normalInF, disF, true); //F
}