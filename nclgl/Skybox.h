#pragma once

#include <memory>

#include "Texture.h"
#include "Shader.h"
#include "Utility.h"

#include "ShaderManager.h"

using namespace std;

class Skybox
{
public:
	Skybox() = default;
	virtual ~Skybox() = default;
	
	inline void SetCubeMap(shared_ptr<Texture> cubeMap) { if (cubeMap->GetFormat() != GL_TEXTURE_CUBE_MAP) throw TypeNotSupportedException(); this->cubeMap = cubeMap; }
	inline shared_ptr<Texture> GetCubeMap() const { return this->cubeMap; }

	static shared_ptr<Shader> GetSkyboxShader() {
		InitShaderSkybox();
		return shaderSkybox;
	}
	void UploadCubeMapToShader() {
		cubeMap->BindTexture(shaderSkybox, "cubeTex", 0);
	}

protected:
	shared_ptr<Texture> cubeMap;
	static shared_ptr<Shader> shaderSkybox;
	
	static void InitShaderSkybox() {
		if (!shaderSkybox) {
			shaderSkybox = ShaderManager::GetInstance()->CreateShader("skyboxVertex.glsl", "skyboxFragment.glsl");
		}
		else
		{
			return;
		}
		if (!shaderSkybox) {
			shaderSkybox = ShaderManager::GetInstance()->FindShader(ShaderManager::GetInstance()->GetName("skyboxVertex.glsl", "skyboxFragment.glsl"));
		}
		if (!shaderSkybox || !shaderSkybox->LoadSuccess()) {
			throw NullPtrException();
		}
	}
};

