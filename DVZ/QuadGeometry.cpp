#include "QuadGeometry.h"
#include "preamble.glsl"
#include "macrologger.h"

using namespace Graphics;

QuadGeometry::QuadGeometry() {
	this->init();
}


QuadGeometry::~QuadGeometry() {
}

void QuadGeometry::init() {
	float verticies[] = {
		-1, 1, 0,  1, 0, 0,  0, 0, -1,  0, 0,
		1, 1, 0,   0, 1, 0,  0, 0, -1,  1, 0,
		1, -1, 0,  0, 0, 1,  0, 0, -1,  1, 1,
		-1, -1, 0, 1, 1, 1,  0, 0, -1,  0, 1
	};
	
	GLuint elements[] = {
		0,1,2,
		0,2,3
	};

	this->vao.bind();

	VBO vbo(GL_ARRAY_BUFFER);
	vbo.bind();
	vbo.bufferData(sizeof(verticies), verticies, GL_STATIC_DRAW);
	glVertexAttribPointer(POSITION_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glVertexAttribPointer(COLOR_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(NORMAL_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glVertexAttribPointer(TEXCOORD_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	vbo.unbind();

	VBO ebo(GL_ELEMENT_ARRAY_BUFFER);
	ebo.bind();
	ebo.bufferData(sizeof(elements), elements, GL_STATIC_DRAW);
	
	this->vao.unbind();
	ebo.unbind();
	

	this->vertexCount = sizeof(elements)/sizeof(*elements);

	LOG_INFO("%d", this->vertexCount);
}


