#pragma once

#include "Window.h"
#include <algorithm>
#include <array>

#include "Matrix4.h"
#include "Vector3.h"

#include "ComponentManaged.h"
#include "GameObject.h"
#include "Shader.h"
#include "Macros.h"
#include "Plane.h"
#include "RenderTargetTexture.h"
#include "Skybox.h"
#include "ScreenQuad.h"
#include "Rendering_UniformBufferManager.h"

using namespace std;

enum class CameraType {
	CameraType_Perspective,
	CameraType_Orthogonal
};

enum CameraClearType {
	CameraClearType_No,
	CameraClearType_Colour,
	CameraClearType_Skybox
};

struct CameraParamPerspective {
	float nearPlane = 0.001f;
	float farPlane = 1000.0f;
	float aspectWH = 1.0f;
	float fov = 45.0f; //Horizontal
};

struct CameraShaderData {
	Matrix4 viewMatrix;
	Matrix4 projMatrix;
};

//NOTE: Camera only supports perspective mode so far
//TODO: Add support for orth cam
class Camera: 
	public ComponentManaged
{
public:
	Camera(): ComponentManaged() {
		SetCameraParameters(this->cameraParam);
	}
	Camera(const CameraParamPerspective& cameraParam) : ComponentManaged() {
		SetCameraParameters(cameraParam);
	}

	virtual ~Camera() {};

	CameraType cameraType = CameraType::CameraType_Perspective;
	Matrix4 projMatrix;
	CameraParamPerspective cameraParam;

	virtual void Update(float dt) override;

	void SetCameraRenderTarget(int width, int height);
	inline void SetCameraRenderTarget(shared_ptr<RenderTargetTexture> renderTarget) { this->renderTarget = renderTarget; };
	shared_ptr<RenderTargetTexture> GetCameraRenderTarget() const { return renderTarget; }
	inline void SetRenderOrder(int order) { this->renderOrder = order; }
	inline int GetRenderOrder() const { return renderOrder; }

	//Depth Test and Writing
	inline void SetDepthTest(bool toTestDepth) { this->toDepthTest = toDepthTest; }
	inline bool GetDepthTest() const { return this->toDepthTest; }
	inline void SetDepthWrite(bool toWriteDepth) { this->toWriteDepth = toWriteDepth; }
	inline bool GetDepthWrite() const { return this->toWriteDepth; }
	inline void SetDepthFunc(GLenum depthFunc) { this->depthFunc = depthFunc; }
	inline GLenum GetDepthFunc() const { return this->depthFunc; }

	//Colour Writing
	inline void SetColourMask(bool r, bool g, bool b, bool a) { this->colourMask = { r,g,b,a }; }
	inline void GetColourMask(bool& r, bool& g, bool& b, bool& a) const { r = colourMask[0]; 
																		  g = colourMask[1];
																		  b = colourMask[2];
																		  a = colourMask[3];
																		}

	//Culling
	inline void SetFaceCulling(bool toCull) { this->toCull = toCull; }
	inline bool GetFaceCulling() const { return this->toCull; }
	inline void SetFaceCullingType(GLenum cullFacing) { this->cullFacing = cullFacing; }
	inline GLenum GetFaceCullingType() const { return this->cullFacing; }
	GLenum faceWindingOrder = GL_CCW;

	//Clearing
	inline void SetClearType(CameraClearType clearType) { this->clearType = clearType; }
	inline CameraClearType GetClearType() const { return this->clearType; }
	inline void SetClearFlag(GLbitfield clearFlag) { this->clearFlag = clearFlag; }
	inline GLbitfield GetClearFlag() { return this->clearFlag; }
	inline void SetClearColor(Vector4 clearColor) { this->clearColor = clearColor; };
	inline Vector4 GetClearColor() const { return this->clearColor; }
	inline void SetSkybox(shared_ptr<Skybox> skybox) { this->skybox = skybox; };
	shared_ptr<Skybox> GetSkybox() const { return skybox; }
	
	void RenderSkybox() {
		if (!skybox)
			throw NullPtrException();

		GLboolean isDepMaskWriteOn;
		glGetBooleanv(GL_DEPTH_WRITEMASK, &isDepMaskWriteOn);
		glDepthMask(GL_FALSE);

		auto shaderSkybox = Skybox::GetSkyboxShader();
		glUseProgram(shaderSkybox->GetProgram());
		UpdateMatrixToShader(shaderSkybox);
		skybox->UploadCubeMapToShader();

		ScreenQuad::GetScreenQuad()->Draw();

		if (isDepMaskWriteOn)
			glDepthMask(GL_TRUE);
	}

	void SetCameraParameters(const CameraParamPerspective& cameraParam);
	void GetCameraParameters(CameraParamPerspective& cameraParam)const { cameraParam = this->cameraParam; }
	Matrix4 BuildViewMatrix();
	void UpdateMatrixToShader(shared_ptr<Shader> shader);
	void UpdatePositionToLightedShader(shared_ptr<Shader> shader);

protected:
	//Render
	shared_ptr<RenderTargetTexture> renderTarget = nullptr; //The render target of the camera. If null, camera will render to Framebuffer 0
	int renderOrder = 0; //Smaller renderer->earlier rendering.

	//Depth Test and Writing
	bool toDepthTest = true;
	bool toWriteDepth = true;
	GLenum depthFunc = GL_LESS;

	//Color Writing
	array<bool, 4> colourMask = {true, true, true, true};

	//Culling
	bool toCull = true;
	GLenum cullFacing = GL_BACK;

	//Clearing
	CameraClearType clearType = CameraClearType_Colour;
	Vector4 clearColor = Vector4(0.1, 0.05, 0.15, 1.0);
	GLbitfield clearFlag;
	shared_ptr<Skybox> skybox;

	void UpdateProjMatrix();
};

