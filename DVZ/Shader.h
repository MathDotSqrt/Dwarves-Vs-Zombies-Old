#pragma once
#include <GL/glew.h>
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

namespace Shader {
	

	class GLSLShader {
	public:
		friend GLSLShader *getShader(string);
		friend GLSLShader *createShader(string);
		friend void disposeAll();


		inline bool isValid() {
			return this->m_isValid;
		}

		inline void use() {
			if(this->isValid()) 
				glUseProgram(this->programID);
		}
		void dispose();

	private:

		const string name;
		const GLuint programID;
		const GLuint vertexID;
		const GLuint geometryID;
		const GLuint fragmentID;
		const bool hasGeometryShader;

		bool m_isValid;

		GLSLShader(string name, GLuint programID, GLuint vertexID, GLuint fragmentID);
		GLSLShader(string name, GLuint programID, GLuint vertexID, GLuint geometryID, GLuint fragmentID);
		~GLSLShader();
	};

	
	namespace Internal {
		static map<string, GLSLShader*> shaderMap;

		GLuint linkProgram(GLuint vertexID, GLuint geometryID, GLuint fragmentID);
		GLuint compileShader(string src, GLenum shaderType);

		void getCompilationError(GLuint shaderID);
		void getLinkError(GLuint programID);
		string readFile(string filename, bool isRequired = true);
	}
	
	GLSLShader* getShader(string shaderName);
	GLSLShader* createShader(string shaderName);

	void disposeAll();
}

