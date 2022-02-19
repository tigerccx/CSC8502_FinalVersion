#include "GameObjectManager.h"

bool GameObjectManager::DeleteGameObject(GameObject* go)
{
	auto itr = find(goList.begin(), goList.end(), go);
	if (itr == goList.end()) {
		return false;
	}
	else
	{
		vector<Transform*> children;
		go->GetChildren(children);
		for (int i = 0; i < children.size(); ++i) {
			DeleteGameObject(children[i]->GetGO());
		}
		go->transform->SetParent(nullptr);

		delete *itr;
		itr = goList.erase(itr);
		return true;
	}
}

GameObjectManager::~GameObjectManager()
{
	while (!goList.empty()) {
		GameObject* go = goList[goList.size() - 1];
		if (go)
			delete go;
		goList.pop_back();
	}
}

void GameObjectManager::Update(float dt)
{
	for (auto& itr : goList) {
		if(itr)
			itr->Update(dt);
	}
}

void GameObjectManager::FixedUpdate(float fdt)
{
	for (auto& itr : goList) {
		if (itr)
			itr->FixedUpdate(fdt);
	}
}

void GameObjectManager::PreRender()
{
	for (auto& itr : goList) {
		if (itr)
			itr->PreRender();
	}
}

void GameObjectManager::PrePerCameraDraw(Camera* camera)
{
	for (auto& itr : goList) {
		if (itr)
			itr->PrePerCameraDraw(camera);
	}
}

void GameObjectManager::PostPerCameraDraw(Camera* camera)
{
	for (auto& itr : goList) {
		if (itr)
			itr->PostPerCameraDraw(camera);
	}
}

void GameObjectManager::PostRender()
{
	for (auto& itr : goList) {
		if (itr)
			itr->PostRender();
	}
}
