#include "PPO_DoubleVision.h"

PPO_DoubleVision::PPO_DoubleVision() :
	PostProcessObject() {
	shaderDoubleVision = ShaderManager::GetInstance()->CreateShader(SCREEN_QUAD_SHADER_VERTEX, "PostProcess_DoubleVision_Fragment.glsl");
	if (!shaderDoubleVision) {
		shaderDoubleVision = ShaderManager::GetInstance()->FindShader(ShaderManager::GetInstance()->GetName(SCREEN_QUAD_SHADER_VERTEX, "PostProcess_DoubleVision_Fragment.glsl"));
	}
	if (!shaderDoubleVision) {
		throw NullPtrException();
	}
	if (!shaderDoubleVision->LoadSuccess()) {
		throw NotImplementedException();
	}
}

void PPO_DoubleVision::SetupPostProcessObject(int screenWidth, int screenHeight)
{
	PostProcessObject::SetupPostProcessObject(screenWidth, screenHeight, true, false);
}


void PPO_DoubleVision::RenderPostProcess(shared_ptr<RenderTargetTexture> renderTargetLast)
{
	glUseProgram(shaderDoubleVision->GetProgram());

	GLboolean isDepthTestEnabled = false;
	glGetBooleanv(GL_DEPTH_TEST, &isDepthTestEnabled);
	glDisable(GL_DEPTH_TEST);

	renderTargetFinal->SetAsRenderTarget();
	glUniform2fv(glGetUniformLocation(shaderDoubleVision->GetProgram(), "offset"), 1, (float*)&offset);
	glUniform1f(glGetUniformLocation(shaderDoubleVision->GetProgram(), "blend"), blend);
	renderTargetLast->GetColourTexture()->BindTexture(shaderDoubleVision, SCREEN_QUAD_TEXTURE, 0);
	ScreenQuad::GetScreenQuad()->Draw();
	RenderTargetTexture::SetDefaultRenderTarget();

	if (isDepthTestEnabled)
		glEnable(GL_DEPTH_TEST);
}