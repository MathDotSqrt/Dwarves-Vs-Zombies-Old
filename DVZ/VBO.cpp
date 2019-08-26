#include "VBO.h"

using namespace Graphics;

VBO::VBO(GLenum bufferType){
	this->bufferType = bufferType;
	glGenBuffers(1, &this->vboID);

	VBO::vbos.push_back(this->vboID);

	LOG_DEBUG("Generated VBO ID: %d", this->vboID);
}


VBO::~VBO(){
}

void VBO::bind() {
	glBindBuffer(this->bufferType, this->vboID);
}

void VBO::unbind() {
	glBindBuffer(this->bufferType, 0);
}
//todo figure out if i want the binds within the method
void VBO::bufferData(size_t bytes, void* data, GLenum drawType) {
	//this->bind();
	glBufferData(this->bufferType, bytes, data, drawType);
	//this->unbind();
}

void VBO::dispose() {
	glDeleteBuffers(1, &this->vboID);

	//removes a vbo from the vbo list
	VBO::vbos.erase(std::remove(VBO::vbos.begin(), VBO::vbos.end(), this->vboID), vbos.end());

	LOG_DEBUG("Deleted VBO ID: %d", this->vboID);
}

void VBO::disposeAll() {
	glDeleteBuffers(VBO::vbos.size(), VBO::vbos.data());

	LOG_DEBUG("Deleted %d VBOs ", VBO::vbos.size());
}

std::vector<GLuint> VBO::vbos;
