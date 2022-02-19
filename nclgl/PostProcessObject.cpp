#include "PostProcessObject.h"

shared_ptr<Shader> PostProcessObject::shaderScreenQuad = nullptr;

PostProcessObject::PostProcessObject()
{
	InitPostProcessShader();
}

void PostProcessObject::SetupPostProcessObject(int screenWidth, int screenHeight, bool toRenderColour, bool toRenderDepthAndStencil)
{
	renderTargetFinal = make_shared<RenderTargetTexture>(screenWidth, screenHeight, toRenderColour, toRenderDepthAndStencil);
}

void PostProcessObject::RenderToDefaultRenderTarget()
{
	RenderTargetTexture::SetDefaultRenderTarget();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderScreenQuad->GetProgram());
	
	GLboolean isDepTestOn = false;
	glGetBooleanv(GL_DEPTH_TEST, &isDepTestOn);
	glDisable(GL_DEPTH_TEST);
	renderTargetFinal->GetColourTexture()->BindTexture(shaderScreenQuad, SCREEN_QUAD_TEXTURE, 0);
	ScreenQuad::GetScreenQuad()->Draw();
	renderTargetFinal->GetColourTexture()->UnbindTexture(shaderScreenQuad, 0);
	if(isDepTestOn)
		glEnable(GL_DEPTH_TEST);
}
