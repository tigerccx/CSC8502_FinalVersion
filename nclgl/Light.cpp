#include "Light.h"

shared_ptr<Shader> Light::shadowShader = nullptr;
shared_ptr<Shader> Light::shadowShaderInstanced = nullptr;
shared_ptr<Shader> Light::shadowShaderPointLight = nullptr;
shared_ptr<Shader> Light::shadowShaderPointLightInstanced = nullptr;

Light::Light()
{
	lightType = LightType_Directional;
}

void Light::Update(float dt) {
	if(toCastShadow)
		UpdateViewMatrices();
}

Vector4 Light::GetLightParameterPackedVector() const
{
	Vector4 param = Vector4::zero;
	switch (lightType)
	{
	case LightType_Directional: {
		//x: dirX_norm
		//y: dirY_norm
		//z: dirZ_norm
		//w: none
		auto go = GetGO();
		if (!go) {
			throw NullPtrException();
		}
		Vector3 dir = go->transform->GetForward();
		param.x = dir.x;
		param.y = dir.y;
		param.z = dir.z;
	}
		break;
	case LightType_Point: {
		//x: radius
		//y: none
		//z: none
		//w: none
		param.x = pointLightParam.radius;
	}
		break;
	case LightType_Spot: {
		//x: dirX_norm
		//y: dirY_norm
		//z: radius
		//w: cosHalfAngle
		auto go = GetGO();
		if (!go) {
			throw NullPtrException();
		}
		Vector3 dir = go->transform->GetForward();
		param.x = dir.x;
		param.y = dir.y;
		param.z = MyMaths::SignNoZero(dir.z) * spotLightParam.radius;
		param.w = cos(DegToRad(spotLightParam.angle/2.0f));
	}
		break;
	default: {
		throw NotImplementedException();
	}
		break;
	}
	return param;
}

void Light::SetUpProjectionMatrices()
{
	switch (lightType)
	{
	case LightType_Directional: {
		projectionMatrices[0] = Matrix4::Orthographic(DIRECTIONAL_NEAR, DIRECTIONAL_FAR, DIRECTIONAL_RANGE, -DIRECTIONAL_RANGE, DIRECTIONAL_RANGE, -DIRECTIONAL_RANGE);
	}
							  break;
	case LightType_Point: {
		for (int i = 0; i < 6; ++i) {
			projectionMatrices[i] = Matrix4::Perspective(0.1f, pointLightParam.radius, 1, 90);
		}
	}
						break;
	case LightType_Spot: {
		projectionMatrices[0] = Matrix4::Perspective(0.1f, spotLightParam.radius, 1, spotLightParam.angle);
	}
					   break;
	default:
		break;
	}
}

void Light::UpdateViewMatrices() {
	Transform* transform = GetGO()->transform;
	Vector3 posW = transform->GetPositionWorld();

	switch (lightType)
	{
	case LightType_Directional: {
		viewMatrices[0] = Matrix4::BuildViewMatrix(DIRECTIONAL_CENT, DIRECTIONAL_CENT + transform->GetForward());
	}
							  break;
	case LightType_Point: {
		viewMatrices[0] = Matrix4::BuildViewMatrix(posW, posW + Vector3(0, 0, 1));
		viewMatrices[1] = Matrix4::BuildViewMatrix(posW, posW + Vector3(0, 0, -1));
		viewMatrices[2] = Matrix4::BuildViewMatrix(posW, posW + Vector3(1, 0, 0));
		viewMatrices[3] = Matrix4::BuildViewMatrix(posW, posW + Vector3(-1, 0, 0));
		viewMatrices[4] = Matrix4::BuildViewMatrix(posW, posW + Vector3(0, -1, 0), Vector3(0, 0, 1));
		viewMatrices[5] = Matrix4::BuildViewMatrix(posW, posW + Vector3(0, 1, 0), Vector3(0, 0, -1)); 
	}
						break;
	case LightType_Spot: {
		viewMatrices[0] = Matrix4::BuildViewMatrix(posW, posW + transform->GetForward());
	}
					   break;
	default:
		break;
	}
}

void Light::SetUpShadowMap() {
	switch (lightType)
	{
	case LightType_Directional: {
		shadowMap = make_shared<RenderTargetTexture>(shadowMapSize, shadowMapSize, false, true, false, false,
													 GL_LINEAR, GL_LINEAR);
	}
							  break;
	case LightType_Point: {
		shadowCubeMap = make_shared<RenderTargetCubeTexture>(shadowMapSize, shadowMapSize, false, true, false, false, true);
	}
						break;
	case LightType_Spot: {
		shadowMap = make_shared<RenderTargetTexture>(shadowMapSize, shadowMapSize, false, true, false, false,
													 GL_LINEAR, GL_LINEAR);
	}
					   break;
	default:
		break;
	}
}