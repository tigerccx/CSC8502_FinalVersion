#include "PPO_ColourGrading.h"

PPO_ColourGrading::PPO_ColourGrading() :
	PostProcessObject() {
	shaderGrading = ShaderManager::GetInstance()->CreateShader(SCREEN_QUAD_SHADER_VERTEX, "PostProcess_ColourGrading_Fragment.glsl");
	if (!shaderGrading) {
		throw NullPtrException();
	}
	if (!shaderGrading->LoadSuccess()) {
		throw NotImplementedException();
	}
}

void PPO_ColourGrading::SetupPostProcessObject(int screenWidth, int screenHeight)
{
	PostProcessObject::SetupPostProcessObject(screenWidth, screenHeight, true, false);
}


void PPO_ColourGrading::RenderPostProcess(shared_ptr<RenderTargetTexture> renderTargetLast)
{
	glUseProgram(shaderGrading->GetProgram());
	
	GLboolean isDepthTestEnabled = false;
	glGetBooleanv(GL_DEPTH_TEST, &isDepthTestEnabled);
	glDisable(GL_DEPTH_TEST);

	renderTargetFinal->SetAsRenderTarget();
	glUniform1i(glGetUniformLocation(shaderGrading->GetProgram(), "stages"), stages);
	renderTargetLast->GetColourTexture()->BindTexture(shaderGrading, SCREEN_QUAD_TEXTURE, 0);
	ScreenQuad::GetScreenQuad()->Draw();
	RenderTargetTexture::SetDefaultRenderTarget();

	if (isDepthTestEnabled)
		glEnable(GL_DEPTH_TEST);
}

