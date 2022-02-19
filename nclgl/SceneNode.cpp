#include "SceneNode.h"

SceneNode::SceneNode(shared_ptr<Mesh> m, Vector4 colour)
{
	this->mesh = m;
	this->colour = colour;
	parent = nullptr;
	modelScale = Vector3::one;
}

SceneNode::~SceneNode(void) {
	for (unsigned int i = 0; i < children.size(); ++i) {
		if (children[i]) {
			delete children[i];
			children[i] = nullptr;
		}
	}
}

bool SceneNode::AddChild(SceneNode* s) {
	if (s == this) {
		return false;
	}

	children.push_back(s);
	s->parent = this;
	return true;
}

void SceneNode::RemoveChildren()
{
	for (int i = children.size()-1; i >=0 ; --i) {
		if (children[i]) {
			children[i]->RemoveChildren();
		}
		delete children[i];
		children.pop_back();
	}
}

void SceneNode::Update(float dt)
{
	if (parent) {
		worldTransform = parent->worldTransform * transform;
	}
	else
	{
		worldTransform = transform;
	}
	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->Update(dt);
	}
}

void SceneNode::Draw(const OGLRenderer& r) {
	if (mesh) { mesh->Draw(); }
}
