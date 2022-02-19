/******************************************************************************
Class:Shader
Implements:
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:VERY simple class to encapsulate GLSL shaderPrePixelBump loading, linking,
and binding. Useful additions to this class would be overloaded functions to
replace the glUniformxx functions in external code, and possibly a map to store
uniform names and their resulting bindings. 

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once
#include "OGLRenderer.h"

enum ShaderStage {
	SHADER_VERTEX,
	SHADER_FRAGMENT,
	SHADER_GEOMETRY,
	SHADER_DOMAIN,
	SHADER_HULL,
	SHADER_MAX
};

struct TransformFeedbackParam {
	vector<char*> feedbackVaryings;
	char** GetFeedbackVarying() {
		return feedbackVaryings.data();
	}
	int GetCount() {
		return feedbackVaryings.size();
	}
};

class Shader	{
public:
	Shader(const std::string& vertex, const std::string& fragment = "", const std::string& geometry = "", const std::string& domain = "", const std::string& hull = "",
			bool transformFeedback = false, const TransformFeedbackParam& transformFeedbackParam = TransformFeedbackParam());
	virtual ~Shader(void);

	GLuint  GetProgram() const { return programID;}

	void	Reload(bool deleteOld = true);

	bool	LoadSuccess() {
		return shaderValid[0] == GL_TRUE && programValid == GL_TRUE && !isShaderIncomplete;
	}

	inline bool	IsLighted() const { return glGetUniformLocation(programID, NCLGL_SHADER_LIGHT_TYPE) >= 0; }
	inline bool	IsShadowed() const { return glGetUniformLocation(programID, NCLGL_SHADER_LIGHT_SHADOW_TO_CAST) >= 0; }

	static void	PrintCompileLog(GLuint object);
	static void	PrintLinkLog(GLuint program);

protected:
	void	DeleteIDs();

	bool	LoadShaderFile(const  std::string& from, std::string &into);
	bool	CheckIsInclude(const string& line, string& includeFileName);
	bool	LoadShaderIncludeFile(const string& filename, string& into, int& lineNum);
	bool	GenerateShaderObject(unsigned int i);
	void	SetDefaultAttributes();
	void	LinkProgram();

	bool isShaderIncomplete = false;
	GLuint	programID = 0;
	GLuint	objectIDs[SHADER_MAX];
	GLint	programValid;
	GLint	shaderValid[SHADER_MAX];

	std::string  shaderFiles[SHADER_MAX];

	bool transformFeedback = false;
	TransformFeedbackParam transformFeedbackParam;

	void SetUpTransformFeedback();
};

