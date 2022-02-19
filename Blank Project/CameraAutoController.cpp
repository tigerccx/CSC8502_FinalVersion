#include "CameraAutoController.h"

void CameraAutoController::Update(float dt)
{
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W) ||
		Window::GetKeyboard()->KeyDown(KEYBOARD_A) || 
		Window::GetKeyboard()->KeyDown(KEYBOARD_S) || 
		Window::GetKeyboard()->KeyDown(KEYBOARD_D) || 
		Window::GetKeyboard()->KeyDown(KEYBOARD_Q) || 
		Window::GetKeyboard()->KeyDown(KEYBOARD_E) ) {
		Deactivate();
	}

	if (!isMovingToALocation) {
		float rotY = dt * 5.0f;
		GetGO()->transform->RotateAround(Vector3(0, rotY, 0), cent);
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
			isMovingToALocation = true;
			posFrom = GetGO()->transform->GetPositionWorld();
		}
	}
	else {
		tAccu += dt;
		if (tAccu < time) {
			Vector3 posCur = (dst-posFrom)* tAccu/ time + posFrom;
			GetGO()->transform->SetPositionWorld(posCur);
		}
		else
		{
			GetGO()->transform->SetPositionWorld(dst);
			isMovingToALocation = false;
		}
	}
}
