#include "CubeGeometry.h"
#include "preamble.glsl"

using namespace Graphics;

CubeGeometry::CubeGeometry() {
	this->init();
}


CubeGeometry::~CubeGeometry(){
}

void CubeGeometry::init() {
	float verticies[24] = {  // Coordinates for the vertices of a cube.
		   1,1,1,   1,1,-1,   1,-1,-1,   1,-1,1,
		  -1,1,1,  -1,1,-1,  -1,-1,-1,  -1,-1,1 
	};

	int elements[24] = {  // Vertex number for the six faces.
		  0,1,2,3, 0,3,7,4, 0,4,5,1,
		  6,2,1,5, 6,5,4,7, 6,7,3,2 
	};

	this->vao.bind();

	VBO vbo(GL_ARRAY_BUFFER);
	vbo.bufferData(sizeof(verticies), verticies, GL_STATIC_DRAW);
	vbo.bind();

	VBO ebo(GL_ELEMENT_ARRAY_BUFFER);
	ebo.bufferData(sizeof(elements), elements, GL_STATIC_DRAW);
	glVertexAttribPointer(POSITION_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	ebo.bind();
	vbo.unbind();
}
