#include "Texture.h"

GLuint Texture::fboTextureCopy = 0;

Texture::Texture() :
	format(GL_TEXTURE_2D)
{
	glCreateTextures(format, 1, &idTexture);
}

//TODO: Support formats other than GL_TEXTURE_2D
Texture::Texture(GLenum format) :
	format(format)
{
	glCreateTextures(format, 1, &idTexture);
}

Texture::~Texture()
{
	DestroyTextureCopyFBO();
	glDeleteTextures(1, &idTexture);
}

bool Texture::SetupTexture2D(GLint level,
						GLint internalformat,
						GLsizei width,
						GLsizei height,
						GLint border,
						GLenum pixelDataFormat,
						GLenum pixelDataType,
						const void* data, 
						GLfloat minFilter,
						GLfloat maxFilter,
						GLfloat wrapMode)
{
	if (format != GL_TEXTURE_2D) //Only support GL_TEXTURE_2D
		throw NotImplementedException(); //TODO: Try how it works with CubeMap

	glBindTexture(format, idTexture);
	glTexImage2D(format, level, internalformat, width, height, border, pixelDataFormat, pixelDataType, data);
	this->width = width;
	this->height = height;
	this->internalFormat = internalformat;
	this->mipMapLevel = level;
	this->borderType = border;
	this->pixelDataFormat = pixelDataFormat;
	this->pixelDataType = pixelDataType;

	SetMinFilter(minFilter);
	SetMaxFilter(maxFilter);
	SetWrapMode(wrapMode);

	glBindTexture(format, 0);

	return true;
}

bool Texture::SetupCubeMap(GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum pixelDataFormat, GLenum pixelDataType, const void* data, GLfloat minFilter, GLfloat maxFilter, GLfloat wrapMode)
{
	if (format != GL_TEXTURE_CUBE_MAP) //Only support GL_TEXTURE_2D
		throw NotImplementedException(); //TODO: Try how it works with CubeMap

	glBindTexture(format, idTexture);
	for (int i = 0; i < 6; ++i) {
		glTexImage2D(GetGLCubemapTarget((CubeTextureSide)i), level, internalformat, width, height, border, pixelDataFormat, pixelDataType, data);
	}
	
	this->width = width;
	this->height = height;
	this->internalFormat = internalformat;
	this->mipMapLevel = level;
	this->borderType = border;
	this->pixelDataFormat = pixelDataFormat;
	this->pixelDataType = pixelDataType;
	SetMinFilter(minFilter);
	SetMaxFilter(maxFilter);
	SetWrapMode(wrapMode);

	glBindTexture(format, 0);

	return true;
}

bool Texture::Load(const std::string& pathTexture, unsigned int flag)
{
	if (format != GL_TEXTURE_2D)
		return false;
	std::fstream f(pathTexture);
	if (!f||!f.good()) {
		throw LoadErrorException();
	}
	f.close();

	glActiveTexture(GL_TEXTURE0 + NCLGL_CUBETEX_UNIT_LOAD);

	int channels;
	idTexture = SOIL_load_OGL_texture(pathTexture.c_str(), SOIL_LOAD_AUTO, idTexture, flag,
		&width, &height, &channels); //TODO: Somehow causing OpenGL Error (but it works!). Figure out what is going on here...

	switch (channels)
	{
	case 1:
		internalFormat = GL_LUMINANCE;
		break;
	case 2:
		internalFormat = GL_LUMINANCE_ALPHA;
		break;
	case 3:
		internalFormat = GL_RGB;
		break;
	case 4:
		internalFormat = GL_RGBA;
		break;
	}

	glActiveTexture(GL_TEXTURE0);

	return idTexture != 0;
}

bool Texture::LoadCubeMap(const array<std::string, 6>& pathTextures, unsigned int flag)
{
	if (format != GL_TEXTURE_CUBE_MAP)
		return false;

	for (auto& path : pathTextures) {
		std::fstream f(path);
		if (!f.good()) {
			throw LoadErrorException();
		}
		f.close();
	}

	glActiveTexture(GL_TEXTURE0 + NCLGL_CUBETEX_UNIT_LOAD);

	int channels;
	idTexture = SOIL_load_OGL_cubemap(pathTextures[0].c_str(), pathTextures[1].c_str(),
									  pathTextures[2].c_str(), pathTextures[3].c_str(),
									  pathTextures[4].c_str(), pathTextures[5].c_str(),
									  SOIL_LOAD_AUTO, idTexture, flag,
									  &width, &height, &channels);

	switch (channels)
	{
	case 1:
		internalFormat = GL_LUMINANCE;
		break;
	case 2:
		internalFormat = GL_LUMINANCE_ALPHA;
		break;
	case 3:
		internalFormat = GL_RGB;
		break;
	case 4:
		internalFormat = GL_RGBA;
		break;
	}

	glActiveTexture(GL_TEXTURE0);

	return idTexture != 0;
}

bool Texture::Copy(shared_ptr<Texture> src, shared_ptr<Texture> targ)
{
	if (src->format != GL_TEXTURE_2D) //Only support GL_TEXTURE_2D
		throw NotImplementedException(); //TODO: Try how it works with CubeMap

	if (src->format != targ->format)
		return false;
	GLenum format = src->format;
	Texture::InitTextureCopyFBO();
	
	/// bind the FBO
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboTextureCopy);
	/// attach the source textureTest0 to the fbo
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		format, src->GetTextureID(), 0);
	
	/// bind the destination textureTest0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(format, targ->GetTextureID());
	
	/// copy from framebuffer (here, the FBO!) to the bound textureTest0
	glCopyTexSubImage2D(format, targ->mipMapLevel, 0, 0, 0, 0, targ->width, targ->height);
	
	/// unbind the FBO
	glBindTexture(format, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	return true;
}

shared_ptr<Texture> Texture::Copy()
{
	if (format != GL_TEXTURE_2D) //Only support GL_TEXTURE_2D
		throw NotImplementedException(); //TODO: Try how it works with CubeMap

	Texture::InitTextureCopyFBO(); //Initialize FBO

	//Create new textureTest0
	shared_ptr<Texture> texNew = make_shared<Texture>(format);
	if(format==GL_TEXTURE_2D)
		texNew->SetupTexture2D(mipMapLevel, internalFormat, width, height, borderType, pixelDataFormat, pixelDataType, 
								nullptr, GetMinFilter(), GetMaxFilter(), GetWrapMode(true));
	else if(format==GL_TEXTURE_CUBE_MAP)
		texNew->SetupTexture2D(mipMapLevel, internalFormat, width, height, borderType, pixelDataFormat, pixelDataType,
			nullptr, GetMinFilter(), GetMaxFilter(), GetWrapMode(true));

	//Bind source textureTest0 to FBO
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboTextureCopy);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		format, idTexture, mipMapLevel);

	//Activate and bind new textureTest0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(format, texNew->GetTextureID());
	//glCopyTexImage2D(format, texNew->mipMapLevel, texNew->internalFormat, 0, 0, texNew->width, texNew->height, texNew->borderType);

	//Copy textureTest0
	glCopyTexSubImage2D(format, texNew->mipMapLevel, 0, 0, 0, 0, texNew->width, texNew->height);
	
	//Finish up
	glBindTexture(format, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	return texNew;
}

int Texture::GetChannelCount(GLenum pixelDataFormat)
{
	switch (pixelDataFormat)
	{
	case(GL_STENCIL_INDEX):
	case(GL_DEPTH_COMPONENT):
	case(GL_DEPTH_STENCIL):
	case(GL_RED):
	case(GL_GREEN):
	case(GL_BLUE): 
	case(GL_RED_INTEGER):
	case(GL_GREEN_INTEGER):
	case(GL_BLUE_INTEGER):
	{
		return 1;
	}
	case(GL_RG): 
	case(GL_RG_INTEGER):
	{
		return 2;
	}
	case(GL_RGB):
	case(GL_BGR):
	case(GL_RGB_INTEGER):
	case(GL_BGR_INTEGER):
	{
		return 3;
	}
	case(GL_RGBA):
	case(GL_BGRA):
	case(GL_RGBA_INTEGER):
	case(GL_BGRA_INTEGER):
	{
		return 4;
	}
	default:
		break;
	}
}

void Texture::BindTexture(shared_ptr<Shader> shader, const string& gpuTextureName, GLenum glTextureUnit) const
{
	GLenum texUnit = glTextureUnit;
	if (format == GL_TEXTURE_2D) {

	}
	else if(format == GL_TEXTURE_CUBE_MAP){
		texUnit = NCLGL_CUBETEX_UNIT_BEGIN + glTextureUnit;
	}

	glUniform1i(glGetUniformLocation(shader->GetProgram(), gpuTextureName.c_str()), texUnit);	//Setting sampler in shaderPrePixelBump
	glActiveTexture(GL_TEXTURE0 + texUnit);	//Activating textureTest0 slot on GPU

	glBindTexture(format, idTexture);	//Binding texture object on CPU to GL_TEXTUREX activated on GPU
}

void Texture::UnbindTexture(shared_ptr<Shader> shader, GLenum glTextureUnit) const
{
	glActiveTexture(GL_TEXTURE0 + glTextureUnit);	//Activating texture slot on GPU
	glBindTexture(format, 0);	//Binding no texture
}

//TODO: Use sampler2DArray instead of this...
void Texture::BindTextures(shared_ptr<Shader> shader, const vector<shared_ptr<Texture>>& textures, const string& gpuTextureName, GLenum glTextureUnitStart)
{
	GLenum format = 0;
	int count = textures.size();

	for (int i = 0; i < count; ++i) {
		if (textures[i]) {
			format = textures[i]->GetFormat();
		}
	}
	if (format == 0) {
		return;
	}

	GLenum texUnitStart = glTextureUnitStart;
	if (format == GL_TEXTURE_2D) {

	}
	else if (format == GL_TEXTURE_CUBE_MAP) {
		texUnitStart = NCLGL_CUBETEX_UNIT_BEGIN + glTextureUnitStart;
	}

	vector<int> ids;
	for (int i = 0; i < count; ++i) {
		if (textures[i]==nullptr) {
			ids.emplace_back(0);
			continue;
		}
		ids.emplace_back(texUnitStart + i);
		glActiveTexture(GL_TEXTURE0 + texUnitStart + i);
		GLenum formatCur = textures[i]->format;
		if (format == 0)
			format = formatCur;
		if (formatCur != format) {
			cout << "texture array must be of the same format (due to OpenGL glitch of Cannot having 2 different format textures binding to 1 unit)" << endl;
			throw TypeNotSupportedException();
		}
		glBindTexture(format, textures[i]->idTexture);
	}

	glUniform1iv(glGetUniformLocation(shader->GetProgram(), gpuTextureName.c_str()), count, (int*)ids.data());
}


