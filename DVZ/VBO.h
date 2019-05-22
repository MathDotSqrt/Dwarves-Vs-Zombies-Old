#pragma once

#include <GL/glew.h>
#include <vector>
#include <algorithm>


class VBO
{
public:
	VBO(GLenum);
	~VBO();

	void bind();
	void unbind(); 

	void bufferData(size_t bytes, void* data, GLenum drawType);

	void dispose();
	void disposeAll();
private:
	static std::vector<GLuint> vbos;
	GLuint vboID;
	GLuint bufferType;

};

