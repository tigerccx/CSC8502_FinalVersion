#include "BoundingGeometry.h"

void BoundingGeometry::UpdateBoundingGeometryByMesh(shared_ptr<Mesh> mesh)
{
	switch (renderOrder)
	{
	case BoundingGeometryType_Sphere: {
		UpdateSphere(mesh);
	}
		break;
	case BoundingGeometryType_AABB: {
		UpdateAABB(mesh);
	}
		break;
	case BoundingGeometryType_OBB: {
		UpdateOBB(mesh);
	}
		break;
	case BoundingGeometryType_Mesh: {
		UpdateBoundingMesh(mesh);
	}
		break;
	default:
		break;
	}
}

void BoundingGeometry::UpdateBoundingGeometryByVertices(Vector3* vertices, int count)
{
	switch (renderOrder)
	{
	case BoundingGeometryType_Sphere: {
		UpdateSphere(vertices, count);
	}
	break;
	case BoundingGeometryType_AABB: {
		UpdateAABB(vertices, count);
	}
	break;
	case BoundingGeometryType_OBB: {
		UpdateOBB(vertices, count);
	}
	break;
	case BoundingGeometryType_Mesh: 
		break;
	default:
		break;
	}
}

void BoundingGeometry::UpdateSphere(shared_ptr<Mesh> mesh)
{
	if (mesh) {
		UpdateSphere(mesh->GetVertices(), mesh->GetNumVertices());
	}
	else
	{
		Sphere sphere(Vector3::zero, 0);
		SetSphere(sphere);
	}
}

void BoundingGeometry::UpdateSphere(Vector3* vertices, int count)
{
	bool failed = false;
		
	Vector3 cent = Vector3::zero;
	if (count == 0) {
		failed = true;
	}
	else {
		for (int i = 0; i < count; ++i) {
			cent += vertices[i];
		}
		cent = cent / count;
		float maxSqrDis = 0;
		for (int i = 0; i < count; ++i) {
			float curSqrDis = (vertices[i] - cent).SqrLength();
			if (curSqrDis > maxSqrDis) {
				maxSqrDis = curSqrDis;
			}
		}
		Sphere sphere(cent, sqrt(maxSqrDis));
		SetSphere(sphere);
	}
	if (failed) {
		Sphere sphere(Vector3::zero, 0);
		SetSphere(sphere);
	}
}

void BoundingGeometry::UpdateAABB(shared_ptr<Mesh> mesh)
{
	if (mesh) {
		UpdateAABB(mesh->GetVertices(), mesh->GetNumVertices());
	}
	else
	{
		Box box;
		SetAABB(box);
	}
}

void BoundingGeometry::UpdateAABB(Vector3* vertices, int count)
{
	bool failed = false;
	
	Vector3 vMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3 vMax = Vector3(FLT_MIN, FLT_MIN, FLT_MIN);
	if (count == 0) {
		failed = true;
	}
	else {
		for (int i = 0; i < count; ++i) {
			vMin = Vector3::Min(vertices[i], vMin);
			vMax = Vector3::Max(vertices[i], vMax);
		}
		Box box(vMin, vMax);
		SetAABB(box);
	}

	if (failed)
	{
		Box box;
		SetAABB(box);
	}
}

void BoundingGeometry::UpdateOBB(shared_ptr<Mesh> mesh)
{
	//TODO: PCA vertices and calculate the OBB
	throw NotImplementedException();
}

void BoundingGeometry::UpdateOBB(Vector3* vertices, int count)
{
	throw NotImplementedException();
}

void BoundingGeometry::UpdateBoundingMesh(shared_ptr<Mesh> mesh)
{
	boundingMesh = mesh;
}
