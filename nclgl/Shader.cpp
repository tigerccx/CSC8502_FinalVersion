#include "Shader.h"
#include "Mesh.h"
#include <iostream>

using std::string;
using std::cout;
using std::ifstream;

//vector<shared_ptr<Shader>> Shader::allShaders;

GLuint shaderTypes[SHADER_MAX] = {
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	GL_GEOMETRY_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER
};

string ShaderNames[SHADER_MAX] = {
	"Vertex",
	"Fragment",
	"Geometry",
	"Tess. Control",
	"Tess. Eval"
};


Shader::Shader(const string& vertex, const string& fragment, const string& geometry, const string& domain, const string& hull, 
				bool transformFeedback, const TransformFeedbackParam& transformFeedbackParam)	{
	shaderFiles[SHADER_VERTEX]		= vertex;
	shaderFiles[SHADER_FRAGMENT]	= fragment;
	shaderFiles[SHADER_GEOMETRY]	= geometry;
	shaderFiles[SHADER_DOMAIN]		= domain;
	shaderFiles[SHADER_HULL]		= hull;

	this->transformFeedback = transformFeedback;
	this->transformFeedbackParam = transformFeedbackParam;

	Reload(false);
	//allShaders.emplace_back(shared_ptr<Shader>(this));
}

Shader::~Shader(void)	{
	DeleteIDs();
}

void	Shader::Reload(bool deleteOld) {
	if(deleteOld) {
		DeleteIDs();
	}

	programID		= glCreateProgram();

	isShaderIncomplete = false;
	for (int i = 0; i < SHADER_MAX; ++i) {
		if (!shaderFiles[i].empty()) {
			if (!GenerateShaderObject(i)) { //If generation failed
				isShaderIncomplete = true;
				break;
			}
		}
		else {
			objectIDs[i]	= 0;
			shaderValid[i]	= 0;
		}
	}
	if (isShaderIncomplete) {
		return;
	}
	SetDefaultAttributes();
	if (transformFeedback)
		SetUpTransformFeedback();
	LinkProgram();
	PrintLinkLog(programID);
}

bool	Shader::LoadShaderFile(const string& filename, string &into)	{
	ifstream	file(SHADERDIR + filename);
	string		textLine;

	cout << "Loading shader text from " << filename << "\n\n";

	if(!file.is_open()){
		cout << "ERROR ERROR ERROR ERROR: File does not exist!\n";
		return false;
	}
	string includeFileName = "";
	int lineNum = 1; 
	while(!file.eof()){
		getline(file,textLine);
		if (CheckIsInclude(textLine, includeFileName)) {
			textLine.clear();
			LoadShaderIncludeFile(includeFileName, into, lineNum);
		}
		else {
			textLine += "\n";
			into += textLine;
			cout << "(" << lineNum << ") :" << textLine;
			++lineNum;
		}
	}
	cout << "\nLoaded shader text!\n\n";
	return true;
}

bool	Shader::CheckIsInclude(const string& line, string& includeFileName)
{
	if (line.substr(0, 8) != "#include")
		return false;
	else {
		size_t quoteFront = line.find_first_of('\"');
		if (quoteFront >= line.size()) {
			return false;
		}
		size_t quoteBack = line.find_first_of('\"', quoteFront + 1);
		if (quoteBack >= line.size()) {
			return false;
		}
		includeFileName = line.substr(quoteFront + 1, quoteBack - quoteFront - 1);
		return true;
	}
}

bool	Shader::LoadShaderIncludeFile(const string& filename, string& into, int& lineNum) {
	ifstream	file(SHADERDIR + filename);
	string		textLine;

	if (!file.is_open()) {
		cout << "ERROR ERROR ERROR ERROR: Include file does not exist! " << filename << "\n";
		return false;
	}
	while (!file.eof()) {
		getline(file, textLine);
		textLine += "\n";
		into += textLine;
		cout << "(" << lineNum << ") :" << textLine;
		++lineNum;
	}
	return true;
}

bool	Shader::GenerateShaderObject(unsigned int i)	{
	cout << "Compiling Shader...\n";

	string shaderText;
	if(!LoadShaderFile(shaderFiles[i],shaderText)) {
		cout << "Loading failed!\n";
		shaderValid[i] = false;
		return false;
	}

	objectIDs[i] = glCreateShader(shaderTypes[i]);	//Create shaderPrePixelBump object and get shaderPrePixelBump object id. 

	const char *chars	= shaderText.c_str();
	int textLength		= (int)shaderText.length();
	glShaderSource(objectIDs[i], 1, &chars, &textLength);	//Fill in(replace) code to a shaderPrePixelBump object. 
	glCompileShader(objectIDs[i]);	//Compile shaderPrePixelBump.

	glGetShaderiv(objectIDs[i], GL_COMPILE_STATUS, &shaderValid[i]);	//Retrieve compiling state. 

	if (!shaderValid[i]) {
		cout << "Compiling failed!\n";
		PrintCompileLog(objectIDs[i]);
	}
	else {
		cout << "Compiling success!\n\n";
	}

	glObjectLabel(GL_SHADER, objectIDs[i], -1, shaderFiles[i].c_str());		//Mark label for debug. 
	glAttachShader(programID, objectIDs[i]);

	if (!shaderValid[i]) {
		return false;
	}
	else
	{
		return true;
	}
}

void Shader::LinkProgram()	{
	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &programValid);
}

void Shader::SetUpTransformFeedback()
{
	glTransformFeedbackVaryings(programID, transformFeedbackParam.GetCount(), transformFeedbackParam.GetFeedbackVarying(), GL_INTERLEAVED_ATTRIBS);
}

void	Shader::SetDefaultAttributes()	{
	glBindAttribLocation(programID, VERTEX_BUFFER,  "position");	//Bind buffer and buffer name in program. 
	glBindAttribLocation(programID, COLOUR_BUFFER,  "colour");
	glBindAttribLocation(programID, NORMAL_BUFFER,  "normal");
	glBindAttribLocation(programID, TANGENT_BUFFER, "tangent");
	glBindAttribLocation(programID, TEXTURE_BUFFER, "texCoord");

	glBindAttribLocation(programID, WEIGHTVALUE_BUFFER, "jointWeights");
	glBindAttribLocation(programID, WEIGHTINDEX_BUFFER, "jointIndices");
}

void	Shader::DeleteIDs() {
	if (!programID) {
		return;
	}
	for (int i = 0; i < SHADER_MAX; ++i) {
		if (objectIDs[i]) {
			glDetachShader(programID, objectIDs[i]);
			glDeleteShader(objectIDs[i]);
		}
	}
	glDeleteProgram(programID);
	programID = 0;
}

void	Shader::PrintCompileLog(GLuint object) {
	int logLength = 0;
	glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength) {
		char* tempData = new char[logLength];
		glGetShaderInfoLog(object, logLength, NULL, tempData);
		std::cout << "Compile Log:\n" << tempData << std::endl;
		delete[] tempData;
	}
}

void	Shader::PrintLinkLog(GLuint program) {
	int logLength = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

	if (logLength) {
		char* tempData = new char[logLength];
		glGetProgramInfoLog(program, logLength, NULL, tempData);
		std::cout << "Link Log:\n" << tempData << std::endl;
		delete[] tempData;
	}
}