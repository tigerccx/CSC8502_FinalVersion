#pragma once

#include <vector>
#include <memory>
#include <typeinfo>
#include <string>

#include "Component.h"
#include "ComponentManaged.h"
#include "Transform.h"

using namespace std;

class Camera;

class GameObject
{
public:
	GameObject() { this->transform = new Transform(); AddComponentAndActivate(this->transform);};
	GameObject(GameObject* parent) { this->transform = new Transform(); AddComponentAndActivate(this->transform); this->transform->SetParent(parent->transform); };
	GameObject(Transform* parent) { this->transform = new Transform(); AddComponentAndActivate(this->transform); this->transform->SetParent(parent); };

	virtual ~GameObject();

	Transform* transform;
	string name = "GameObject";

	vector<Component*> componentList;
	vector<Component*> componentManangedList;

	//inline unsigned int GetID() { return (unsigned int)this; } //TODO: This is not safe! Find a new way. 
	inline void AddComponent(Component* component) { 
		component->SetGO(this);
		componentList.push_back(component); 
	};
	inline void AddComponent(ComponentManaged* component) {
		component->SetGO(this);
		componentManangedList.push_back(component);
	};
	inline void AddComponentAndActivate(Component* component) {
		AddComponent(component);
		componentList[componentList.size() - 1]->Activate();
	}
	void AddComponentAndActivate(ComponentManaged* component) {
		AddComponent(component);
		componentManangedList[componentManangedList.size() - 1]->Activate();
	}

	template<class T> T* RemoveComponent();
	template<class T> T* RemoveComponent(T* component);

	template<class T> T* GetComponent();
	template<class T> bool GetComponents(vector<T*>& components);

	inline bool AddChild(GameObject* goChild) {
		return transform->AddChild(goChild->transform);
	}

	inline void ClearChildren() {
		transform->ClearChildren();
	}

	Transform* GetChild(int idx) {
		return transform->GetChild(idx);
	}

	inline void GetChildren(vector<Transform*>& childrenOut) {
		transform->GetChildren(childrenOut);
	}

	inline bool RemoveChild(GameObject* goChild) {
		return transform->RemoveChild(goChild->transform);
	}


	virtual void Update(float dt);
	virtual void FixedUpdate(float fdt);
	virtual void PreRender();
	virtual void PrePerCameraDraw(Camera* camera);
	virtual void PostPerCameraDraw(Camera* camera);
	virtual void PostRender();


private:
	GameObject(const GameObject& other) = delete;
	GameObject(GameObject&& other) = delete;
	GameObject& operator=(const GameObject& other) = delete;
	GameObject& operator=(GameObject&& other) = delete;
};

template<class T>
inline T* GameObject::RemoveComponent()
{
	size_t idTypeGet = typeid(T).hash_code();
	for (auto itr = componentList.begin(); itr != componentList.end(); ++itr) {
		if ((*itr)->GetTypeID() == idTypeGet) {
			T* ret = (T*)(*itr);
			(*itr)->gameobject = nullptr;
			itr = componentList.erase(itr);
			return ret;
		}
	}
	for (auto itr = componentManangedList.begin(); itr != componentManangedList.end(); ++itr) {
		if ((*itr)->GetTypeID() == idTypeGet) {
			T* ret = (T*)(*itr);
			(*itr)->gameobject = nullptr;
			itr = componentManangedList.erase(itr);
			return ret;
		}
	}
	return nullptr;
}

template<class T>
inline T* GameObject::RemoveComponent(T* component)
{
	size_t idTypeGet = typeid(T).hash_code();
	for (auto itr = componentList.begin(); itr != componentList.end(); ++itr) {
		if ((*itr)->GetTypeID() == idTypeGet) {
			T* ret = (T*)(*itr);
			if (ret != component)
				continue;
			(*itr)->gameobject = nullptr;
			itr = componentList.erase(itr);
			return ret;
		}
	}
	for (auto itr = componentManangedList.begin(); itr != componentManangedList.end(); ++itr) {
		if ((*itr)->GetTypeID() == idTypeGet) {
			T* ret = (T*)(*itr);
			if (ret != component)
				continue;
			(*itr)->gameobject = nullptr;
			itr = componentManangedList.erase(itr);
			return ret;
		}
	}
	return nullptr;
}

template<class T>
inline T* GameObject::GetComponent()
{
	size_t idTypeGet = typeid(T).hash_code();
	for (Component* itr : componentList) {
		if (itr->GetTypeID() == idTypeGet) {
			return (T*)itr;
		}
	}
	for (Component* itr : componentManangedList) {
		if (itr->GetTypeID() == idTypeGet) {
			return (T*)itr;
		}
	}
	return nullptr;
}

template<class T>
inline bool GameObject::GetComponents(vector<T*>& componentsOut)
{
	size_t size0 = componentsOut.size();
	size_t idTypeGet = typeid(T).hash_code();
	for (Component* itr : componentList) {
		if (itr->GetTypeID() == idTypeGet) {
			componentsOut.emplace_back((T*)itr);
		}
	}
	for (Component* itr : componentManangedList) {
		if (itr->GetTypeID() == idTypeGet) {
			componentsOut.emplace_back((T*)itr);
		}
	}
	return componentsOut.size() != size0;
}
