#pragma once
#include <vector>
#include "VAO.h"
#include "VBO.h"


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

