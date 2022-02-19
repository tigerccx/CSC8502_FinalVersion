#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>

class SceneNode
{
public:
	SceneNode(shared_ptr<Mesh> m = nullptr, Vector4 colour = Vector4::one);
	virtual ~SceneNode(void);

	void SetTransform(const Matrix4& matrix) { transform = matrix; }
	const Matrix4& GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }

	Vector4 GetColour() const { return colour; }
	void SetColout(Vector4 c) { colour = c; }

	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }

	shared_ptr<Mesh> GetMesh() const { return mesh; }
	void SetMesh(shared_ptr<Mesh> m) { mesh = m; }

	bool AddChild(SceneNode* s);
	void RemoveChildren();
	virtual void Update(float dt);
	virtual void Draw(const OGLRenderer& r);

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() { return children.begin(); }
	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() { return children.end(); }

protected:
	SceneNode* parent = nullptr;
	shared_ptr<Mesh> mesh = nullptr;
	Matrix4 worldTransform = Matrix4::identity;
	Matrix4 transform = Matrix4::identity;
	Vector3 modelScale = Vector3::one;
	Vector4 colour = Vector4::one;
	std::vector<SceneNode*> children;
};

