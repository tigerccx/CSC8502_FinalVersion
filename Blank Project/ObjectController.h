#pragma once
#include "Component.h"
#include "GameObject.h"
class ObjectController :
    public Component
{
    void Update(float dt) override;
};

