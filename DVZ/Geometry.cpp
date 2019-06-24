#include "Geometry.h"

using namespace Graphics;

Geometry::Geometry() {}

Geometry::Geometry(VAO vao, int vertexCount) {
	this->vao = vao;
	this->vertexCount = vertexCount;
}


Geometry::~Geometry() {
}

VAO Geometry::getVAO() {
	return vao;
}

int Geometry::getVertexCount() {
	return vertexCount;
}
