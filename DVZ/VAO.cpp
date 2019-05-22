#include "VAO.h"


VAO::VAO(){
	glGenVertexArrays(1, &this->vaoID);
	vaos.push_back(this->vaoID);
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
}

void VAO::disposeAll() {
	glDeleteVertexArrays(VAO::vaos.size(), VAO::vaos.data());
}

std::vector<GLuint> VAO::vaos;
