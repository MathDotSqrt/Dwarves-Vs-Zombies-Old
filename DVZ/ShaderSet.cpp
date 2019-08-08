#include "ShaderSet.h"
#include "macrologger.h"
#include <gtc/type_ptr.hpp>
#include <assert.h>

using namespace std;
using namespace Graphics;
using namespace Shader;
using namespace Shader::Internal;

GLSLProgram::GLSLProgram(string name, ProgramID programID, VertexID vertexID, FragmentID fragmentID) :
	name(name),
	programID(programID),
	vertexID(vertexID),
	geometryID(0),
	fragmentID(fragmentID){
	this->m_isValid = true;
}


GLSLProgram::GLSLProgram(string name, ProgramID programID, VertexID vertexID, GeometryID geometryID, FragmentID fragmentID) :
	name(name),
	programID(programID),
	vertexID(vertexID),
	geometryID(geometryID),
	fragmentID(fragmentID){
	this->m_isValid = true;
}

GLSLProgram::~GLSLProgram() {
	if (this->isValid()) {
		this->dispose();
	}
}

GLint GLSLProgram::getUniformLocation(string uniformName) {
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

void GLSLProgram::setUniform1i(string uniformName, int i) {
	GLuint uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation != -1)
		glUniform1i(uniformLocation, i);
}

void GLSLProgram::setUniform1f(string uniformName, float f) {
	GLuint uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation != -1)
		glUniform1f(uniformLocation, f);
}

void GLSLProgram::setUniform3f(string uniformName, glm::vec3 vec3) {
	setUniform3f(uniformName, vec3.x, vec3.y, vec3.z);
}

void GLSLProgram::setUniform3f(string uniformName, float vec[3]) {
	setUniform3f(uniformName, vec[0], vec[1], vec[2]);
}

void GLSLProgram::setUniform3f(string uniformName, float x, float y, float z) {
	GLint uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation != -1)
		glUniform3f(uniformLocation, x, y, z);
}

void GLSLProgram::setUniformMat3(string uniformName, glm::mat3 mat, bool transpose) {
	GLuint uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation != -1)
		glUniformMatrix3fv(uniformLocation, 1, transpose, glm::value_ptr(mat));
}

void GLSLProgram::setUniformMat3(string uniformName, float mat[3 * 3], bool transpose) {
	GLuint uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation != -1)
		glUniformMatrix3fv(uniformLocation, 1, transpose, mat);
}

void GLSLProgram::setUniformMat4(string uniformName, glm::mat4 mat, bool transpose) {
	GLint uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation != -1)
		glUniformMatrix4fv(uniformLocation, 1, transpose, glm::value_ptr(mat));
}

void GLSLProgram::setUniformMat4(string uniformName, float mat[4 * 4], bool transpose) {
	GLint uniformLocation = getUniformLocation(uniformName);

	if (uniformLocation != -1)
		glUniformMatrix4fv(uniformLocation, 1, transpose, mat);
}

void GLSLProgram::dispose() {
	glDetachShader(this->programID, this->vertexID);
	glDeleteShader(this->vertexID);

	glDetachShader(this->programID, this->fragmentID);
	glDeleteShader(this->fragmentID);

	if (this->geometryID) {
		glDetachShader(this->programID, this->geometryID);
		glDeleteShader(this->geometryID);
	}

	glDeleteProgram(this->programID);

	LOG_SHADE("Disposed %s: {ProgramID %d, VertexID %d, GeometryID %d, Fragment %d}",
		this->name.c_str(), this->programID, this->vertexID, this->geometryID, this->fragmentID);

	this->uniforms.clear();
	this->m_isValid = false;
}




GLSLProgram* Shader::getShaderSet(const std::vector<std::string>& shaderFilenames) {
	if (shaderFilenames.size() <= 1) {
		return nullptr;
	}
	
	std::string programName = getProgramName(shaderFilenames);

	GLSLProgram* program = shaderMap[programName];

	if (program == nullptr || program->isValid() == false) {
		LOG_SHADE("Creating program: %s", programName.c_str());
		program = createShaderSet(shaderFilenames);

		if (program == nullptr) {
			LOG_ERROR("Failed to create program %s", programName.c_str());
			return nullptr;
		}

		//todo maybe have to delete the invalid program from memory
		//not sure of unordered map does that automatically
		shaderMap[programName] = program;
	}
	

	return program;
}

GLSLProgram* Shader::createShaderSet(const std::vector<std::string>& shaderFilenames) {
	std::vector<std::string> sources;
	
	ProgramID programID;
	VertexID vertexID = 0;
	GeometryID geometryID = 0;
	FragmentID fragmentID = 0;

	GLuint *shaderID = nullptr;

	for (std::string shaderFilename : shaderFilenames) {
		GLuint shaderType = GL_NONE;
		size_t pos = shaderFilename.find_last_of('.');

		if (pos == std::string::npos) {
			LOG_ERROR("Invalid Filename [%s]", shaderFilename.c_str());
			return nullptr;
		}

		std::string ext = shaderFilename.substr(pos + 1);
		if (ext == VERTEX_EXT) {
			shaderType = GL_VERTEX_SHADER;
			shaderID = &vertexID;
		}
		else if (ext == GEOMETRY_EXT) {
			shaderType = GL_GEOMETRY_SHADER;
			shaderID = &geometryID;

		}
		else if (ext == FRAGMENT_EXT) {
			shaderType = GL_FRAGMENT_SHADER;
			shaderID = &fragmentID;
		}
		else {
			LOG_ERROR("Invalid Filetype [%s]", ext.c_str());
			return nullptr;
		}

		std::string source = readFile(shaderFilename);
		*shaderID = compileShader(source, shaderType);

		if (*shaderID == 0) {
			return nullptr;
		}
	}

	programID = linkProgram(vertexID, geometryID, fragmentID);

	if (programID == 0) {
		return nullptr;
	}

	return new GLSLProgram(getProgramName(shaderFilenames), programID, vertexID, geometryID, fragmentID);
}

std::string Shader::Internal::getProgramName(const std::vector<std::string>& shaders) {
	std::string out = "{";
	for (int i = 0; i < shaders.size(); i++) {
		out += shaders[i];

		if (i != shaders.size() - 1) {
			out += ", ";
		}
	}
	out += "}";
	return out;
}


string Shader::Internal::readFile(string filename) {
	std::ifstream file(filename, std::ios::in);
	std::string text;
	if (file) {
		file.seekg(0, std::ios::end);		// go to end
		text.resize(file.tellg());			// resize text buffer to file size
		file.seekg(0, std::ios::beg);		// back to begin
		file.read(&text[0], text.size());	// read into buffer
		file.close();
	}
	else {
		std::string error_message = std::string("File not found: ") + filename;
		LOG_ERROR("%s", error_message.c_str());
		throw std::runtime_error(error_message);	//abort
	}
	return text;
}

GLuint Shader::Internal::compileShader(std::string source, GLenum shaderType) {
	const char *src = source.c_str();

	GLuint shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, &src, NULL);
	glCompileShader(shaderID);

	int compiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		//Getting shader compulation Error
		int infologLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, (GLint *)&infologLength);
		char* infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(shaderID, infologLength, NULL, infoLog); // will include terminate char
		LOG_ERROR("(S) Shader compilation error:\n%s", infoLog);
		free(infoLog);

		glDeleteShader(shaderID);
		return 0;
	}

	LOG_SHADE("OK - Shader ID: (%i)", shaderID);
	return shaderID;
}

GLuint Shader::Internal::linkProgram(GLuint vertexID, GLuint geometryID, GLuint fragmentID) {
	GLuint programID = glCreateProgram();

	glAttachShader(programID, vertexID);
	if (geometryID != 0) glAttachShader(programID, geometryID);
	glAttachShader(programID, fragmentID);

	GLint isLinked = 0;
	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &isLinked); // check if program linked
	if (isLinked == GL_FALSE) {
		//Link error
		GLint infologLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, (GLint *)&infologLength);
		char* infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(programID, infologLength, NULL, infoLog); // will include terminate char
		LOG_ERROR("(P) Program linking error: \n%s", infoLog);
		free(infoLog);

		glDeleteProgram(programID);
		return 0;
	}

	LOG_SHADE("(P) Linked program %i", programID);
	return programID;
}

void Shader::disposeAll() {
	LOG_SHADE("Disposing all shaders...");
	unordered_map<string, GLSLProgram*>::iterator it = shaderMap.begin();
	while (it != shaderMap.end()) {
		delete it->second;
		it++;
	}

	shaderMap.clear();
}