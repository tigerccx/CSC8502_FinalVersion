#include "GameObject.h"

GameObject::~GameObject()
{
	//Clear unmanaged components;
	for (Component* itr : componentList) {
		if(itr)
			itr->Deactivate();
			delete itr;
	}
}

void GameObject::Update(float dt)
{
	for (Component* itr : componentList) {
		if(itr->IsActive())
			itr->Update(dt);
	}
	for (Component* itr : componentManangedList) {
		if (itr->IsActive())
			itr->Update(dt);
	}
}

void GameObject::FixedUpdate(float fdt)
{
	for (Component* itr : componentList) {
		if (itr->IsActive())
			itr->FixedUpdate(fdt);
	}
	for (Component* itr : componentManangedList) {
		if (itr->IsActive())
			itr->FixedUpdate(fdt);
	}
}

void GameObject::PreRender()
{
	for (Component* itr : componentList) {
		if (itr->IsActive())
			itr->PreRender();
	}
	for (Component* itr : componentManangedList) {
		if (itr->IsActive())
			itr->PreRender();
	}
}

void GameObject::PrePerCameraDraw(Camera* camera)
{
	for (Component* itr : componentList) {
		if (itr->IsActive())
			itr->PrePerCameraDraw(camera);
	}
	for (Component* itr : componentManangedList) {
		if (itr->IsActive())
			itr->PrePerCameraDraw(camera);
	}
}

void GameObject::PostPerCameraDraw(Camera* camera)
{
	for (Component* itr : componentList) {
		if (itr->IsActive())
			itr->PostPerCameraDraw(camera);
	}
	for (Component* itr : componentManangedList) {
		if (itr->IsActive())
			itr->PostPerCameraDraw(camera);
	}
}

void GameObject::PostRender()
{
	for (Component* itr : componentList) {
		if (itr->IsActive())
			itr->PostRender();
	}
	for (Component* itr : componentManangedList) {
		if (itr->IsActive())
			itr->PostRender();
	}
}
