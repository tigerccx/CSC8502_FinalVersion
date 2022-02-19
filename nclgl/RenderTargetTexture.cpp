#include "RenderTargetTexture.h"

RenderTargetTexture::~RenderTargetTexture()
{
	if (idFrameBuffer)
		glDeleteFramebuffers(1, &idFrameBuffer);
}

void RenderTargetTexture::SetupRenderTargetTexture(int width, int height,
	bool toRenderColor,
	bool toRenderDepth,
	bool toRenderStencil,
	bool toCompressDepthWithStencil,
	GLfloat minFilter, GLfloat maxFilter)
{
	glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer);
	if (toRenderColor) {
		textureColour = make_shared<Texture>();
		textureColour->SetupTexture2D(0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL, minFilter, maxFilter, GL_CLAMP);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColour->GetTextureID(), 0);
	}
	else
	{
		glDrawBuffer(GL_NONE);
	}

	if (toCompressDepthWithStencil && toRenderDepth && toRenderStencil) {
		textureDepthAndStencil = make_shared<Texture>();
		textureDepthAndStencil->SetupTexture2D(0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL, minFilter, maxFilter, GL_CLAMP);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureDepthAndStencil->GetTextureID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureDepthAndStencil->GetTextureID(), 0);
	}
	else {
		if (toCompressDepthWithStencil) {
			throw ParameterNotSupportedException();
		}
		else {
			if (toRenderDepth) {
				textureDepth = make_shared<Texture>();
				textureDepth->SetupTexture2D(0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr,
					minFilter, maxFilter, GL_CLAMP_TO_EDGE);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureDepth->GetTextureID(), 0);
			}
			if (toRenderStencil) {
				textureStencil = make_shared<Texture>();
				textureStencil->SetupTexture2D(0, GL_R8, width, height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_INT, nullptr,
					minFilter, maxFilter, GL_CLAMP_TO_EDGE);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureStencil->GetTextureID(), 0);
			}
		}
	}

	this->width = width;
	this->height = height;

	GLenum completeness = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (completeness != GL_FRAMEBUFFER_COMPLETE) {
		throw NotImplementedException(); //Probably need some handling
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
