#include "Mesh.h"
#include "Matrix2.h"

using std::string;

Mesh::Mesh(void)	{
	glGenVertexArrays(1, &arrayObject);		//Generate identifier identifying a new part of GPU mem. 
	
	for(int i = 0; i < MAX_BUFFER; ++i) {
		bufferObject[i] = 0;
	}

	numVertices  = 0;
	renderMode		 = GL_TRIANGLES;

	numIndices		= 0;
	vertices		= nullptr;
	textureCoords	= nullptr;
	normals			= nullptr;
	tangents		= nullptr;
	indices			= nullptr;
	colours			= nullptr;
	weights			= nullptr;
	weightIndices	= nullptr;
}

Mesh::~Mesh(void)	{
	glDeleteVertexArrays(1, &arrayObject);			//Delete our VAO
	glDeleteBuffers(MAX_BUFFER, bufferObject);		//Delete our VBOs

	delete[]	vertices;
	delete[]	indices;
	delete[]	textureCoords;
	delete[]	tangents;
	delete[]	normals;
	delete[]	colours;
	delete[]	weights;
	delete[]	weightIndices;
}

void Mesh::Draw()	{
	/*glShadeModel(shadeMode);*/
	glBindVertexArray(arrayObject);		//Bind buffer (for accessing).
	if(bufferObject[INDEX_BUFFER]) {
		glDrawElements(renderMode, numIndices, GL_UNSIGNED_INT, 0);
	}
	else{
		glDrawArrays(renderMode, 0, numVertices);
	}
	glBindVertexArray(0);	//Close buffer.
}

void Mesh::DrawSubMesh(int i) {
	if (i < 0 || i >= (int)meshLayers.size()) {
		return;
	}
	SubMesh m = meshLayers[i];

	glBindVertexArray(arrayObject);
	if (bufferObject[INDEX_BUFFER]) {
		const GLvoid* offset = (const GLvoid * )(m.start * sizeof(unsigned int)); 
		glDrawElements(renderMode, m.count, GL_UNSIGNED_INT, offset);
	}
	else {
		glDrawArrays(renderMode, m.start, m.count);	//Draw the triangle!
	}
	glBindVertexArray(0);
}

void Mesh::DrawInstanced(int count)
{
	glBindVertexArray(arrayObject);		//Bind buffer (for accessing).
	if (bufferObject[INDEX_BUFFER]) {
		glDrawElementsInstanced(renderMode, numIndices, GL_UNSIGNED_INT, 0, count);
	}
	else {
		glDrawArraysInstanced(renderMode, 0, numVertices, count);
	}
	glBindVertexArray(0);	//Close buffer.
}

void Mesh::DrawSubMeshInstanced(int i, int count)
{
	if (i < 0 || i >= (int)meshLayers.size()) {
		return;
	}
	SubMesh m = meshLayers[i];

	glBindVertexArray(arrayObject);
	if (bufferObject[INDEX_BUFFER]) {
		const GLvoid* offset = (const GLvoid*)(m.start * sizeof(unsigned int));
		glDrawElementsInstanced(renderMode, m.count, GL_UNSIGNED_INT, offset, count);
	}
	else {
		glDrawArraysInstanced(renderMode, m.start, m.count, count);
	}
	glBindVertexArray(0);
}

void UploadAttribute(GLuint* id, int numElements, int dataSize, int attribSize, int attribID, void* pointer, const string&debugName) {
	//
	//A set of default operations required for uploading. (Operating on VBO in VAO)
	//
	glGenBuffers(1, id);	//Create new mem on GPU. Get buffer id. 
	glBindBuffer(GL_ARRAY_BUFFER, *id);		//Bind buffer id. 
	glBufferData(GL_ARRAY_BUFFER, numElements * dataSize, pointer, GL_STATIC_DRAW);		//Upload data to the buffer which the buffer id is identifying. 

	glVertexAttribPointer(attribID, attribSize, GL_FLOAT, GL_FALSE, 0, 0);		//Define buffer format. 
	glEnableVertexAttribArray(attribID);	//Enable buffer. 

	glObjectLabel(GL_BUFFER, *id, -1, debugName.c_str());		//Mark label for debug. 
}

void Mesh::GenerateNormals()
{
	if (!normals) {
		normals = new Vector3[numVertices];
	}
	for (GLuint i = 0; i < numVertices; ++i) {
		normals[i] = Vector3::zero;
	}
	int triCount = GetTriCount();

	for (int i = 0; i < triCount; ++i) {
		unsigned int a = 0;
		unsigned int b = 0;
		unsigned int c = 0;
		GetVertexIndicesForTri(i, a, b, c);
		Vector3 normal = Vector3::Cross((vertices[b] - vertices[a]), (vertices[c] - vertices[a]));

		normals[a] += normal;
		normals[b] += normal;
		normals[c] += normal;
	}
	for (GLuint i = 0; i < numVertices; ++i) {
		normals[i].Normalise();
	}
}

bool Mesh::GetVertexIndicesForTri(unsigned int i, unsigned int& a, unsigned int& b, unsigned int& c) const
{
	//Here renderMode Must be GL_TRIANGLES!
	if (renderMode != GL_TRIANGLES) {
		throw NotImplementedException();
	}
	unsigned int triCount = GetTriCount();
	if (i >= triCount) {
		return false;
	}
	if (numIndices > 0) {
		a = indices[(i * 3)];
		b = indices[(i * 3)+1];
		c = indices[(i * 3) + 2];
	}
	else
	{
		a = i * 3;
		b = i * 3 + 1;
		c = i * 3 + 2;
	}
	return true;
}

void Mesh::GenerateTangents()
{
	if (!textureCoords) {
		return;
	}
	if (!tangents) {
		tangents = new Vector4[numVertices];
	}
	for (GLuint i = 0; i < numVertices; ++i) {
		tangents[i] = Vector4(0, 0, 0, 0);
	}

	int triCount = GetTriCount();
	for (int i = 0; i < triCount; ++i) {
		unsigned int a = 0;
		unsigned int b = 0;
		unsigned int c = 0;
		GetVertexIndicesForTri(i, a, b, c);
		Vector4 tangent = GenerateTangent(a, b, c);
		tangents[a] += tangent;
		tangents[b] += tangent;
		tangents[c] += tangent;
	}

	for (GLuint i = 0; i < numVertices; ++i) {
		float handedness = tangents[i].w > 0.0f ? 1.0f: -1.0f;
		tangents[i].w = 0.0f;
		tangents[i].Normalise();
		tangents[i].w = handedness;
	}
}

Vector4 Mesh::GenerateTangent(int a, int b, int c)
{
	Vector3 ba = vertices[b] - vertices[a];
	Vector3 ca = vertices[c] - vertices[a];

	Vector2 tba = textureCoords[b] - textureCoords[a];
	Vector2 tca = textureCoords[c] - textureCoords[a];

	Matrix2 texMatrix = Matrix2(tba, tca);
	texMatrix.Invert();

	Vector3 tangent, binormal;
	tangent = ba * texMatrix.values[0] + ca * texMatrix.values[1];
	binormal = ba * texMatrix.values[2] + ca * texMatrix.values[3];

	Vector3 normal = Vector3::Cross(ba, ca);
	Vector3 biCross = Vector3::Cross(normal, tangent);
	float handedness = 1.0f;
	if (Vector3::Dot(biCross, binormal) < 0.0f) {
		handedness = -1.0f;
	}
	return Vector4(tangent, handedness);
}

void	Mesh::BufferData()	{
	glBindVertexArray(arrayObject);		//Bind(open up) buffer (for changing). 

	//
	//Modify buffer.
	//

	////Buffer vertex data
	UploadAttribute(&bufferObject[VERTEX_BUFFER], numVertices, sizeof(Vector3), 3, VERTEX_BUFFER, vertices, "Positions");

	if(textureCoords) {	//Buffer textureTest0 data
		UploadAttribute(&bufferObject[TEXTURE_BUFFER], numVertices, sizeof(Vector2), 2, TEXTURE_BUFFER, textureCoords, "TexCoords");
	}

	if (colours) {
		UploadAttribute(&bufferObject[COLOUR_BUFFER], numVertices, sizeof(Vector4), 4, COLOUR_BUFFER, colours, "Colours");
	}

	if (normals) {	//Buffer normal data
		UploadAttribute(&bufferObject[NORMAL_BUFFER], numVertices, sizeof(Vector3), 3, NORMAL_BUFFER, normals, "Normals");
	}

	if (tangents) {	//Buffer tangent data
		UploadAttribute(&bufferObject[TANGENT_BUFFER], numVertices, sizeof(Vector4), 4, TANGENT_BUFFER, tangents, "Tangents");
	}

	if (weights) {		//Buffer weights data
		UploadAttribute(&bufferObject[WEIGHTVALUE_BUFFER], numVertices, sizeof(Vector4), 4, WEIGHTVALUE_BUFFER, weights, "Weights");
	}

	//Buffer weight indices data...uses a different function since its integers...
	if (weightIndices) {
		glGenBuffers(1, &bufferObject[WEIGHTINDEX_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[WEIGHTINDEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(int) * 4, weightIndices, GL_STATIC_DRAW);
		glVertexAttribIPointer(WEIGHTINDEX_BUFFER, 4, GL_INT, 0, 0); //note the new function...
		glEnableVertexAttribArray(WEIGHTINDEX_BUFFER);

		glObjectLabel(GL_BUFFER, bufferObject[WEIGHTINDEX_BUFFER], -1, "Weight Indices");
	}

	//buffer index data
	if(indices) {
		glGenBuffers(1, &bufferObject[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(GLuint), indices, GL_STATIC_DRAW);

		glObjectLabel(GL_BUFFER, bufferObject[INDEX_BUFFER], -1, "Indices");
	}
	glBindVertexArray(0);		//Close buffer. 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::ChangeBufferData(MeshBuffer buffer, void* ptBufferContent, int bufferSize)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[buffer]);
	void* ptBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	int elementSize = 0;
	switch (buffer)
	{
	case VERTEX_BUFFER: 
		elementSize = sizeof(Vector3);
		break;
	case COLOUR_BUFFER: 
		elementSize = sizeof(Vector4);
		break;
	case TEXTURE_BUFFER:
		elementSize = sizeof(Vector2);
		break;
	case NORMAL_BUFFER:
		elementSize = sizeof(Vector3);
		break;
	case TANGENT_BUFFER:
		elementSize = sizeof(Vector4);
		break;
	case WEIGHTVALUE_BUFFER:
		elementSize = sizeof(Vector4);
		break;
	case WEIGHTINDEX_BUFFER:
		elementSize = sizeof(int);
		break;
	case INDEX_BUFFER:
		elementSize = sizeof(unsigned int);
		break;
	case MAX_BUFFER:
		break;
	default:
		break;
	}
	memcpy(ptBuffer, ptBufferContent, elementSize * bufferSize);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

/*
* 
* Extra file loading stuff!
* 
* */

enum class GeometryChunkTypes {
	VPositions		= 1,
	VNormals		= 2,
	VTangents		= 4,
	VColors			= 8,
	VTex0			= 16,
	VTex1			= 32,
	VWeightValues	= 64,
	VWeightIndices	= 128,
	Indices			= 256,
	JointNames		= 512,
	JointParents	= 1024,
	BindPose		= 2048,
	BindPoseInv		= 4096,
	Material		= 65536,
	SubMeshes		= 1 << 14,
	SubMeshNames	= 1 << 15
};

void ReadTextFloats(std::ifstream& file, vector<Vector2>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		Vector2 temp;
		file >> temp.x;
		file >> temp.y;
		element.emplace_back(temp);
	}
}

void ReadTextFloats(std::ifstream& file, vector<Vector3>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		Vector3 temp;
		file >> temp.x;
		file >> temp.y;
		file >> temp.z;
		element.emplace_back(temp);
	}
}

void ReadTextFloats(std::ifstream& file, vector<Vector4>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		Vector4 temp;
		file >> temp.x;
		file >> temp.y;
		file >> temp.z;
		file >> temp.w;
		element.emplace_back(temp);
	}
}

void ReadTextVertexIndices(std::ifstream& file, vector<int>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		int indices[4];
		file >> indices[0];
		file >> indices[1];
		file >> indices[2];
		file >> indices[3];
		element.emplace_back(indices[0]);
		element.emplace_back(indices[1]);
		element.emplace_back(indices[2]);
		element.emplace_back(indices[3]);
	}
}

void ReadIndices(std::ifstream& file, vector<unsigned int>& elements, int numIndices) {
	for (int i = 0; i < numIndices; ++i) {
		unsigned int temp;
		file >> temp;
		elements.emplace_back(temp);
	}
}

void ReadJointParents(std::ifstream& file, vector<int>& dest) {
	int jointCount = 0;
	file >> jointCount;

	for (int i = 0; i < jointCount; ++i) {
		int id = -1;
		file >> id;
		dest.emplace_back(id);
	}
}

void ReadJointNames(std::ifstream& file, vector<string>& dest) {
	int jointCount = 0;
	file >> jointCount;
	for (int i = 0; i < jointCount; ++i) {
		std::string jointName;
		file >> jointName;
		dest.emplace_back(jointName);
	}
}

void ReadRigPose(std::ifstream& file, Matrix4** into) {
	int matCount = 0;
	file >> matCount;

	*into = new Matrix4[matCount];

	for (int i = 0; i < matCount; ++i) {
		Matrix4 mat;
		for (int i = 0; i < 16; ++i) {
			file >> mat.values[i];
		}
		(*into)[i] = mat;
	}
}

void ReadSubMeshes(std::ifstream& file, int count, vector<Mesh::SubMesh> & subMeshes) {
	for (int i = 0; i < count; ++i) {
		Mesh::SubMesh m;
		file >> m.start;
		file >> m.count;
		subMeshes.emplace_back(m);
	}
}

void ReadSubMeshNames(std::ifstream& file, int count, vector<string>& names) {
	std::string scrap;
	std::getline(file, scrap);

	for (int i = 0; i < count; ++i) {
		std::string meshName;
		std::getline(file, meshName);
		names.emplace_back(meshName);
	}
}

bool Mesh::LoadFromMeshFile(const string& name) {
	string path = MESHDIR + name;
	std::ifstream file(path);
	if (!file) {
		std::cout << "File does not exist!" << std::endl;
		return false;
	}

	std::string filetype;
	int fileVersion;

	file >> filetype;

	if (filetype != "MeshGeometry") {
		std::cout << "File is not a MeshGeometry file!" << std::endl;
		return false;
	}

	file >> fileVersion;

	if (fileVersion != 1) {
		std::cout << "MeshGeometry file has incompatible version!" << std::endl;
		return false;
	}

	int numMeshes	= 0; //read	//number of submeshes
	int numVertices = 0; //read	//number of vertices
	int numIndices	= 0; //read	//number of indices
	int numChunks	= 0; //read	//number of data chunks (for different data: vertex, index, normal, ...)

	file >> numMeshes;
	file >> numVertices;
	file >> numIndices;
	file >> numChunks;

	vector<Vector3> readPositions;
	vector<Vector4> readColours;
	vector<Vector3> readNormals;
	vector<Vector4> readTangents;
	vector<Vector2> readUVs;
	vector<Vector4> readWeights;
	vector<int> readWeightIndices;

	vector<unsigned int>		readIndices;

	for (int i = 0; i < numChunks; ++i) {
		int chunkType = (int)GeometryChunkTypes::VPositions;

		file >> chunkType;

		switch ((GeometryChunkTypes)chunkType) {
		case GeometryChunkTypes::VPositions:ReadTextFloats(file, readPositions, numVertices);  break;
		case GeometryChunkTypes::VColors:	ReadTextFloats(file, readColours, numVertices);  break;
		case GeometryChunkTypes::VNormals:	ReadTextFloats(file, readNormals, numVertices);  break;
		case GeometryChunkTypes::VTangents:	ReadTextFloats(file, readTangents, numVertices);  break;
		case GeometryChunkTypes::VTex0:		ReadTextFloats(file, readUVs, numVertices);  break;
		case GeometryChunkTypes::Indices:	ReadIndices(file, readIndices, numIndices); break;

		case GeometryChunkTypes::VWeightValues:		ReadTextFloats(file, readWeights, numVertices);  break;
		case GeometryChunkTypes::VWeightIndices:	ReadTextVertexIndices(file, readWeightIndices, numVertices);  break;
		case GeometryChunkTypes::JointNames:		ReadJointNames(file, this->jointNames);  break;
		case GeometryChunkTypes::JointParents:		ReadJointParents(file, this->jointParents);  break;
		case GeometryChunkTypes::BindPose:			ReadRigPose(file, &this->bindPose);  break;
		case GeometryChunkTypes::BindPoseInv:		ReadRigPose(file, &this->inverseBindPose);  break;
		case GeometryChunkTypes::SubMeshes: 		ReadSubMeshes(file, numMeshes, this->meshLayers); break;
		case GeometryChunkTypes::SubMeshNames: 		ReadSubMeshNames(file, numMeshes, this->layerNames); break;
		}
	}
	//Now that the data has been read, we can shove it into the actual Mesh object

	this->numVertices	= numVertices;
	this->numIndices	= numIndices;

	if (!readPositions.empty()) {
		this->vertices = new Vector3[numVertices];
		memcpy(this->vertices, readPositions.data(), numVertices * sizeof(Vector3));
	}

	if (!readColours.empty()) {
		this->colours = new Vector4[numVertices];
		memcpy(this->colours, readColours.data(), numVertices * sizeof(Vector4));
	}

	if (!readNormals.empty()) {
		this->normals = new Vector3[numVertices];
		memcpy(this->normals, readNormals.data(), numVertices * sizeof(Vector3));
	}

	if (!readTangents.empty()) {
		this->tangents = new Vector4[numVertices];
		memcpy(this->tangents, readTangents.data(), numVertices * sizeof(Vector4));
	}

	if (readNormals.empty() && readTangents.empty()) {
		GenerateNormals();
		GenerateTangents();
	}

	if (!readUVs.empty()) {
		this->textureCoords = new Vector2[numVertices];
		memcpy(this->textureCoords, readUVs.data(), numVertices * sizeof(Vector2));
	}
	if (!readIndices.empty()) {
		this->indices = new unsigned int[numIndices];
		memcpy(this->indices, readIndices.data(), numIndices * sizeof(unsigned int));
	}

	if (!readWeights.empty()) {
		this->weights = new Vector4[numVertices];
		memcpy(this->weights, readWeights.data(), numVertices * sizeof(Vector4));
	}

	if (!readWeightIndices.empty()) {
		this->weightIndices = new int[numVertices * 4];
		memcpy(this->weightIndices, readWeightIndices.data(), numVertices * sizeof(int) * 4);
	}

	this->BufferData();
	return true;
}

shared_ptr<Mesh> Mesh::LoadMeshFromMeshFile(const std::string& name)
{
	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->LoadFromMeshFile(name);
	return mesh;
}

int Mesh::GetIndexForJoint(const std::string& name) const {
	for (unsigned int i = 0; i < jointNames.size(); ++i) {
		if (jointNames[i] == name) {
			return i;
		}
	}
	return -1;
}

int Mesh::GetParentForJoint(const std::string& name) const {
	int i = GetIndexForJoint(name);
	if (i == -1) {
		return -1;
	}
	return jointParents[i];
}

int Mesh::GetParentForJoint(int i) const {
	if (i == -1 || i >= (int)jointParents.size()) {
		return -1;
	}
	return jointParents[i];
}

bool Mesh::GetSubMesh(int i, const SubMesh* s) const {
	if (i < 0 || i >= (int)meshLayers.size()) {
		return false;
	}
	s = &meshLayers[i];
	return true;
}

bool Mesh::GetSubMesh(const string& name, const SubMesh* s) const {
	for (unsigned int i = 0; i < layerNames.size(); ++i) {
		if (layerNames[i] == name) {
			return GetSubMesh(i, s);
		}
	}
	return false;
}

bool Mesh::GetSubMeshName(int i, string& nameOut)
{
	if (i < 0 || i >= layerNames.size()) {
		return false;
	}
	nameOut = layerNames[i];
	return true; 
}
