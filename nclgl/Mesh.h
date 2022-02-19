/******************************************************************************
Class:Mesh
Implements:
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:Wrapper around OpenGL primitives, geometry and related 
OGL functions.

There's a couple of extra functions in here that you didn't get in the tutorial
series, to draw debug normals and tangents. 


-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "OGLRenderer.h"
#include <vector>
#include <string>
#include <memory>

#include "Exceptions.h"

using namespace std;

//A handy enumerator, to determine which member of the bufferObject array
//holds which data
enum MeshBuffer {
	VERTEX_BUFFER	,
	COLOUR_BUFFER	, 
	TEXTURE_BUFFER	,
	NORMAL_BUFFER	, 
	TANGENT_BUFFER	,

	WEIGHTVALUE_BUFFER,		//new this year, weight values of vertices
	WEIGHTINDEX_BUFFER,	//new this year, indices of weights

	INDEX_BUFFER	,

	MAX_BUFFER
};

class Mesh {
public:	
	struct SubMesh {
		int start;
		int count;
	};

	Mesh(void);
	virtual ~Mesh(void);

	void Draw();
	void DrawSubMesh(int i);
	void DrawInstanced(int count);
	void DrawSubMeshInstanced(int i, int count);

	bool LoadFromMeshFile(const std::string& name);
	static shared_ptr<Mesh> LoadMeshFromMeshFile(const std::string& name);

	inline unsigned int GetTriCount() const {
		int primCount = indices ? numIndices : numVertices;
		return primCount / 3;
	}

	inline unsigned int GetJointCount() const {
		return (unsigned int)jointNames.size();
	}


	int GetIndexForJoint(const std::string& name) const;
	int GetParentForJoint(const std::string& name) const;
	int GetParentForJoint(int i) const;

	inline const Matrix4* GetBindPose() const {
		return bindPose;
	}

	inline const Matrix4* GetInverseBindPose() const {
		return inverseBindPose;
	}
	
	//If return 0, this is one big mesh. 
	inline int		GetSubMeshCount() const {
		return (int)meshLayers.size(); 
	}

	bool GetSubMesh(int i, const SubMesh* s) const;
	bool GetSubMesh(const std::string& name, const SubMesh* s) const;
	bool GetSubMeshName(int i, string& nameOut);

	void ChangeBufferData(MeshBuffer buffer, void* ptBufferContent, int bufferSize);
	inline Vector3* GetVertices() const { return vertices; }

	inline GLuint GetNumVertices() const { return numVertices; }
	inline GLuint GetNumIndices() const { return numIndices; }
	inline GLuint GetNumVerticesToShader(int i) {
		if (i < 0) {
			if (bufferObject[INDEX_BUFFER]) {
				return numIndices;
			}
			else
			{
				return numVertices;
			}
		}
		else if (i<meshLayers.size())
		{
			return meshLayers[i].count;
		}
	}

	inline GLuint GetRenderMode()const { return renderMode; }
	inline void SetRenderMode(GLuint renderMode) { this->renderMode = renderMode; }

	void GenerateNormals();
	bool GetVertexIndicesForTri(unsigned int i, unsigned int& a, unsigned int& b, unsigned int& c) const;

	void GenerateTangents();
	Vector4 GenerateTangent(int a, int b, int c);

	friend class Geometry;
	void BufferData();

protected:

	GLuint	arrayObject;

	GLuint	bufferObject[MAX_BUFFER];	//Containing ids of all buffers. 

	GLuint	numVertices = 0;
	GLuint	numIndices = 0;
	
	GLuint	renderMode;

	//GLuint shadeMode;

	Vector3*		vertices = nullptr;
	Vector4*		colours = nullptr;
	Vector2*		textureCoords = nullptr;
	Vector3*		normals = nullptr;
	Vector4*		tangents = nullptr;

	Vector4*		weights = nullptr;
	int*			weightIndices = nullptr;

	unsigned int*	indices = nullptr;

	Matrix4* bindPose = nullptr;
	Matrix4* inverseBindPose = nullptr;	//Matrices used to inverse model from bind pose to normal pose.

	std::vector<std::string>	jointNames;
	std::vector<int>			jointParents;
	std::vector<SubMesh>		meshLayers;
	std::vector<std::string>	layerNames;
};

