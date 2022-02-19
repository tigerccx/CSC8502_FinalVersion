#pragma once

#include <memory>
#include <array>

#include "Texture.h"
#include "Utility.h"

using namespace std;

//TODO: Incomplete
class RenderTargetCubeTexture
{
public:
	RenderTargetCubeTexture() { glGenFramebuffers(1, &idFrameBuffer); }
	RenderTargetCubeTexture(int width, int height,
		bool toRenderColor,
		bool toRenderDepth = true,
		bool toRenderStencil = false,
		bool toCompressDepthWithStencil = false,
		bool depthAsColor = false) {
		glGenFramebuffers(1, &idFrameBuffer);
		SetupRenderTargetTexture(width, height, toRenderColor, toRenderDepth, toRenderStencil, toCompressDepthWithStencil, depthAsColor);
	}

	void SetupRenderTargetTexture(int width, int height,
		bool toRenderColor = true,
		bool toRenderDepth = false,
		bool toRenderStencil = false,
		bool toCompressDepthWithStencil = false,
		bool depthAsColor = false);

	inline shared_ptr<Texture> GetDepthTexture() const { return cubeTextureDepth; }

	void SetAsRenderTarget(CubeTextureSide side) {
		glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer); 
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, Texture::GetGLCubemapTarget(side), cubeTextureDepth->GetTextureID(), 0);

		GLenum completeness = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (completeness != GL_FRAMEBUFFER_COMPLETE) {
			throw NotImplementedException(); //Probably need some handling
		}
	}
	void ClearRenderTargetBinding(CubeTextureSide side) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, Texture::GetGLCubemapTarget(side), 0, 0);
	}
	static void SetDefaultRenderTarget() { 
		glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	}

protected:
	GLuint idFrameBuffer;
	shared_ptr<Texture> cubeTextureDepth; //F, B, R, L, T, B


	bool toRenderColor = false;
	bool toRenderDepth = false;
	bool depthAsColor = false;
	bool toRenderStencil = false;
	bool toCompressDepthWithStencil = false;
};

