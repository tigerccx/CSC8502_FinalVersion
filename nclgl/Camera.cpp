#include "Camera.h"

void Camera::Update(float dt) {

}

void Camera::SetCameraParameters(const CameraParamPerspective& cameraParam)
{
	this->cameraParam = cameraParam;
	UpdateProjMatrix();
}

//Note: Rotation order: ZXY
Matrix4 Camera::BuildViewMatrix()
{
	Transform* transform = GetGO()->transform;
	Vector3 rotation = transform->GetRotationEulerLocal();
	Vector3 scale = transform->GetScaleLocal();
	Matrix4 viewMatrix = (Matrix4::Scale(Vector3(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z)) *
		(Matrix4::Rotation(-(rotation.z), Vector3(0, 0, 1)) *
		(Matrix4::Rotation(-(rotation.x), Vector3(1, 0, 0)) *
		(Matrix4::Rotation(-(rotation.y), Vector3(0, 1, 0)) *
		Matrix4::Translation(-(transform->GetPositionLocal()))))));
	Transform* transParent = GetGO()->transform->GetParent();
	while (transParent) {
		Vector3 vecScale = transParent->GetScaleLocal();
		rotation = transParent->GetRotationEulerLocal();
		Matrix4 parentInvTransform = (Matrix4::Scale(Vector3(1.0f / vecScale.x, 1.0f / vecScale.y, 1.0f / vecScale.z)) *
			(Matrix4::Rotation(-(rotation.z), Vector3(0, 0, 1)) *
			(Matrix4::Rotation(-(rotation.x), Vector3(1, 0, 0)) *
			(Matrix4::Rotation(-(rotation.y), Vector3(0, 1, 0)) *
			Matrix4::Translation(-(transParent->GetPositionLocal()))))));
		viewMatrix = viewMatrix * parentInvTransform;
		transParent = transParent->GetParent();
	}

	return viewMatrix;
}

void Camera::UpdateMatrixToShader(shared_ptr<Shader> shader)
{
	if (shader) {
		auto uniformBlockIndex = glGetUniformBlockIndex(shader->GetProgram(), NCLGL_SHADER_INSTANCING_UB_CAMERA_DATA);
		if (uniformBlockIndex != GL_INVALID_INDEX) {
			glUniformBlockBinding(shader->GetProgram(), uniformBlockIndex, NCLGL_SHADER_INSTANCING_UB_CAMERA_DATA_BP);
		}
		else{ //Keep compatible with older shaders
			auto ulView = glGetUniformLocation(shader->GetProgram(), NCLGL_SHADER_VIEW_MATRIX);
			auto ulProj = glGetUniformLocation(shader->GetProgram(), NCLGL_SHADER_PROJECTION_MATRIX);
			if (ulView > 0 && ulProj > 0) {
				glUniformMatrix4fv(ulView, 1, false, BuildViewMatrix().values);
				glUniformMatrix4fv(ulProj, 1, false, projMatrix.values);
			}
		}
	}
}

void Camera::UpdatePositionToLightedShader(shared_ptr<Shader> shader)
{
	if (shader) {
		auto go = GetGO();
		if (!go)
			throw NullPtrException();
		glUniform3fv(glGetUniformLocation(shader->GetProgram(), NCLGL_SHADER_CAMERA_POSITION), 
			1, (float*)&(go->transform->GetPositionLocal()));
	}
}

void Camera::SetCameraRenderTarget(int width, int height)
{
	renderTarget = make_shared<RenderTargetTexture>(width, height, true, true, true, true);
}

void Camera::UpdateProjMatrix()
{
	projMatrix = Matrix4::Perspective(cameraParam.nearPlane, 
		cameraParam.farPlane, 
		cameraParam.aspectWH, 
		cameraParam.fov);
}
