#pragma once

#include <memory>

#include "Mesh.h"
#include "Geometry.h"

using namespace std;

class ScreenQuad
{
public:
	static shared_ptr<Mesh> GetScreenQuad() {
		if (!screenQuad) {
			screenQuad = Geometry::GenerateQuad(true);
		}
		if (!screenQuad)
			throw NullPtrException();
		return screenQuad;
	}
protected:
	static shared_ptr<Mesh> screenQuad;
};

