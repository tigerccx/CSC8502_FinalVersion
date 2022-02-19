#include "MirrorCameraController.h"

void MirrorCameraController::Update(float dt)
{
	auto up = GetGO()->transform->GetUp();
	transMirrorCam->SetReflectionForMirror(transRefCam, GetGO()->transform->GetUp(), GetGO()->transform->GetPositionWorld());
	auto a = transMirrorCam->GetPositionWorld();
	
	auto c = transRefCam->GetRotationWorld().ToEuler();
	auto b = transMirrorCam->GetRotationWorld().ToEuler();
}

void MirrorCameraController::PrePerCameraDraw(Camera* camera)
{
	if (camera == mirrorCam) {
		rendererSelf->Deactivate();
	}
	else {
		rendererSelf->Activate();
		matMirror->SetMatrix4("viewMatrixMirrored", mirrorCam->BuildViewMatrix());
	}
}
