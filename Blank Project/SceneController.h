#pragma once

#include "Component.h"
#include "GameObject.h"

class SceneController :
    public Component
{
public:
    float sceneTime = 0;
    vector<GameObject*> sceneGOs;

    virtual void Update(float dt) override;
};

