#pragma once

#include <memory>

#include "Texture.h"
#include "Utility.h"

using namespace std;

class RenderTargetTexture
{
public:
	RenderTargetTexture() { glGenFramebuffers(1, &idFrameBuffer); }
	RenderTargetTexture(int width, int height,
		bool toRenderColor,
		bool toRenderDepth = true,
		bool toRenderStencil = false,
		bool toCompressDepthWithStencil = false,
		GLfloat minFilter = GL_NEAREST, GLfloat maxFilter = GL_NEAREST) {
		glGenFramebuffers(1, &idFrameBuffer);
		SetupRenderTargetTexture(width, height, toRenderColor, toRenderDepth, toRenderStencil, toCompressDepthWithStencil, minFilter, maxFilter);
	}
	virtual ~RenderTargetTexture();

	inline int GetWidth() { return width; }
	inline int GetHeight() { return height; }

	void SetupRenderTargetTexture(int width, int height,
		bool toRenderColor = true,
		bool toRenderDepth = false,
		bool toRenderStencil = false,
		bool toCompressDepthWithStencil = false,
		GLfloat minFilter = GL_NEAREST, GLfloat maxFilter = GL_NEAREST);

	void SetColourTexture(shared_ptr<Texture> textureColour) {
		this->textureColour = textureColour;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColour->GetTextureID(), 0);
	}
	inline shared_ptr<Texture> GetColourTexture() const { return textureColour; }

	void SetDepthAndStencilTexture(shared_ptr<Texture> textureDepthAndStencil) {
		this->textureDepthAndStencil = textureDepthAndStencil;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureDepthAndStencil->GetTextureID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureDepthAndStencil->GetTextureID(), 0);
	}
	inline shared_ptr<Texture> GetDepthAndStencilTexture() const { return textureDepthAndStencil; }

	void SetDepthTexture(shared_ptr<Texture> textureDepth) {
		this->textureDepth = textureDepth;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureDepth->GetTextureID(), 0);
	}
	inline shared_ptr<Texture> GetDepthTexture() const { return textureDepth; }

	void SetStencilTexture(shared_ptr<Texture> textureStencil) {
		this->textureStencil = textureStencil;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureStencil->GetTextureID(), 0);
	}
	inline shared_ptr<Texture> GetStencilTexture() const { return textureStencil; }

	void SetAsRenderTarget() { 
		glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer); 
		glViewport(0, 0, width, height);
		GLenum completeness = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (completeness != GL_FRAMEBUFFER_COMPLETE) {
			throw NotImplementedException(); //Probably need some handling
		}
	}
	static void SetDefaultRenderTarget() { 
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		Vector2 size = Window::GetCurrentWindow()->GetScreenSize();
		glViewport(0, 0, size.x, size.y);
	}

protected:
	GLuint idFrameBuffer;
	shared_ptr<Texture> textureColour = nullptr;
	shared_ptr<Texture> textureDepth = nullptr;
	shared_ptr<Texture> textureStencil = nullptr;
	shared_ptr<Texture> textureDepthAndStencil = nullptr;

	int width;
	int height;
};