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
