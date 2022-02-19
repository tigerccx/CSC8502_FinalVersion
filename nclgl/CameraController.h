#pragma once

#include "Window.h"
#include <algorithm>

#include "Component.h"
#include "GameObject.h"

using namespace std;

class CameraController: public Component
{
public:
	CameraController() : Component() {
		speed = 5.0f;
	}

	CameraController(float speed, float speedRow) : Component() {
		this->speed = speed;
		//this->speedRow = speedRow;
	}

	CameraController(float speed) : Component() {
		this->speed = speed;
	}

	virtual void Update(float dt) { UpdateCamera(dt); }
	void UpdateCamera(float dt = 1.0f);

	float GetSpeed()const { return speed; }
	void SetSpeed(float s) { speed = s; }

protected:
	float speed;
};

