#pragma once
#include <string>
#include <memory>
#include <array>
#include <fstream>

#include "common.h"

#include <glad\glad.h>
#include "SOIL/SOIL.h"

#include "Shader.h"
#include "Matrix4.h"
#include "Utility.h"

using namespace std;

enum CubeTextureSide {
	CubeTextureSide_Front,
	CubeTextureSide_Back,
	CubeTextureSide_Right,
	CubeTextureSide_Left,
	CubeTextureSide_Top,
	CubeTextureSide_Bottom,
};

//TODO: Add more supported format
//Currently only fully support GL_TEXTURE_2D. 
//Need to add more support for GL_TEXTURE_CUBE_MAP
class Texture
{
public:
	Texture();
	Texture(GLenum format);
	virtual ~Texture();

	bool SetupTexture2D(GLint level,
					GLint internalformat,
					GLsizei width,
					GLsizei height,
					GLint border,
					GLenum pixelDataFormat,
					GLenum pixelDataType,
					const void* data = nullptr,
					GLfloat minFilter = GL_LINEAR,
					GLfloat maxFilter = GL_LINEAR,
					GLfloat wrapMode = GL_CLAMP);
	bool SetupCubeMap(
		GLint level,
		GLint internalformat,
		GLsizei width,
		GLsizei height,
		GLint border,
		GLenum pixelDataFormat,
		GLenum pixelDataType,
		const void* data = nullptr,
		GLfloat minFilter = GL_LINEAR,
		GLfloat maxFilter = GL_LINEAR,
		GLfloat wrapMode = GL_CLAMP
	);
	bool Load(const string& pathTexture, unsigned int flag);
	bool LoadCubeMap(const array<string, 6>& pathTextures, unsigned int flag);

	//Texture& operator=(const Texture& other);

	static bool Copy(shared_ptr<Texture> src, shared_ptr<Texture> targ);
	shared_ptr<Texture> Copy();
	static int GetChannelCount(GLenum pixelDataFormat);

	static GLenum GetGLCubemapTarget(CubeTextureSide side) {
		switch (side)
		{
		case CubeTextureSide_Front:
			return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
			break;
		case CubeTextureSide_Back:
			return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
			break;
		case CubeTextureSide_Right:
			return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
			break;
		case CubeTextureSide_Left:
			return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			break;
		case CubeTextureSide_Top:
			return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
			break;
		case CubeTextureSide_Bottom:
			return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
			break;
		default:
			break;
		}
	}

	//void* GetTextureData();
	//static shared_ptr<Texture> CreateCubeMapFromTextures(array<shared_ptr<Texture>, 6>& textures);

	static void InitTextureCopyFBO() { if(!fboTextureCopy) glGenFramebuffers(1, &fboTextureCopy); }
	static void DestroyTextureCopyFBO() { if (!fboTextureCopy) { glDeleteFramebuffers(1, &fboTextureCopy); fboTextureCopy = 0; } }

	void BindTexture(shared_ptr<Shader> shader, const string& gpuTextureName, GLenum glTextureUnit) const;
	void UnbindTexture(shared_ptr<Shader> shader, GLenum glTextureUnit) const;
	static void BindTextures(shared_ptr<Shader> shader, const vector<shared_ptr<Texture>>& textures, 
						const string& gpuTextureName, GLenum glTextureIDStart);
	inline bool IsValid() const { return (bool)idTexture; }
	inline GLuint GetTextureID() const { return idTexture; }

	void inline SetParameterf(GLenum pname, GLfloat param) { glBindTexture(format, idTexture); glTexParameterf(format, pname, param); glBindTexture(format, 0); }
	void inline SetParameteri(GLenum pname, GLint param) { glBindTexture(format, idTexture); glTexParameteri(format, pname, param); glBindTexture(format, 0); }
	void inline SetParameterfv(GLenum pname, const GLfloat* param) { glBindTexture(format, idTexture); glTexParameterfv(format, pname, param); glBindTexture(format, 0); }
	void inline SetParameteriv(GLenum pname, const GLint* param) { glBindTexture(format, idTexture); glTexParameteriv(format, pname, param); glBindTexture(format, 0); }
	void inline GetParameterf(GLenum pname, GLfloat* param) { glBindTexture(format, idTexture); glGetTexParameterfv(format, pname, param); glBindTexture(format, 0); }
	void inline GetParameteri(GLenum pname, GLint* param) { glBindTexture(format, idTexture); glGetTexParameterIiv(format, pname, param); glBindTexture(format, 0); }
	void inline GetParameterfv(GLenum pname, GLfloat* param) { glBindTexture(format, idTexture); glGetTexParameterfv(format, pname, param); glBindTexture(format, 0); }
	void inline GetParameteriv(GLenum pname, GLint* param) { glBindTexture(format, idTexture); glGetTexParameterIiv(format, pname, param); glBindTexture(format, 0); }

	void inline SetTextureRepeat(bool isRepeating) {
		this->isRepeating = isRepeating;
		SetParameteri(GL_TEXTURE_WRAP_S, isRepeating ? GL_REPEAT : GL_CLAMP);
		SetParameteri(GL_TEXTURE_WRAP_T, isRepeating ? GL_REPEAT : GL_CLAMP);
	}

	GLenum inline GetFormat() const { return format;} 

	void inline SetMinFilter(GLfloat filter) { SetParameterf(GL_TEXTURE_MIN_FILTER, filter); }
	GLfloat inline GetMinFilter() { GLfloat rslt; GetParameterf(GL_TEXTURE_MIN_FILTER, &rslt); return rslt; }
	void inline SetMaxFilter(GLfloat filter) { SetParameterf(GL_TEXTURE_MAG_FILTER, filter); }
	GLfloat inline GetMaxFilter() { GLfloat rslt; GetParameterf(GL_TEXTURE_MAG_FILTER, &rslt); return rslt; }
	void inline SetWrapMode(GLfloat wrapMode) { SetParameterf(GL_TEXTURE_WRAP_S, wrapMode); SetParameterf(GL_TEXTURE_WRAP_T, wrapMode);}
	GLfloat inline GetWrapMode(bool isHorizontal = true) { GLfloat rslt; GetParameterf(isHorizontal? GL_TEXTURE_WRAP_S:GL_TEXTURE_WRAP_T, &rslt); return rslt; }

protected:

	const GLenum format;
	GLuint idTexture = 0;
	
	int width = 0;
	int height = 0;
	GLint internalFormat = 0;
	GLint mipMapLevel = 0;
	GLint borderType = 0;
	GLenum pixelDataFormat = 0;
	GLenum pixelDataType = 0;

	bool isRepeating = false;

	static GLuint fboTextureCopy;
};

