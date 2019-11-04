#include "VBO.h"
#include <assert.h>

using namespace Graphics;

VBO::VBO(GLenum bufferType){
	this->bufferType = bufferType;
	glGenBuffers(1, &this->vboID);
	LOG_DEBUG("Generated VBO ID: %d", this->vboID);
}

VBO::VBO(VBO&& other) : vboID(other.vboID), bufferType(other.bufferType){
	other.vboID = 0;
}

VBO::~VBO(){
	this->dispose();
}

VBO& VBO::operator=(VBO &&other) {
	if (this != &other) {
		this->dispose();

		//this->vboID is zero
		std::swap(this->vboID, other.vboID);
		std::swap(this->bufferType, other.bufferType);
	}

	//todo figure this out
	return *this;
}

void VBO::bind() {
	assert(this->vboID);
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
	if (this->vboID) {
		glDeleteBuffers(1, &this->vboID);
		LOG_DEBUG("Deleted VBO ID: %d", this->vboID);
		this->vboID = 0;
	}
}

