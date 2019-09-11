#include "VAO.h"

using namespace Graphics;

VAO::VAO(){
	glGenVertexArrays(1, &this->vaoID);

	LOG_DEBUG("Generated VAO ID: %d", this->vaoID);
}

VAO::~VAO(){
	this->dispose();
}

VAO::VAO(VAO &&other){
	this->vaoID = other.vaoID;
	other.vaoID = 0; //use null buffer id for old object
}

VAO& VAO::operator=(VAO &&other) {
	if (this != &other) {
		this->dispose();
		//this->vaoID is zero
		std::swap(this->vaoID, other.vaoID);
	}

	//todo figure this out
	return *this;
}

void VAO::bind() {
	glBindVertexArray(this->vaoID);
}

void VAO::unbind() {
	glBindVertexArray(0);
}

void VAO::dispose() {
	if (this->vaoID) {
		glDeleteVertexArrays(1, &this->vaoID);
		LOG_DEBUG("Deleted VAO ID: %d", this->vaoID);
		this->vaoID = 0;
	}
}
