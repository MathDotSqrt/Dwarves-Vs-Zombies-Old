#include "ShaderSet.h"
#include "macrologger.h"
#include <gtc/type_ptr.hpp>
#include <assert.h>

using namespace std;
using namespace Graphics;
using namespace Shader;
using namespace Shader::Internal;

GLSLShader::GLSLShader(string name, GLuint programID, GLuint vertexID, GLuint fragmentID) :
	name(name),
	programID(programID),
	vertexID(vertexID),
	geometryID(-1),
	fragmentID(fragmentID),
	hasGeometryShader(false) {
	this->m_isValid = true;
}

GLSLShader::GLSLShader(string name, GLuint programID, GLuint vertexID, GLuint geometryID, GLuint fragmentID) :
	name(name),
	programID(programID),
	vertexID(vertexID),
	geometryID(geometryID),
	fragmentID(fragmentID),
	hasGeometryShader(this->geometryID != -1) {
	this->m_isValid = true;
}

GLSLShader::~GLSLShader() {
	if (this->isValid()) {
		this->dispose();
	}
}

GLint GLSLShader::getUniformLocation(string uniformName) {
	//if shader has been deleted on the card then there is no
	//uniform name to find
	if (!this->isValid()) {
		return -1;
	}

	GLint location;

	//iterator for the uniforms map that searches for uniformName 
	unordered_map<string, GLint>::iterator it = this->uniforms.find(uniformName);

	//if iterator could not find uniform name
	if (it == this->uniforms.end()) {
		//find shader uniform location given the name
		location = glGetUniformLocation(this->programID, uniformName.c_str());
		//store it within the map, to make subsequent searches way faster
		this->uniforms[uniformName] = location;
	}
	//if iterator found the uniform name with in the map
	else {
		//set location the the value of the uniformName key
		location = it->second;
	}

	return location;
}

void GLSLShader::setUniform1i(string uniformName, int i) {
	GLuint uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation != -1)
		glUniform1i(uniformLocation, i);
}

void GLSLShader::setUniform1f(string uniformName, float f) {
	GLuint uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation != -1)
		glUniform1f(uniformLocation, f);
}

void GLSLShader::setUniform3f(string uniformName, glm::vec3 vec3) {
	setUniform3f(uniformName, vec3.x, vec3.y, vec3.z);
}

void GLSLShader::setUniform3f(string uniformName, float vec[3]) {
	setUniform3f(uniformName, vec[0], vec[1], vec[2]);
}

void GLSLShader::setUniform3f(string uniformName, float x, float y, float z) {
	GLint uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation != -1)
		glUniform3f(uniformLocation, x, y, z);
}

void GLSLShader::setUniformMat3(string uniformName, glm::mat3 mat, bool transpose) {
	GLuint uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation != -1)
		glUniformMatrix3fv(uniformLocation, 1, transpose, glm::value_ptr(mat));
}

void GLSLShader::setUniformMat3(string uniformName, float mat[3 * 3], bool transpose) {
	GLuint uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation != -1)
		glUniformMatrix3fv(uniformLocation, 1, transpose, mat);
}

void GLSLShader::setUniformMat4(string uniformName, glm::mat4 mat, bool transpose) {
	GLint uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation != -1)
		glUniformMatrix4fv(uniformLocation, 1, transpose, glm::value_ptr(mat));
}

void GLSLShader::setUniformMat4(string uniformName, float mat[4 * 4], bool transpose) {
	GLint uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation != -1)
		glUniformMatrix4fv(uniformLocation, 1, transpose, mat);
}

void GLSLShader::dispose() {
	glDetachShader(this->programID, this->vertexID);
	glDeleteShader(this->vertexID);

	glDetachShader(this->programID, this->fragmentID);
	glDeleteShader(this->fragmentID);

	if (this->hasGeometryShader) {
		glDetachShader(this->programID, this->geometryID);
		glDeleteShader(this->geometryID);
	}

	glDeleteProgram(this->programID);

	if (this->hasGeometryShader) {
		LOG_SHADE("Disposed %s: {ProgramID %d, VertexID %d, GeometryID %d, Fragment %d}",
			this->name.c_str(), this->programID, this->vertexID, this->geometryID, this->fragmentID);
	}
	else {
		LOG_SHADE("Disposed %s: {ProgramID %d, VertexID %d, Fragment %d}",
			this->name.c_str(), this->programID, this->vertexID, this->fragmentID);
	}

	this->uniforms.clear();
	this->m_isValid = false;
}

GLSLShader* Shader::getShaderSet(string shaderName[]) {
	//Assert that shader name is size > 0
	assert(shaderName->length);
}