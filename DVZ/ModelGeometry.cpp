//#define TINYOBJLOADER_IMPLEMENTATION //for tinyobj
#include "ModelGeometry.h"
#include "OBJLoader.h"
#include "VBO.h"
#include "preamble.glsl"


using namespace Graphics;
using namespace std;

ModelGeometry::ModelGeometry(std::string filename){
	Loader::OBJLoader loader;
	loader.LoadFile(filename);
	vector<Loader::Vertex> verticies = loader.LoadedVertices;
	vector<GLuint> indicies = loader.LoadedIndices;

	/*this->vao.bind();
	this->vbo.bind();
	this->vbo.bufferData((sizeof(Loader::Vertex) * verticies.size()), verticies.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(POSITION_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Loader::Vertex), (void*)0);
	glVertexAttribPointer(NORMAL_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Loader::Vertex), (void*)(1 * sizeof(Loader::Vector3)));
	glVertexAttribPointer(TEXCOORD_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(Loader::Vertex), (void*)(2 * sizeof(Loader::Vector3)));
	this->vbo.unbind();

	this->ebo.bind();
	this->ebo.bufferData((sizeof(GLuint) * indicies.size()), indicies.data(), GL_STATIC_DRAW);
	
	this->vao.unbind();
	this->ebo.unbind();
	this->indexCount = (int)indicies.size();*/
}


ModelGeometry::~ModelGeometry()
{
}
