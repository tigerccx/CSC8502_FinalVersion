#include "SceneController.h"

void SceneController::Update(float dt) 
{
	sceneTime += dt;

	for (int i = 1; i < min(4, sceneGOs.size()); ++i) {
		if (sceneGOs[i]) {
			sceneGOs[i]->transform->SetPositionWorld(Vector3(-10 + (5 * i), 2.0f + sin(sceneTime * i), 0));
			sceneGOs[i]->transform->SetRotationWorld(sceneTime * 10 * i, 0, 0);
		}
	}
}
