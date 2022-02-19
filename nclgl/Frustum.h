#pragma once
#include <array>

#include "Plane.h"

enum CameraFrustumPlane {
	CameraFrustumPlane_L = 0,
	CameraFrustumPlane_R,
	CameraFrustumPlane_B,
	CameraFrustumPlane_T,
	CameraFrustumPlane_N,
	CameraFrustumPlane_F
};

class CameraFrustum
{
public:
	CameraFrustum(Matrix4 modelMatrix,
		float fov, float aspectWH, float nearPlane, float farPlane) {
		UpdateFrustum(modelMatrix, fov, aspectWH, nearPlane, farPlane);
	};
	CameraFrustum(Matrix4 modelMatrixCent,
		float zNear, float zFar, float l, float r, float t, float b) {
		UpdateOrthographicFrustum(modelMatrixCent, zNear, zFar, l, r, t, b);
	};
	virtual ~CameraFrustum() = default;
	
	void UpdateFrustum(Matrix4 modelMatrix,
		float fov, float aspectWH, float nearPlane, float farPlane);
	void UpdateOrthographicFrustum(Matrix4 modelMatrixCent,
		float zNear, float zFar, float r, float l, float t, float b);

	bool IsSphereInsideFrustum(const Sphere& sphere);
	inline Plane GetFrustumPlane(CameraFrustumPlane cfp) { return frustumPlanes[(int)cfp]; }

	array<Plane, 6> frustumPlanes; // frustum will return in the order of: l,r,b,t,n,f
};

