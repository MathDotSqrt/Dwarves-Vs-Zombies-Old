
#include "QuadGeometry.h"

using namespace Graphics;

Geometry<PositionAttrib, ColorAttrib, NormalAttrib, TexcoordAttrib> Graphics::CreateQuad() {
	typedef Geometry<PositionAttrib, ColorAttrib, NormalAttrib, TexcoordAttrib> QuadGeometry;
	
	PositionAttrib p;
	ColorAttrib c;
	NormalAttrib n;
	TexcoordAttrib t;

	QuadGeometry g(p, c, n, t);
	QuadGeometry::GeometryVertex v(glm::vec3(-1, 1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, -1), glm::vec2(0, 0));
	g.pushVertex(glm::vec3(-1, 1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, -1), glm::vec2(0, 0));
	g.pushVertex(glm::vec3(1, 1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec2(1, 0));
	g.pushVertex(glm::vec3(1, -1, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, -1), glm::vec2(1, 1));
	g.pushVertex(glm::vec3(-1, -1, 0), glm::vec3(1, 1, 1), glm::vec3(0, 0, -1), glm::vec2(0, 1));


	//g.pushVertex();
	g.pushTriangle(0, 1, 2);
	g.pushTriangle(0, 2, 3);
	return g;
}

//#include "QuadGeometry.h"
//#include "preamble.glsl"
//#include "macrologger.h"
//
//using namespace Graphics;
////todo make a static geometry creator
//QuadGeometry::QuadGeometry(){
//	this->init();
//}
//
//
//QuadGeometry::~QuadGeometry() {
//}
//
//void QuadGeometry::init() {
//	//float verticies[] = {
//	//	-1, 1, 0,  1, 0, 0,  0, 0, -1,  0, 0,
//	//	1, 1, 0,   0, 1, 0,  0, 0, -1,  1, 0,
//	//	1, -1, 0,  0, 0, 1,  0, 0, -1,  1, 1,
//	//	-1, -1, 0, 1, 1, 1,  0, 0, -1,  0, 1
//	//};
//	//
//	//GLuint elements[] = {
//	//	0,1,2,
//	//	0,2,3
//	//};
//
//	QuadVertex v0 = { -1, 1, 0,  1, 0, 0,  0, 0, -1,  0, 0 };
//	QuadVertex v1 = { 1, 1, 0,   0, 1, 0,  0, 0, -1,  1, 0 };
//	QuadVertex v2 = { 1, -1, 0,  0, 0, 1,  0, 0, -1,  1, 1 };
//	QuadVertex v3 = { -1, -1, 0, 1, 1, 1,  0, 0, -1,  0, 1 };
//
//	this->verticies.push_back(v0);
//	this->verticies.push_back(v1);
//	this->verticies.push_back(v2);
//	this->verticies.push_back(v3);
//
//	this->indices.push_back(0);
//	this->indices.push_back(1);
//	this->indices.push_back(2);
//	this->indices.push_back(0);
//	this->indices.push_back(2);
//	this->indices.push_back(3);
//
//	/*this->vao.bind();
//
//	VBO vbo(GL_ARRAY_BUFFER);
//	vbo.bind();
//	vbo.bufferData(sizeof(verticies), verticies, GL_STATIC_DRAW);
//	glVertexAttribPointer(POSITION_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
//	glVertexAttribPointer(COLOR_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
//	glVertexAttribPointer(NORMAL_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
//	glVertexAttribPointer(TEXCOORD_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
//	vbo.unbind();
//
//	VBO ebo(GL_ELEMENT_ARRAY_BUFFER);
//	ebo.bind();
//	ebo.bufferData(sizeof(elements), elements, GL_STATIC_DRAW);
//	
//	this->vao.unbind();
//	ebo.unbind();
//	
//
//	this->indexCount = sizeof(elements)/sizeof(*elements);
//
//	LOG_INFO("%d", this->indexCount);*/
//}
//
//

