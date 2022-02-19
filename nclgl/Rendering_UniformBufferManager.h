#pragma once

#include <set>

#include "KHR\khrplatform.h"
#include "glad\glad.h"

#include "GL/GL.h"
#include "KHR/WGLext.h"

#include "Singleton.h"
#include "Exceptions.h"

#define NCLGL_UBO_MAX_COUNT 36

class Rendering_UniformBufferObject {
public:
	GLuint buffer;
protected:
	friend class Rendering_UniformBufferManager;
	int idBuffer;
};

class Rendering_UniformBufferManager: public Singleton<Rendering_UniformBufferManager>
{
protected:
	enum UBOBufferFlag {
		UBOBufferFlag_Uninit,
		UBOBufferFlag_Available,
		UBOBufferFlag_Occupied,
	};
public:
	bool GetUBO(Rendering_UniformBufferObject& ubo) {
		for (int i = 0; i < NCLGL_UBO_MAX_COUNT; ++i) {
			if (bufferFlags[i] == UBOBufferFlag_Occupied)
				continue;
			else if (bufferFlags[i] == UBOBufferFlag_Uninit) {
				InitBuffer(i);
			}
			bufferFlags[i] = UBOBufferFlag_Occupied;
			ubo.buffer = buffers[i];
			ubo.idBuffer = i;
			return true;
		}
		return false;
	}

	bool ReturnUBO(const Rendering_UniformBufferObject& ubo) {
		if (ubo.idBuffer > NCLGL_UBO_MAX_COUNT) {
			return false;
		}
		if (buffers[ubo.idBuffer] != ubo.buffer || bufferFlags[ubo.idBuffer] != UBOBufferFlag_Occupied) {
			return false;
		}
		bufferFlags[ubo.idBuffer] = UBOBufferFlag_Available;
	}

public:
	Rendering_UniformBufferManager() {
		for (int i = 0; i < NCLGL_UBO_MAX_COUNT; ++i) {
			bufferFlags[i] = UBOBufferFlag_Uninit;
		}
	}

	GLuint buffers[NCLGL_UBO_MAX_COUNT];
	UBOBufferFlag bufferFlags[NCLGL_UBO_MAX_COUNT];

	void InitBuffer(int i) {
		glGenBuffers(1, &buffers[i]);
		bufferFlags[i] = UBOBufferFlag_Available;
	}
};