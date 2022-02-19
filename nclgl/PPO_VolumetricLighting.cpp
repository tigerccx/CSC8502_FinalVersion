#include "PPO_VolumetricLighting.h"

PPO_VolumetricLighting::PPO_VolumetricLighting():
	PostProcessObject(){
	shaderVolumetricLighting = ShaderManager::GetInstance()->CreateShader(SCREEN_QUAD_SHADER_RAY_MARCH_VERTEX, "PostProcess_VolumetricLighting_Fragment.glsl");
	if (!shaderVolumetricLighting) {
		throw NullPtrException();
	}
	if (!shaderVolumetricLighting->LoadSuccess()) {
		throw NotImplementedException();
	}

	shaderAdd = ShaderManager::GetInstance()->CreateShader(SCREEN_QUAD_SHADER_VERTEX, "PostProcess_Add_Fragment.glsl");
	if (!shaderAdd) {
		throw NullPtrException();
	}
	if (!shaderAdd->LoadSuccess()) {
		throw NotImplementedException();
	}
}

void PPO_VolumetricLighting::SetRenderScale(float scale)
{
	rtTemp = make_shared<RenderTargetTexture>(int(screenWidth * scale), int(screenHeight * scale), true, false);
}

void PPO_VolumetricLighting::SetupPostProcessObject(int screenWidth, int screenHeight)
{
	PostProcessObject::SetupPostProcessObject(screenWidth, screenHeight, true, false);
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	SetRenderScale(1);
}

void PPO_VolumetricLighting::RenderPostProcess(shared_ptr<RenderTargetTexture> renderTargetLast)
{
	assert(camRender);

	glUseProgram(shaderVolumetricLighting->GetProgram());

	GLboolean isDepthTestEnabled = false;
	glGetBooleanv(GL_DEPTH_TEST, &isDepthTestEnabled);
	glDisable(GL_DEPTH_TEST);

	//Render light to tmp
	rtTemp->SetAsRenderTarget();
	Matrix4 viewInvMatrix = camRender->BuildViewMatrix();
	viewInvMatrix.Invert();
	Matrix4 projInvMatrix = camRender->projMatrix;
	projInvMatrix.Invert();

	LightManager::GetInstance()->UploadLightsDataToShader(shaderVolumetricLighting);
	camRender->UpdatePositionToLightedShader(shaderVolumetricLighting);
	glUniform1f(glGetUniformLocation(shaderVolumetricLighting->GetProgram(), "cameraNear"), camRender->cameraParam.nearPlane);
	LightManager::GetInstance()->UploadShadowDataToShader(shaderVolumetricLighting);

	glUniformMatrix4fv(glGetUniformLocation(shaderVolumetricLighting->GetProgram(), "viewInverseMatrix"), 1, false, viewInvMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(shaderVolumetricLighting->GetProgram(), "projInverseMatrix"), 1, false, projInvMatrix.values);
	glUniform1i(glGetUniformLocation(shaderVolumetricLighting->GetProgram(), "steps"), steps);
	glUniform1f(glGetUniformLocation(shaderVolumetricLighting->GetProgram(), "stepLength"), stepLength);
	glUniform1f(glGetUniformLocation(shaderVolumetricLighting->GetProgram(), "strength"), strength);
	renderTargetLast->GetColourTexture()->BindTexture(shaderVolumetricLighting, SCREEN_QUAD_TEXTURE, 0);
	auto quad = ScreenQuad::GetScreenQuad();
	quad->Draw();
	renderTargetLast->GetColourTexture()->UnbindTexture(shaderVolumetricLighting, 0);
	
	glUseProgram(shaderAdd->GetProgram());
	//glViewport(0, 0, renderTargetFinal->GetWidth(), renderTargetFinal->GetHeight());
	renderTargetFinal->SetAsRenderTarget();
	renderTargetLast->GetColourTexture()->BindTexture(shaderAdd, "texA", 0);
	rtTemp->GetColourTexture()->BindTexture(shaderAdd, "texB", 1);
	quad->Draw();
	renderTargetLast->GetColourTexture()->UnbindTexture(shaderAdd, 0);
	rtTemp->GetColourTexture()->UnbindTexture(shaderAdd, 1);

	RenderTargetTexture::SetDefaultRenderTarget();

	if (isDepthTestEnabled)
		glEnable(GL_DEPTH_TEST);
}