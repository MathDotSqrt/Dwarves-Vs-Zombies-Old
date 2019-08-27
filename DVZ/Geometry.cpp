#include "Geometry.h"

using namespace Graphics;

Geometry::Geometry() : vbo(GL_ARRAY_BUFFER), ebo(GL_ELEMENT_ARRAY_BUFFER){}

//Geometry::Geometry(VAO vao, int vertexCount) {
//	this->vao = vao;
//	this->indexCount = vertexCount;
//}


Geometry::~Geometry() {
}

VAO& Geometry::getVAO() {
	return vao;
}

int Geometry::getVertexCount() {
	return indexCount;
}
