#include "CameraController.h"

void CameraController::UpdateCamera(float dt)
{
	if (!GetGO()) {
		return;
	}

	float pitch = GetGO()->transform->GetRotationEulerLocal().x;
	float yaw = GetGO()->transform->GetRotationEulerLocal().y;

	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);

	pitch = min(pitch, 90.0f);
	pitch = max(pitch, -90.0f);

	if (yaw < 0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}
	
	float roll = GetGO()->transform->GetRotationEulerLocal().z;

	GetGO()->transform->SetRotationLocal(pitch, yaw, roll);

	Transform* t = GetGO()->transform;

	Vector3 forward = GetGO()->transform->GetForward();
	Vector3 right = GetGO()->transform->GetRight();

	float speed = this->speed * dt;

	float speedScale = 1.0f;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
		speedScale = 3.0f;
	}
	speed *= speedScale;

	Vector3 position = GetGO()->transform->GetPositionLocal();

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		position += forward * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		position -= forward * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		position -= right * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		position += right * speed;
	}
	
	/*if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		position.y += speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_CONTROL)) {
		position.y -= speed;
	}*/
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_E)) {
		position.y += speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_Q)) {
		position.y -= speed;
	}

	GetGO()->transform->SetPositionLocal(position);
}