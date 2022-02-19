#include "WaterAnimator.h"

void WaterAnimator::Update(float dt)
{
	waterCycle += dt * speed0;
	
	Matrix4 matTex = Matrix4::Translation(Vector3(waterCycle, 0.0f, 0.6*waterCycle));//* Matrix4::Scale(Vector3(10, 10, 10)) * Matrix4::Rotation(0.5, 0.5, waterRotate);
	mat->SetMatrix4("texMatrix", matTex);
	
	waterCycle1 += dt * speed1;
	waterRotate1 += dt * rotSpeed1;

	matTex = Matrix4::Translation(Vector3(waterCycle, 0.0f, -0.6*waterCycle)) * Matrix4::Rotation(0.5, 0.5, waterRotate1);;//* Matrix4::Scale(Vector3(10, 10, 10)) * Matrix4::Rotation(0.5, 0.5, waterRotate);
	mat->SetMatrix4("texMatrix1", matTex);
}

void WaterAnimator::PreRender()
{
	//mat->Set3FloatVector(NCLGL_SHADER_CAMERA_POSITION, cam->GetGO()->transform->GetPositionWorld());
	//cam->UpdatePositionToLightedShader(GetMat()->GetShader());
}
