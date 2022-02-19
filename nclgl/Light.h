#pragma once

#include <memory>

#include "Vector4.h"

#include "ComponentManaged.h"
#include "GameObject.h"
#include "Shader.h"
#include "Utility.h"
#include "RenderTargetTexture.h"
#include "RenderTargetCubeTexture.h"

#include "ShaderManager.h"

#define DIRECTIONAL_CENT (Vector3(0,0,0))
#define DIRECTIONAL_NEAR -500
#define DIRECTIONAL_FAR 500
#define DIRECTIONAL_RANGE 100

using namespace std;

enum LightType {
	LightType_Directional,
	LightType_Point,
	LightType_Spot
};

struct PointLightParameters {
	float radius; //radius of light
};

struct SpotLightParameters {
	float radius; //radius of light
	float angle; //full angle of light
};

class Light:
	public ComponentManaged
{
public:
	Light();
	virtual ~Light() = default;

	virtual void Update(float dt) override;

	inline void SetLightType(LightType lightType) { this->lightType = lightType; if (toCastShadow) SetUpShadow(); }
	inline LightType GetLightType() const { return this->lightType; }
	inline void SetLightColourAndIntensity(Vector4 colourAndIntensity) { this->colourAndIntensity = colourAndIntensity; }
	inline void SetLightColour(Vector3 colour) {
		this->colourAndIntensity.x = colour.x;
		this->colourAndIntensity.y = colour.y;
		this->colourAndIntensity.z = colour.z;
	}
	inline void SetLightIntensity(float intensity) {
		this->colourAndIntensity.z = intensity;
	}
	inline Vector4 GetLightColourAndIntensity() const { return this->colourAndIntensity; }


	//
	//Light Parameters
	// 
	//Directional Light
	//...
	//Point Light
	inline void SetPointLightParameters(PointLightParameters param) { this->pointLightParam = param; if (toCastShadow) SetUpProjectionMatrices(); }
	inline PointLightParameters GetPointLightParameters() const { return this->pointLightParam; }
	//SpotLight
	inline void SetSpotLightParameters(SpotLightParameters param) { this->spotLightParam = param; if(toCastShadow) SetUpProjectionMatrices(); }
	inline SpotLightParameters GetSpotLightParameters() const { return this->spotLightParam; }

	Vector4 GetLightParameterPackedVector() const;

	//Shadows
	inline void SetCastShadow(bool toCastShadow) { this->toCastShadow = toCastShadow; this->toCastShadow ? SetUpShadow() : DisposeShadowMap(); }
	inline bool GetCastShadow() const { return this->toCastShadow; }
	inline void SetShadowMapSize(int shadowMapSize) { this->shadowMapSize = shadowMapSize; if (toCastShadow) SetUpShadow(); }
	inline int GetShadowMapSize() const { return this->shadowMapSize; }

	inline Matrix4 GetShadowMatrix(int idx=0) const { return projectionMatrices[idx] * viewMatrices[idx]; }

	inline void UploadViewProjMatrixToShadowShader(int idx=0, bool isInstanced = false) const {
		shared_ptr<Shader> shader;
		if (lightType == LightType_Point) {
			if(!isInstanced)
				shader = GetPointLightShadowShader();
			else
				shader = GetPointLightShadowShaderInstanced();
		}
		else
		{
			if (!isInstanced)
				shader = GetShadowShader();
			else
				shader = GetShadowShaderInstanced();
		}
		glUseProgram(shader->GetProgram());
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), NCLGL_SHADER_VIEW_MATRIX), 1, false, viewMatrices[idx].values);
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), NCLGL_SHADER_PROJECTION_MATRIX), 1, false, projectionMatrices[idx].values);
	}

	//TODO: These are for shadow offset calculation for per vertex. Use this when support for that is added.
	//inline void UploadLightPositionToShader(shared_ptr<Shader> shader) const {
	//	Vector3 posW = GetGO()->transform->GetPositionWorld();
	//	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "lightPos"), 1, (float*)&posW);
	//}
	//inline void UploadShadowMatrixToShader(shared_ptr<Shader> shader, int idx = 0) const {
	//	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), NCLGL_SHADER_LIGHT_SHADOW_MATRIX), 1, false, ();
	//}
	//inline void UploadShadowMapToShader(shared_ptr<Shader> shader, int idx = 0) const {
	//	shadowMaps[idx]->GetDepthTexture()->BindTexture(shader, NCLGL_SHADER_LIGHT_SHADOW_MAP, NCLGL_TEXTURESLOT_SHADOWMAP); //TODO: Need way to avoid conflict with Material Textures
	//}
	//TODO END

	inline shared_ptr<RenderTargetTexture> GetShadowMap() const { return shadowMap; }
	inline shared_ptr<RenderTargetCubeTexture> GetShadowCubeMap() const { return shadowCubeMap; }
	inline static shared_ptr<Shader> GetShadowShader() { InitShadowShader(); return shadowShader; }
	inline static shared_ptr<Shader> GetShadowShaderInstanced() { InitShadowShader(); return shadowShaderInstanced; }
	inline static shared_ptr<Shader> GetPointLightShadowShader() { InitShadowShader(); return shadowShaderPointLight; }
	inline static shared_ptr<Shader> GetPointLightShadowShaderInstanced() { InitShadowShader(); return shadowShaderPointLightInstanced; }

protected:
	LightType lightType;
	Vector4 colourAndIntensity;

	PointLightParameters pointLightParam;
	SpotLightParameters spotLightParam;

	//Shadow
	bool toCastShadow = false;
	shared_ptr<RenderTargetTexture> shadowMap;
	shared_ptr<RenderTargetCubeTexture> shadowCubeMap;  
	int shadowMapSize = NCLGL_SHADOWMAP_SIZE;
	array<Matrix4, 6> viewMatrices;
	array<Matrix4, 6> projectionMatrices;

	static shared_ptr<Shader> shadowShader;
	static shared_ptr<Shader> shadowShaderInstanced;
	static shared_ptr<Shader> shadowShaderPointLight;
	static shared_ptr<Shader> shadowShaderPointLightInstanced;
	static void InitShadowShader() {
		if (!shadowShader) {
			shadowShader = ShaderManager::GetInstance()->CreateShader("shadowVert.glsl", "shadowFrag.glsl");
		}
		if (!shadowShader || !shadowShader->LoadSuccess()) {
			throw NullPtrException();
		}

		if (!shadowShaderInstanced) {
			shadowShaderInstanced = ShaderManager::GetInstance()->CreateShader("shadowVert_Instanced.glsl", "shadowFrag.glsl");
		}
		if (!shadowShaderInstanced || !shadowShaderInstanced->LoadSuccess()) {
			throw NullPtrException();
		}

		if (!shadowShaderPointLight) {
			shadowShaderPointLight = ShaderManager::GetInstance()->CreateShader("shadowPointLightVert.glsl", "shadowPointLightFrag.glsl");
		}
		if (!shadowShaderPointLight || !shadowShaderPointLight->LoadSuccess()) {
			throw NullPtrException();
		}

		if (!shadowShaderPointLightInstanced) {
			shadowShaderPointLightInstanced = ShaderManager::GetInstance()->CreateShader("shadowPointLightVert_Instanced.glsl", "shadowPointLightFrag.glsl");
		}
		if (!shadowShaderPointLightInstanced || !shadowShaderPointLightInstanced->LoadSuccess()) {
			throw NullPtrException();
		}
	}

	void SetUpShadow() {
		SetUpProjectionMatrices();
		UpdateViewMatrices();
		SetUpShadowMap();
	}

	void DisposeShadow() {
		DisposeShadowMap();
	}

	void SetUpProjectionMatrices();
	void UpdateViewMatrices();
	void SetUpShadowMap();
	inline void DisposeShadowMap() { shadowMap = nullptr; shadowCubeMap = nullptr;	}
};

