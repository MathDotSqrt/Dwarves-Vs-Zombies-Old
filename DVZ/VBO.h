#pragma once

#include <GL/glew.h>
#include <vector>
#include <algorithm>
#include "macrologger.h"
#include <string>
namespace Graphics {

class VBO {
public:
	VBO(GLenum);
	~VBO();

	void bind();
	void unbind(); 

	void bufferData(size_t bytes, void* data, GLenum drawType);

	template<typename T>
	void bufferData(std::vector<T> &vector, GLenum drawType) {
		glBufferData(this->bufferType, sizeof(T) * vector.size(), vector.data(), drawType);
	}

	void dispose();
	void disposeAll();
private:
	static std::vector<GLuint> vbos;
	GLuint vboID;
	GLuint bufferType;

};

}

