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
	VBO(VBO&& other);
	~VBO();

	VBO(const VBO&) = delete;
	VBO& operator =(const VBO&) = delete;
	VBO& operator=(VBO &&other);

	void bind();
	void unbind(); 

	void bufferData(size_t bytes, void* data, GLenum drawType);

	template<typename T>
	void bufferData(const std::vector<T> &vector, GLenum drawType) {
		glBufferData(this->bufferType, sizeof(T) * vector.size(), vector.data(), drawType);
	}

	void dispose();
private:
	GLuint vboID = 0;
	GLuint bufferType;

};

}

