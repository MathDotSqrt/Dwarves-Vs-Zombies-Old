#pragma once
#include <GL/glew.h>
#include <glm.hpp>
#include <unordered_map>
#include <map>
#include <string>
#include <cstdio>
#include <fstream>
#include <exception>
#include "macrologger.h"

#define VERTEX_EXT ".vert"
#define GEOMETRY_EXT ".geom"
#define FRAGMENT_EXT ".frag"

using namespace std;
namespace Graphics { namespace Shader {
	

	class GLSLShader {
	public:
		friend GLSLShader *getShader(string);
		friend GLSLShader *createShader(string);
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

		GLint getUniformLocation(string uniformName);

		void setUniform1i(string uniformName, int i);

		void setUniform3f(string uniformName, glm::vec3 vec3);
		void setUniform3f(string uniformName, float vec[3]);
		void setUniform3f(string uniformName, float x, float y, float z);

		void setUniformMat4(string uniformName, glm::mat4 mat, bool transpose = GL_FALSE);
		void setUniformMat4(string uniformName, float mat[4 * 4], bool transpose = GL_FALSE);

		void dispose();

	private:
		const string name;
		const GLuint programID;
		const GLuint vertexID;
		const GLuint geometryID;
		const GLuint fragmentID;
		const bool hasGeometryShader;

		map<string, GLint> uniforms;
		bool m_isValid;

		GLSLShader(string name, GLuint programID, GLuint vertexID, GLuint fragmentID);
		GLSLShader(string name, GLuint programID, GLuint vertexID, GLuint geometryID, GLuint fragmentID);
		~GLSLShader();
	};

	
	namespace Internal {
		static unordered_map<string, GLSLShader*> shaderMap;

		GLuint linkProgram(GLuint vertexID, GLuint geometryID, GLuint fragmentID);
		GLuint compileShader(string src, GLenum shaderType);

		void getCompilationError(GLuint shaderID);
		void getLinkError(GLuint programID);
		string readFile(string filename, bool isRequired = true);
	}
	
	GLSLShader* getShader(string shaderName);
	GLSLShader* createShader(string shaderName);

	void disposeAll();
}}