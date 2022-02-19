#include "HeightMap.h"

HeightMap::HeightMap(const string& name)
{
	int iWidth, iHeight, iChans;
	unsigned char* data = SOIL_load_image(name.c_str(), &iWidth, &iHeight, &iChans, 1);
	if (!data) {
		cout << "Heightmap can't load file! " << name << "\n";
		return;
	}

	numVertices = iWidth * iHeight;
	numIndices = (iWidth - 1) * (iHeight - 1) * 6;
	vertices = new Vector3[numVertices];
	colours = new Vector4[numVertices]; //If to use height colourAndIntensity
	textureCoords = new Vector2[numVertices];

	indices = new GLuint[numIndices];

	Vector3 vertexScale = Vector3(16.0f, 1.0f, 16.0f);
	Vector2 textureScale = Vector2(1 / 16.0f, 1 / 16.0f);

	for (int z = 0; z < iHeight; ++z) {
		for (int x = 0; x < iWidth; ++x) {
			int offset = (z * iWidth) + x;
			vertices[offset] = Vector3(x, data[offset], z) * vertexScale;
			textureCoords[offset] = Vector2(x, z) * textureScale;
			
			colours[offset] = Vector4(1, 1, 1, 1);
		}
	}
	SOIL_free_image_data(data);

	int i = 0;
	for (int z = 0; z < iHeight - 1; ++z) {
		for (int x = 0; x < iWidth - 1; ++x) {
			int a = (z * iWidth) + x;
			int b = (z * iWidth) + x + 1;
			int c = ((z + 1) * iWidth) + x + 1;
			int d = ((z + 1) * iWidth) + x;
			//First triangle
			indices[i++] = a;
			indices[i++] = c;
			indices[i++] = b;
			//Second triangle
			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;
		}
	}
	
	GenerateNormals();
	GenerateTangents();
	BufferData();

	heightmapSize.x = vertexScale.x * (iWidth - 1);
	heightmapSize.y = vertexScale.y * 255.0f;
	heightmapSize.z = vertexScale.z * (iHeight - 1);

	isInit = true;
}
