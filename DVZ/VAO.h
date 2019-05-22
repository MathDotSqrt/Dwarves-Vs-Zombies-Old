#pragma once
#include <vector>
#include <algorithm>

#include <GL/glew.h>

class VAO
{
public:
	VAO();
	~VAO();

	void bind();
	void unbind();
	void dispose();

	static void disposeAll();
private:
	static std::vector<GLuint> vaos;

	GLuint vaoID;
};

