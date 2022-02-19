#pragma once

#include <memory>

#include "RenderTargetTexture.h"
#include "Mesh.h"
#include "Geometry.h"
#include "ScreenQuad.h"

#include "ShaderManager.h"

using namespace std;

#define SCREEN_QUAD_TEXTURE "screenTex"
#define SCREEN_QUAD_SHADER_VERTEX "ScreenQuadVertex.glsl"
#define SCREEN_QUAD_SHADER_RAY_MARCH_VERTEX "ScreenQuadRayMarchVertex.glsl"
#define SCREEN_QUAD_SHADER_FRAGMENT "ScreenQuadFragment.glsl"

class PostProcessObject
{
public:
	PostProcessObject();
	virtual ~PostProcessObject() = default;

	bool isEnabled = true;

	void SetupPostProcessObject(int screenWidth, int screenHeight, bool toRenderColour=true, bool toRenderDepthAndStencil=false);
	virtual void SetupPostProcessObject(int screenWidth, int screenHeight) = 0;

	virtual void RenderPostProcess(shared_ptr<RenderTargetTexture> renderTargetLast) = 0;
	inline shared_ptr<RenderTargetTexture> GetFinalResult() { return renderTargetFinal; }
	void RenderToDefaultRenderTarget();

protected:
	shared_ptr<RenderTargetTexture> renderTargetFinal; //To contain the final result of this PostProcessObject

	static shared_ptr<Shader> shaderScreenQuad;
	inline static void InitPostProcessShader() {
		if(!shaderScreenQuad)
			shaderScreenQuad = CreatePostProcessShader();
		if (!shaderScreenQuad)
			throw NullPtrException();
	}
	static shared_ptr<Shader> CreatePostProcessShader() {
		shared_ptr<Shader> shader = ShaderManager::GetInstance()->CreateShader(SCREEN_QUAD_SHADER_VERTEX, SCREEN_QUAD_SHADER_FRAGMENT);
		if (!shader) {
			return shaderScreenQuad;
		}
		if (!shader->LoadSuccess()) {
			throw NotImplementedException();
		}
		return shader;
	}
};

