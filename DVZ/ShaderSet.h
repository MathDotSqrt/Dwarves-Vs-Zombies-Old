#pragma once

#include <GL/glew.h>
#include <glm.hpp>
#include <unordered_map>
#include <string>
#include <cstdio>
#include <fstream>
#include <exception>

#define VERTEX_EXT "vert"
#define GEOMETRY_EXT "geom"
#define FRAGMENT_EXT "frag"

namespace Graphics { namespace Shader{

	typedef GLuint ProgramID;
	typedef GLuint VertexID;
	typedef GLuint FragmentID;
	typedef GLuint GeometryID;

	class GLSLProgram {
	public:
		friend GLSLProgram *getShaderSet(const std::vector<std::string>& shaders);
		friend GLSLProgram *createShaderSet(const std::vector<std::string>& shaders);
		friend void disposeAll();


		inline bool isValid() {
			return this->m_isValid;
		}

		inline void use() {
			if (this->isValid())
				glUseProgram(this->programID);
		}

		inline void end() {
			if (this->isValid()) {
				glUseProgram(0);
			}
		}

		inline GLuint getAttrib(const char *attrib) {
			return glGetAttribLocation(this->programID, attrib);
		}

		GLint getUniformLocation(std::string uniformName);

		void setUniform1i(std::string uniformName, int i);
		void setUniform1f(std::string uniformName, float f);

		void setUniform3f(std::string uniformName, const glm::vec3& vec3);
		void setUniform3f(std::string uniformName, const float vec[3]);
		void setUniform3f(std::string uniformName, float x, float y, float z);

		void setUniformMat3(std::string uniformName, glm::mat3, bool transpose = GL_FALSE);
		void setUniformMat3(std::string uniformName, float mat[3 * 3], bool transpose = GL_FALSE);

		void setUniformMat4(std::string uniformName, glm::mat4 mat, bool transpose = GL_FALSE);
		void setUniformMat4(std::string uniformName, float mat[4 * 4], bool transpose = GL_FALSE);

		void dispose();

	private:
		const std::string name;
		const ProgramID programID;
		const VertexID vertexID;
		const FragmentID fragmentID;
		const GeometryID geometryID;

		std::unordered_map<std::string, GLint> uniforms;
		bool m_isValid;

		GLSLProgram(std::string name, ProgramID programID, VertexID vertexID, FragmentID fragmentID);
		GLSLProgram(std::string name, ProgramID programID, VertexID vertexID, GeometryID geometryID, FragmentID fragmentID);
		~GLSLProgram();
	};


	namespace Internal {
		static std::unordered_map<std::string, GLSLProgram*> shaderMap;

		std::string getProgramName(const std::vector<std::string>& shaders);

		GLuint linkProgram(GLuint vertexID, GLuint geometryID, GLuint fragmentID);
		GLuint compileShader(std::string src, GLenum shaderType);

		void getCompilationError(GLuint shaderID);
		void getLinkError(GLuint programID);
		std::string readFile(std::string filename);

		GLuint compileShader(std::string, GLenum shaderType);
		GLuint linkProgram(GLuint vertexID, GLuint geometryID, GLuint fragmentID);
	}

	GLSLProgram* getShaderSet(const std::vector<std::string>& shaders);
	GLSLProgram* createShaderSet(const std::vector<std::string>& shaders);

	void disposeAll();

}
}