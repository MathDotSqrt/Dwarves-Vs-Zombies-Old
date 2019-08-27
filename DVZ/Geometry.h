#pragma once
#include <vector>
#include "VAO.h"
#include "VBO.h"

namespace Graphics {

class Geometry {
protected:
	VAO vao;
	VBO vbo;
	VBO ebo;
	int indexCount;
public:
	//Geometry(VAO vao, int vertexCount);
	Geometry();
	~Geometry();

	VAO& getVAO();
	int getVertexCount();
};
}
