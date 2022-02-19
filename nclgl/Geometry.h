#pragma once

#include <array>

#include "Mesh.h"
#include "Utility.h"
#include "MyMaths.h"

#include "Vector3.h"
#include "Sphere.h"
#include "Plane.h"
#include "Box.h"
#include "Frustum.h"

#include "Transform.h"
#include "BoundingGeometry.h"

class Geometry {
public:
	//
	//Geometry calculation
	//
	inline static bool IsSphereInPlane(const Sphere& sphere, const Plane& plane) {
		float f = Vector3::Dot(sphere.center, plane.normal);
		if (Vector3::Dot(sphere.center, plane.normal) + plane.distance - sphere.radius >= 0) //NOTE: reverse the definition of InPlane (as in now the normal is facing outwards)
			return false;
		return true;
	}

	static bool IsSphereInsideCameraFrustum(const Sphere& sphere, const CameraFrustum& frustum)
	{
		for (int p = 0; p < 6; ++p) {
			if (!IsSphereInPlane(sphere, frustum.frustumPlanes[p])) {
				return false; // scenenode is outside this plane !
			}
		}
		return true;
	}
	
	inline static bool IsVertexInPlane(const Vector3& vertex, const Plane& plane) {
		//auto a = Vector3::Dot(vertex, plane.normal) + plane.distance;
		if (Vector3::Dot(vertex, plane.normal) + plane.distance >= 0) //NOTE: reverse the definition of InPlane (as in now the normal is facing outwards)
			return false;
		return true;
	}

	inline static bool IsVertexInCameraFrustum(const Vector3& vertex, const CameraFrustum& frustum) {
		for (int p = 0; p < 6; ++p) {
			if (!IsVertexInPlane(vertex, frustum.frustumPlanes[p])) {
				return false; // scenenode is outside this plane !
			}
		}
		return true;
	}

	inline static bool IsBoxInsideCameraFrustum(const Box& box, const CameraFrustum& frustum)
	{
		array<Vector3, 8> vertices;
		box.GetVertices(vertices);
		return IsBoxVerticesInsideCameraFrustum(vertices, frustum);
	}

	static bool IsBoxVerticesInsideCameraFrustum(const array<Vector3, 8>& boxVertices, const CameraFrustum& frustum)
	{
		array<int, 6> vertexAllOut = {0};
		for (int v = 0; v < 8; ++v) {
			Vector3 vertex = boxVertices[v];
			bool isInAllPlane = true;
			for (int p = 0; p < 6; ++p) {
				if (!IsVertexInPlane(vertex, frustum.frustumPlanes[p])) {
					vertexAllOut[p]+=1;
					isInAllPlane = false;
				}
			}
			if (isInAllPlane) {
				return true;
			}
		}
		for (int p = 0; p < 6; ++p) {
			if (vertexAllOut[p] == 8) {
				return false;
			}
		}
		return true;
	}

	static bool IsTransformIntersectingCameraViewFrustum(Transform* transform, const CameraFrustum& frustum)
	{
		const BoundingGeometry boundingGeometry = transform->GetBoundingAABB();
		if (boundingGeometry.GetType() == BoundingGeometryType_None) {
			return false;
		}
		if (boundingGeometry.GetType() == BoundingGeometryType_AABB) {
			array<Vector3, 8> vertices;
			Box boxModified = Box::ModifyByModelMatrix(boundingGeometry.GetAABB(),transform->GetModelMatrix());
			return Geometry::IsBoxInsideCameraFrustum(boxModified, frustum);
		}
		return false;
	}


	//
	//Geometry Creation
	//
	static shared_ptr<Mesh> GenerateTriangle();
	static shared_ptr<Mesh> GenerateQuad(bool generateNormalTangent=false);
	static shared_ptr<Mesh> GeneratePlane(bool generateNormalTangent = false);
	static shared_ptr<Mesh> GenerateCube();
	static shared_ptr<Mesh> GenerateGrid(float d, int size);
	static shared_ptr<Mesh> GenerateXYZ();
	static shared_ptr<Mesh> GenerateQuad1();
	static shared_ptr<Mesh> GenerateTest();

	static void GenerateRandomQuad1ColorArray(std::vector<Vector4>& colour, int size);
};