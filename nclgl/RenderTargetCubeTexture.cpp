#include "RenderTargetCubeTexture.h"

void RenderTargetCubeTexture::SetupRenderTargetTexture(int width, int height,
	bool toRenderColor,
	bool toRenderDepth,
	bool toRenderStencil,
	bool toCompressDepthWithStencil,
	bool depthAsColor)
{	
	this->toRenderColor = toRenderColor;
	this->toRenderDepth = toRenderDepth;
	this->toRenderStencil = toRenderStencil;
	this->toCompressDepthWithStencil = toCompressDepthWithStencil;
	this->depthAsColor = depthAsColor;

	glBindFramebuffer(GL_FRAMEBUFFER, idFrameBuffer);

	if (toRenderColor) {
		/*textureColour = make_shared<Texture>();
		textureColour->SetupTexture2D(0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL, GL_NEAREST, GL_NEAREST, GL_CLAMP);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColour->GetTextureID(), 0);*/
		throw NotImplementedException();
	}
	else
	{
		glDrawBuffer(GL_NONE);
	}

	if (toCompressDepthWithStencil && toRenderDepth && toRenderStencil) {
		/*textureDepthAndStencil = make_shared<Texture>();
		textureDepthAndStencil->SetupTexture2D(0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL, GL_NEAREST, GL_NEAREST, GL_CLAMP);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureDepthAndStencil->GetTextureID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureDepthAndStencil->GetTextureID(), 0);*/
		throw NotImplementedException();
	}
	else {
		if (toCompressDepthWithStencil) {
			throw ParameterNotSupportedException();
		}
		else {
			if (toRenderDepth) {
				cubeTextureDepth = make_shared<Texture>(GL_TEXTURE_CUBE_MAP);
				cubeTextureDepth->SetupCubeMap(0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr,
					GL_LINEAR, GL_NEAREST, GL_CLAMP_TO_EDGE);
				/*
				else {
					cubeTextureDepth = make_shared<Texture>(GL_TEXTURE_CUBE_MAP);
					cubeTextureDepth->SetupCubeMap(0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr,
						GL_LINEAR, GL_NEAREST, GL_CLAMP_TO_EDGE);
					cubeTextureDepth0 = make_shared<Texture>(GL_TEXTURE_CUBE_MAP);
					cubeTextureDepth0->SetupCubeMap(0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr,
						GL_LINEAR, GL_NEAREST, GL_CLAMP_TO_EDGE);
				}*/
			}
			if (toRenderStencil) {
				/*textureStencil = make_shared<Texture>();
				textureStencil->SetupTexture2D(0, GL_R8, width, height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_INT, nullptr,
					GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureStencil->GetTextureID(), 0);*/
				throw NotImplementedException();
			}
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}