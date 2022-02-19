#include "Geometry.h"

shared_ptr<Mesh> Geometry::GenerateTriangle() {
	shared_ptr<Mesh> m = make_shared<Mesh>();
	m->numVertices = 3;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.0f, 0.5f, 0.0f);
	m->vertices[1] = Vector3(0.5f, -0.5f, 0.0f);
	m->vertices[2] = Vector3(-0.5f, -0.5f, 0.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

	float scale = 1;
	m->textureCoords = new Vector2[m->numVertices];
	m->textureCoords[0] = Vector2(0.5f * scale, 1.0f * scale);
	m->textureCoords[1] = Vector2(1.0f * scale, 0.0f * scale);
	m->textureCoords[2] = Vector2(0.0f * scale, 0.0f * scale);

	m->BufferData();
	return m;
}

shared_ptr<Mesh> Geometry::GenerateQuad(bool generateNormalTangent)
{
	shared_ptr<Mesh> m = make_shared<Mesh>();
	m->numVertices = 4;
	m->numIndices = 6;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(-1.0f, 1.0f, 0.0f);
	m->vertices[1] = Vector3(-1.0f, -1.0f, 0.0f); 
	m->vertices[2] = Vector3(1.0f, 1.0f, 0.0f);
	m->vertices[3] = Vector3(1.0f, -1.0f, 0.0f);

	m->textureCoords = new Vector2[m->numVertices];
	m->textureCoords[0] = Vector2(0.0f, 1.0f);
	m->textureCoords[1] = Vector2(0.0f, 0.0f);
	m->textureCoords[2] = Vector2(1.0f, 1.0f);
	m->textureCoords[3] = Vector2(1.0f, 0.0f);

	m->indices = new unsigned int[m->numIndices];
	m->indices[0] = 0;
	m->indices[1] = 1;
	m->indices[2] = 2;
	m->indices[3] = 3;
	m->indices[4] = 2;
	m->indices[5] = 1;

	m->colours = new Vector4[m->numVertices];
	for (int i = 0; i < m->numVertices; ++i) {
		m->colours[i] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	if (generateNormalTangent) {
		m->normals = new Vector3[m->numVertices];
		m->tangents = new Vector4[m->numVertices];
		for (int i = 0; i < 4; ++i) {
			m -> normals[i] = Vector3(0.0f, 0.0f, 1.0f); 
			m -> tangents[i] = Vector4(1.0f, 0.0f, 0.0f, 1.0f); 
		}
	}

	m->renderMode = GL_TRIANGLES;

	m->BufferData();
	return m;
}

shared_ptr<Mesh> Geometry::GeneratePlane(bool generateNormalTangent)
{
	shared_ptr<Mesh> m = make_shared<Mesh>();
	m->numVertices = 4;
	m->numIndices = 6;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(-1.0f, 0.0f, 1.0f);
	m->vertices[1] = Vector3(-1.0f, 0.0f, -1.0f);
	m->vertices[2] = Vector3(1.0f, 0.0f, 1.0f);
	m->vertices[3] = Vector3(1.0f, 0.0f, -1.0f);

	m->indices = new unsigned int[m->numIndices];
	m->indices[0] = 1;
	m->indices[1] = 0;
	m->indices[2] = 3;
	m->indices[3] = 2;
	m->indices[4] = 3;
	m->indices[5] = 0;

	m->textureCoords = new Vector2[m->numVertices];
	m->textureCoords[0] = Vector2(0.0f, 0.0f);
	m->textureCoords[1] = Vector2(0.0f, 1.0f);
	m->textureCoords[2] = Vector2(1.0f, 0.0f);
	m->textureCoords[3] = Vector2(1.0f, 1.0f);

	m->colours = new Vector4[m->numVertices];
	for (int i = 0; i < m->numVertices; ++i) {
		m->colours[i] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	if (generateNormalTangent) {
		m->normals = new Vector3[m->numVertices];
		m->tangents = new Vector4[m->numVertices];
		for (int i = 0; i < 4; ++i) {
			m->normals[i] = Vector3(0.0f, 1.0f, 0.0f);
			m->tangents[i] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		}
	}

	m->renderMode = GL_TRIANGLES;

	m->BufferData();
	return m;
}

shared_ptr<Mesh> Geometry::GenerateCube()
{
	shared_ptr<Mesh> m = make_shared<Mesh>();
	m->numVertices = 24;
	m->numIndices = 36;

	m->vertices = new Vector3[m->numVertices];
	m->colours = new Vector4[m->numVertices];
	m->textureCoords = new Vector2[m->numVertices];
	m->indices = new GLuint[m->numIndices];
	int idx = 0;
	int idxIndice = 0;
	
	for (float z : {-0.5, 0.5}) {
		for (float y : {-0.5, 0.5}) {
			for (float x : {-0.5, 0.5}) {
				int idx_ = idx++;
				float s = MyMaths::Sign(z);
				m->vertices[idx_] = Vector3(s * x, y, z);
				m->textureCoords[idx_] = Vector2(x + 0.5, y + 0.5);
				m->colours[idx_] = Vector4::one;
			}
		}
		m->indices[idxIndice++] = idx - 2;
		m->indices[idxIndice++] = idx - 3;
		m->indices[idxIndice++] = idx - 1;

		m->indices[idxIndice++] = idx - 3;
		m->indices[idxIndice++] = idx - 2;
		m->indices[idxIndice++] = idx - 4;
	}
	
	for (float x : {-0.5, 0.5}) {
		for (float y : {-0.5, 0.5}) {
			for (float z : {-0.5, 0.5}) {
				int idx_ = idx++;
				float s = MyMaths::Sign(-x);
				m->vertices[idx_] = Vector3(x, y, s * z);
				m->textureCoords[idx_] = Vector2(z + 0.5, y + 0.5);
				m->colours[idx_] = Vector4::one;
			}
		}
		m->indices[idxIndice++] = idx - 2;
		m->indices[idxIndice++] = idx - 3;
		m->indices[idxIndice++] = idx - 1;

		m->indices[idxIndice++] = idx - 3;
		m->indices[idxIndice++] = idx - 2;
		m->indices[idxIndice++] = idx - 4;
	}

	for (float y : {-0.5, 0.5}) {
		for (float z : {0.5, -0.5}) {
			for (float x : {-0.5, 0.5}) {
				int idx_ = idx++;
				float s = MyMaths::Sign(y);
				m->vertices[idx_] = Vector3(x, y, s * z);
				m->textureCoords[idx_] = Vector2(x + 0.5, - z + 0.5);
				m->colours[idx_] = Vector4::one;
			}
		}
		m->indices[idxIndice++] = idx - 2;
		m->indices[idxIndice++] = idx - 3;
		m->indices[idxIndice++] = idx - 1;

		m->indices[idxIndice++] = idx - 3;
		m->indices[idxIndice++] = idx - 2;
		m->indices[idxIndice++] = idx - 4;
	}

	m->BufferData();
	return m;
}

shared_ptr<Mesh> Geometry::GenerateGrid(float d, int size)
{
	shared_ptr<Mesh> m = make_shared<Mesh>();
	m->renderMode = GL_LINES;

	m->numVertices = (size + 1) * 4;
	m->vertices = new Vector3[m->numVertices];

	int vertexPerEdge = size + 1;
	float vertexScale = d;
	float cent = size / 2.0f;

	int i = 0;
	for (int x = 0; x < vertexPerEdge; ++x) {
		m->vertices[i++] = Vector3(x - cent, 0, -cent) * vertexScale;
		m->vertices[i++] = Vector3(x - cent, 0, +cent) * vertexScale;
	}
	for (int z = 0; z < vertexPerEdge; ++z) {
		m->vertices[i++] = Vector3(-cent, 0, z - cent) * vertexScale;
		m->vertices[i++] = Vector3(+cent, 0, z - cent) * vertexScale;
	}

	m->BufferData();
	return m;
}

shared_ptr<Mesh> Geometry::GenerateXYZ()
{
	shared_ptr<Mesh> m = make_shared<Mesh>();
	m->renderMode = GL_LINES;

	m->numVertices = 6;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0, 0, 0);
	m->vertices[1] = Vector3(1, 0, 0);
	m->vertices[2] = Vector3(0, 0, 0);
	m->vertices[3] = Vector3(0, 1, 0);
	m->vertices[4] = Vector3(0, 0, 0);
	m->vertices[5] = Vector3(0, 0, 1);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1, 0, 0, 0);
	m->colours[1] = Vector4(1, 0, 0, 0);
	m->colours[2] = Vector4(0, 1, 0, 0);
	m->colours[3] = Vector4(0, 1, 0, 0);
	m->colours[4] = Vector4(0, 0, 1, 0);
	m->colours[5] = Vector4(0, 0, 1, 0);

	m->BufferData();
	return m;
}

shared_ptr<Mesh> Geometry::GenerateQuad1()
{
	shared_ptr<Mesh> m = make_shared<Mesh>();
	m->numVertices = 4;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.5f, 0.5f, 0.0f);
	m->vertices[1] = Vector3(0.5f, -0.5f, 0.0f);
	m->vertices[2] = Vector3(-0.5f, -0.5f, 0.0f);
	m->vertices[3] = Vector3(-0.5f, 0.5f, 0.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	m->colours[3] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	m->numIndices = 6;
	m->indices = new unsigned int[m->numIndices];
	m->indices[0] = 0;
	m->indices[1] = 1;
	m->indices[2] = 2;
	m->indices[3] = 0;
	m->indices[4] = 2;
	m->indices[5] = 3;

	m->BufferData();
	return m;
}


shared_ptr<Mesh> Geometry::GenerateTest()
{
	shared_ptr<Mesh> m = make_shared<Mesh>();
	m->numVertices = 4;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.5f, 0.5f, 0.0f);
	m->vertices[1] = Vector3(0.5f, -0.5f, 0.0f);
	m->vertices[2] = Vector3(-0.5f, -0.5f, 0.0f);
	m->vertices[3] = Vector3(-0.5f, 0.5f, 0.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	m->colours[3] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	m->numIndices = 6;
	m->indices = new unsigned int[m->numIndices];
	m->indices[0] = 0;
	m->indices[1] = 1;
	m->indices[2] = 2;
	m->indices[3] = 0;
	m->indices[4] = 2;
	m->indices[5] = 3;

	m->renderMode = GL_LINES;

	m->BufferData();
	return m;
}

void Geometry::GenerateRandomQuad1ColorArray(std::vector<Vector4> & colours, int size)
{
	for (int i = 0; i < size; ++i) {
		(colours).emplace_back(MyMaths::RandomFloat01(), MyMaths::RandomFloat01(), MyMaths::RandomFloat01(), 1.0f);
	}
}


