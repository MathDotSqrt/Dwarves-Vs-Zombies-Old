#include "Shader.h"

using namespace Shader;
using namespace Shader::Internal;

GLSLShader::GLSLShader(string name, GLuint programID, GLuint vertexID, GLuint fragmentID) :
	name(name),
	programID(programID),
	vertexID(vertexID),
	geometryID(-1),
	fragmentID(fragmentID),
	hasGeometryShader(false){
	this->m_isValid = true;
}

GLSLShader::GLSLShader(string name, GLuint programID, GLuint vertexID, GLuint geometryID, GLuint fragmentID) :
	name(name),
	programID(programID),
	vertexID(vertexID),
	geometryID(geometryID),
	fragmentID(fragmentID),
	hasGeometryShader(this->geometryID != -1){
	this->m_isValid = true;
}

GLSLShader::~GLSLShader(){
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
	map<string, GLint>::iterator it = this->uniforms.find(uniformName);

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

void GLSLShader::setUniform3f(string uniformName, glm::vec3 vec3) {
	setUniform3f(uniformName, vec3.x, vec3.y, vec3.z);
}

void GLSLShader::setUniform3f(string uniformName, float vec[3]) {
	setUniform3f(uniformName, vec[0], vec[1], vec[2]);
}

void GLSLShader::setUniform3f(string uniformName, float x, float y, float z) {
	GLint uniformLocation = getUniformLocation(uniformName);

	if(uniformLocation != -1)
		glUniform3f(uniformLocation, x, y, z);
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
		LOG_INFO("Disposed %s: {ProgramID %d, VertexID %d, GeometryID %d, Fragment %d}",
			this->name.c_str(), this->programID, this->vertexID, this->geometryID, this->fragmentID);
	}
	else {
		LOG_INFO("Disposed %s: {ProgramID %d, VertexID %d, Fragment %d}",
			this->name.c_str(), this->programID, this->vertexID, this->fragmentID);
	}

	this->uniforms.clear();
	this->m_isValid = false;
}

GLSLShader* Shader::getShader(string name) {
	GLSLShader* shader = shaderMap[name];

	if (shader == nullptr) {
		LOG_INFO("Creating shader: %s", name.c_str());
		shader = createShader(name);
		shaderMap[name] = shader;
	}
	else if(shader->isValid() == false){
		LOG_INFO("Recreating shader: %s", name.c_str());
		delete shader;
		shader = createShader(name);
		shaderMap[name] = shader;
	}

	return shader;
}

GLSLShader* Shader::createShader(string shaderName) {	
	//string preamble = readFile("preamble.glsl");
	string vertexSrc = readFile(shaderName + VERTEX_EXT);
	string fragmentSrc = readFile(shaderName + FRAGMENT_EXT);
	string geometrySrc = readFile(shaderName + GEOMETRY_EXT, false);


	bool hasGeometryShader = geometrySrc != "";
	LOG_INFO("Compiling shader %s", shaderName.c_str());

	GLuint vertexID = compileShader(vertexSrc, GL_VERTEX_SHADER);
	GLuint fragmentID = compileShader(fragmentSrc, GL_FRAGMENT_SHADER);
	GLuint geometryID = hasGeometryShader ? compileShader(geometrySrc, GL_GEOMETRY_SHADER) : -1;

	if (vertexID == 0 || fragmentID == 0 || geometryID == 0)
		return nullptr;

	GLuint programID = linkProgram(vertexID, geometryID, fragmentID);

	return new GLSLShader(shaderName, programID, vertexID, geometryID, fragmentID);
}

GLuint Shader::Internal::linkProgram(GLuint vertexID, GLuint geometryID, GLuint fragmentID) {
	GLuint programID = glCreateProgram();

	glAttachShader(programID, vertexID);
	glAttachShader(programID, fragmentID);
	if (geometryID != -1) glAttachShader(programID, geometryID);

	GLint isLinked = 0;
	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &isLinked); // check if program linked
	if (isLinked == GL_FALSE) {
		getLinkError(programID);
		glDeleteProgram(programID);
	}
	else {
		LOG_INFO("(P) Linked program %i", programID);
	}

	return programID;
}

GLuint Shader::Internal::compileShader(string source, GLenum shaderType) {
	const char *src = source.c_str();

	GLuint shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, &src, NULL);
	glCompileShader(shaderID);

	int compiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		getCompilationError(shaderID);
		glDeleteShader(shaderID);
		compiled = false;
		return 0;
	}
	
	LOG_INFO("OK - Shader ID: (%i)", shaderID);
	return shaderID;
}

void Shader::Internal::getCompilationError(GLuint shader) {
	int infologLength = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, (GLint *)&infologLength);
	char* infoLog = (char *)malloc(infologLength);
	glGetShaderInfoLog(shader, infologLength, NULL, infoLog); // will include terminate char
	LOG_ERROR("(S) Shader compilation error:\n%s", infoLog);
	free(infoLog);
}

void Shader::Internal::getLinkError(GLuint programID) {
	GLint infologLength = 0;
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, (GLint *)&infologLength);
	char* infoLog = (char *)malloc(infologLength);
	glGetProgramInfoLog(programID, infologLength, NULL, infoLog); // will include terminate char
	LOG_ERROR("(P) Program compilation error: \n%s", infoLog);
	free(infoLog);
}

string Shader::Internal::readFile(string filename, bool isRquired) {
	std::ifstream file(filename, std::ios::in);
	std::string text;
	if (file) {
		file.seekg(0, std::ios::end);		// go to end
		text.resize(file.tellg());			// resize text buffer to file size
		file.seekg(0, std::ios::beg);		// back to begin
		file.read(&text[0], text.size());	// read into buffer
		file.close();
	}
	else if(isRquired){
		std::string error_message = std::string("File not found: ") + filename;
		LOG_ERROR("%s", error_message.c_str());
		throw std::runtime_error(error_message);	//abort
	}
	else {
		return "";
	}
	return text;
}

void Shader::disposeAll() {
	LOG_INFO("Disposing all shaders...");
	unordered_map<string, GLSLShader*>::iterator it = shaderMap.begin();
	while (it != shaderMap.end()) {
		delete it->second;
		it++;
	}
}

