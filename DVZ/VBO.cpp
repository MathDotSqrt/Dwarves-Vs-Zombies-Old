#include "VBO.h"


VBO::VBO(GLenum bufferType){
	this->bufferType = bufferType;
	glGenBuffers(1, &this->vboID);

	VBO::vbos.push_back(this->vboID);
}


VBO::~VBO(){
}

void VBO::bind() {
	glBindBuffer(this->bufferType, this->vboID);
}

void VBO::unbind() {
	glBindBuffer(this->bufferType, 0);
}

void VBO::bufferData(size_t bytes, void* data, GLenum drawType) {
	this->bind();
	glBufferData(this->bufferType, bytes, data, drawType);
	this->unbind();
}

void VBO::dispose() {
	glDeleteBuffers(1, &this->vboID);

	//removes a vbo from the vbo list
	VBO::vbos.erase(std::remove(VBO::vbos.begin(), VBO::vbos.end(), this->vboID), vbos.end());
}

void VBO::disposeAll() {
	glDeleteBuffers(VBO::vbos.size(), VBO::vbos.data());
}

std::vector<GLuint> VBO::vbos;
