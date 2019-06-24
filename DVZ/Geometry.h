#pragma once
#include <vector>
#include "VAO.h"
#include "VBO.h"

namespace Graphics {

class Geometry {
protected:
	Geometry();
	VAO vao;
	int vertexCount;

public:
	Geometry(VAO vao, int vertexCount);
	~Geometry();

	VAO getVAO();
	int getVertexCount();
};
}
