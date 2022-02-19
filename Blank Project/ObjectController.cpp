#include "ObjectController.h"

void ObjectController::Update(float dt)
{
	auto transInControl = GetGO()->transform;

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
	float rotX = 0;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_COMMA)) {
		rotY = +dt * rotSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_PERIOD)) {
		rotY = -dt * rotSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_L)) {
		rotX = dt * rotSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_K)) {
		rotX = -dt * rotSpeed;
	}

	Vector3 offset = transInControl->GetRight() * offsetX + transInControl->GetForward() * offsetZ + Vector3(0, offsetY, 0);
	transInControl->Translate(offset);
	transInControl->Rotate(rotX, rotY, 0);
}
