#pragma once


#include "Component.h"
#include "GameObject.h"


class CameraAutoController :
    public Component
{
    Vector3 cent = Vector3::zero;

	bool isMovingToALocation = false;
	Vector3 dst = Vector3(20, 20, 20);
	float time = 2;
	float tAccu = 0;
	Vector3 posFrom;

    virtual void Update(float dt) override;
};

