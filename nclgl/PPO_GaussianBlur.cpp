#include "PPO_GaussianBlur.h"

PPO_GaussianBlur::PPO_GaussianBlur() :
	PostProcessObject() {
	shaderBlur = ShaderManager::GetInstance()->CreateShader(SCREEN_QUAD_SHADER_VERTEX, "PostProcess_GaussianBlur_Fragment.glsl");
	if (!shaderBlur) {
		shaderBlur = ShaderManager::GetInstance()->FindShader(ShaderManager::GetInstance()->GetName(SCREEN_QUAD_SHADER_VERTEX, "PostProcess_GaussianBlur_Fragment.glsl"));
	}
	if (!shaderBlur) {
		throw NullPtrException();
	}
	if (!shaderBlur->LoadSuccess()) {
		throw NotImplementedException();
	}
}

void PPO_GaussianBlur::SetupPostProcessObject(int screenWidth, int screenHeight)
{
	PostProcessObject::SetupPostProcessObject(screenWidth, screenHeight, true, false);
	renderTargetTmp0 = make_shared<RenderTargetTexture>(screenWidth, screenHeight, true, false);
}


void PPO_GaussianBlur::RenderPostProcess(shared_ptr<RenderTargetTexture> renderTargetLast)
{
	glUseProgram(shaderBlur->GetProgram());
	
	GLboolean isDepthTestEnabled = false;
	glGetBooleanv(GL_DEPTH_TEST, &isDepthTestEnabled);
	glDisable(GL_DEPTH_TEST);

	Texture::Copy(renderTargetLast->GetColourTexture(), renderTargetFinal->GetColourTexture());

	for (int i = 0; i < countPass; ++i) {
		renderTargetTmp0->SetAsRenderTarget();
		glUniform1i(glGetUniformLocation(shaderBlur->GetProgram(), "isVertical"), 0);
		renderTargetFinal->GetColourTexture()->BindTexture(shaderBlur, SCREEN_QUAD_TEXTURE, 0);
		ScreenQuad::GetScreenQuad()->Draw();
		//Swap buffer
		renderTargetFinal->SetAsRenderTarget();
		glUniform1i(glGetUniformLocation(shaderBlur->GetProgram(), "isVertical"), 1);
		renderTargetTmp0->GetColourTexture()->BindTexture(shaderBlur, SCREEN_QUAD_TEXTURE, 0);
		ScreenQuad::GetScreenQuad()->Draw();
	}
	RenderTargetTexture::SetDefaultRenderTarget();

	if (isDepthTestEnabled)
		glEnable(GL_DEPTH_TEST);
}

