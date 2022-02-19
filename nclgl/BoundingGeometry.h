#pragma once

#include "Vector3.h"
#include "Mesh.h"
#include "Box.h"
#include "OrientedBox.h"
#include "Sphere.h"
#include "Plane.h"
#include "Utility.h"

enum BoundingGeometryType {
	BoundingGeometryType_None,
	BoundingGeometryType_Sphere,
	BoundingGeometryType_AABB,
	BoundingGeometryType_OBB,
	BoundingGeometryType_Mesh,
};

class BoundingGeometry
{
public:
	BoundingGeometry() : renderOrder(BoundingGeometryType_None) {}
	BoundingGeometry(BoundingGeometryType renderOrder) : renderOrder(renderOrder) {}
	BoundingGeometry(const BoundingGeometry& other) = default;
	BoundingGeometry& operator=(const BoundingGeometry& other) = default;
	virtual ~BoundingGeometry() = default;

	inline void SetType(BoundingGeometryType renderOrder) { this->renderOrder = renderOrder; }
	inline BoundingGeometryType GetType() const { return renderOrder; }

	void UpdateBoundingGeometryByMesh(shared_ptr<Mesh> mesh);
	void UpdateBoundingGeometryByVertices(Vector3* vertices, int count);

	inline void SetSphere(const Sphere& sphere) { this->sphere = sphere; }
	inline Sphere GetSphere() const { return sphere; }
	void UpdateSphere(shared_ptr<Mesh> mesh);
	void UpdateSphere(Vector3* vertices, int count);

	inline void SetAABB(const Box& aabb) { this->aabb = aabb; }
	inline Box GetAABB() const { return aabb; }
	void UpdateAABB(shared_ptr<Mesh> mesh);
	void UpdateAABB(Vector3* vertices, int count);

	inline void SetOBB(const OrientedBox& obb) { this->obb = obb; }
	inline OrientedBox GetOBB() const { return obb; }
	void UpdateOBB(shared_ptr<Mesh> mesh);
	void UpdateOBB(Vector3* vertices, int count);

	inline void SetBoundingMesh(shared_ptr<Mesh> boundingMesh) { this->boundingMesh = boundingMesh; }
	inline shared_ptr<Mesh> GetBoundingMesh() const { return boundingMesh; }
	void UpdateBoundingMesh(shared_ptr<Mesh> mesh);

protected:
	BoundingGeometryType renderOrder;
	shared_ptr<Mesh> boundingMesh;
	Box aabb;
	OrientedBox obb;
	Sphere sphere;
};

