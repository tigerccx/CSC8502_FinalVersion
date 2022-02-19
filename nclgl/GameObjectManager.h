#pragma once

#include <vector>
#include <algorithm>

#include "Singleton.h"
#include "GameObject.h"

using namespace std;

class GameObjectManager: public Singleton<GameObjectManager>
{
public:
	GameObjectManager() = default;
	GameObjectManager(const GameObjectManager& other) = default;
	GameObjectManager& operator=(const GameObjectManager& other) = default;
	virtual ~GameObjectManager();

	inline GameObject* CreateGameObject() { GameObject* goNew = new GameObject(); goList.emplace_back(goNew); return goNew; }
	bool DeleteGameObject(GameObject* go);

	void Update(float dt);
	void FixedUpdate(float fdt);
	void PreRender();
	void PrePerCameraDraw(Camera* camera);
	void PostPerCameraDraw(Camera* camera);
	void PostRender();

protected:
	vector<GameObject*> goList;
};

