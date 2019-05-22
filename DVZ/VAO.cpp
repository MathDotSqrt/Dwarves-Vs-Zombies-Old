#include "VAO.h"


VAO::VAO(){
	glGenVertexArrays(1, &this->vaoID);
	vaos.push_back(this->vaoID);

	LOG_DEBUG("Generated VAO ID: %d", this->vaoID);

}

VAO::~VAO(){
}

void VAO::bind() {
	glBindVertexArray(this->vaoID);
}

void VAO::unbind() {
	glBindVertexArray(0);
}

void VAO::dispose() {
	glDeleteVertexArrays(1, &this->vaoID);
	
	//removes a vao from the vao list
	VAO::vaos.erase(std::remove(vaos.begin(),vaos.end(), this->vaoID), VAO::vaos.end());

	LOG_DEBUG("Deleted VAO ID: %d", this->vaoID);
}

void VAO::disposeAll() {
	glDeleteVertexArrays(VAO::vaos.size(), VAO::vaos.data());

	LOG_DEBUG("Deleted %d VAOs", VAO::vaos.size());
}

std::vector<GLuint> VAO::vaos;
