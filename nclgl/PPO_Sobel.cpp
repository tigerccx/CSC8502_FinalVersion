#include "PPO_Sobel.h"

PPO_Sobel::PPO_Sobel():
	PostProcessObject() {
	shaderSobel = ShaderManager::GetInstance()->CreateShader(SCREEN_QUAD_SHADER_VERTEX, "PostProcess_Sobel_Fragment.glsl");
	if (!shaderSobel) {
		shaderSobel = ShaderManager::GetInstance()->FindShader(ShaderManager::GetInstance()->GetName(SCREEN_QUAD_SHADER_VERTEX, "PostProcess_Sobel_Fragment.glsl"));
	}
	if (!shaderSobel) {
		throw NullPtrException();
	}
	if (!shaderSobel->LoadSuccess()) {
		throw NotImplementedException();
	}
}

void PPO_Sobel::SetupPostProcessObject(int screenWidth, int screenHeight)
{
	PostProcessObject::SetupPostProcessObject(screenWidth, screenHeight, true, false);
	renderTargetTmp0 = make_shared<RenderTargetTexture>(screenWidth, screenHeight, true, false);
}


void PPO_Sobel::RenderPostProcess(shared_ptr<RenderTargetTexture> renderTargetLast)
{
	glUseProgram(shaderSobel->GetProgram());
	
	GLboolean isDepthTestEnabled = false;
	glGetBooleanv(GL_DEPTH_TEST, &isDepthTestEnabled);
	glDisable(GL_DEPTH_TEST);

	Texture::Copy(renderTargetLast->GetColourTexture(), renderTargetFinal->GetColourTexture());

	shared_ptr<RenderTargetTexture> rtSrc = renderTargetFinal;
	shared_ptr<RenderTargetTexture> rtTarg = renderTargetTmp0;

	for (int i = 0; i < countPass; ++i) {
		rtTarg->SetAsRenderTarget();
		rtSrc->GetColourTexture()->BindTexture(shaderSobel, SCREEN_QUAD_TEXTURE, 0);
		ScreenQuad::GetScreenQuad()->Draw();
		//Swap buffer
		swap(rtSrc, rtTarg);
	}

	if (rtSrc != renderTargetFinal) { //If last pass draw to tmp0
		swap(renderTargetFinal, renderTargetTmp0);
	}

	RenderTargetTexture::SetDefaultRenderTarget();

	if (isDepthTestEnabled)
		glEnable(GL_DEPTH_TEST);
}