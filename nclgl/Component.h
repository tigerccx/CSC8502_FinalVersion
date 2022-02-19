#pragma once

#include <typeinfo>
class Transform;
class GameObject;
class Camera;

class Component
{
public:
	Component() { Init(); } //TODO: determine the pipeline order
	Component(const Component& other) { Init(); }
	Component& operator=(const Component& other) { Init(); }
	virtual ~Component() { OnTerminated(); }

	virtual void Start() {}; //When starting to function
	virtual void Update(float dt) {};   //Called each frame
	virtual void FixedUpdate(float fdt) {};		//TODO: Implement fixed update
	virtual void PreRender() {};	//Called each time just before rendering stage
	virtual void PrePerCameraDraw(Camera* camera) {}; //Called once a camera each time just before per camera rendering
	virtual void PostPerCameraDraw(Camera* camera) {}; //Called once a camera each time just after per camera rendering
	virtual void PostRender() {}; //Called each time just after rendering stage

	virtual void OnDeactivated() {};
	virtual void OnActivated() {}; //When activated (attached to GO or activated manually)
	virtual void OnTerminated() {};

	inline void Activate() { if (!isActive) { isActive = true; OnActivated(); } }
	inline void Deactivate() { if (isActive) { isActive = false; OnDeactivated(); } }
	inline bool IsActive() const { return isActive; }
	inline bool IsManaged() const { return isManaged; }

	inline GameObject* GetGO() const { return gameobject; }
	friend class GameObject;

	virtual const size_t GetTypeID() const { return typeid(*this).hash_code(); }

protected:
	GameObject* gameobject;
	bool isActive = false;
	bool isStarted = false;
	bool isManaged = false;

	virtual void Init() { gameobject = nullptr; }

	inline void SetGO(GameObject* go) { gameobject = go; }
};

