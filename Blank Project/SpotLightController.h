#pragma once
#include "Component.h"
#include "GameObject.h"

class SpotLightController :
    public Component
{
    virtual void Update(float dt) override {
		float speed = 5.0f;
		float offsetX = 0;
		float offsetY = 0;
		float offsetZ = 0;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT)) {
			offsetX = -dt * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) {
			offsetX = dt * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP)) {
			offsetZ = dt * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN)) {
			offsetZ = -dt * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_N)) {
			offsetY = -dt * speed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_M)) {
			offsetY = dt * speed;
		}
		float rotSpeed = 30.0f;
		float rotY = 0;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_COMMA)) {
			rotY = -dt * rotSpeed;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_PERIOD)) {
			rotY = +dt * rotSpeed;
		}

		auto transform = GetGO()->transform;
		Vector3 offset = transform->GetRight() * offsetX + transform->GetForward() * offsetZ + Vector3(0, offsetY, 0);
		transform->Translate(offset);
		transform->Rotate(0, rotY, 0);

		auto rotation = transform->GetRotationEulerWorld();
    }
};

