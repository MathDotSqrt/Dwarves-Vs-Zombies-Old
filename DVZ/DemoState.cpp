#include "DemoState.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "macrologger.h"

DemoState::~DemoState()
{
}

void DemoState::init()
{
	/*const GLfloat vertex_buffer_data[] = {
	   -1, -1, 0, 1, 0, 0, 0, 1,
	   -1, 1, 0, 0, 1, 0, 0, 0,
	   1,  -1, 0, 0, 0, 1, 1, 1,
	   1, 1, 0, 1, 1, 1, 1, 0
	};

	const GLuint elements[] = {
		1, 2, 3,
		0, 1, 2
	};

	this->shader = Shader::getShader("test");
	GLuint posAttrib = shader->getAttrib("vert_pos");
	GLuint colAttrib = shader->getAttrib("vert_col");
	GLuint texAttrib = shader->getAttrib("vert_tex_coord");

	vao = new VAO();
	vao->bind();

	vbo = new VBO(GL_VERTEX_ARRAY);
	vbo->bufferData(sizeof(vertex_buffer_data), (void*)vertex_buffer_data, GL_STATIC_DRAW);
	vbo->bind();

	ebo = new VBO(GL_ELEMENT_ARRAY_BUFFER);
	ebo->bufferData(sizeof(elements), (void*)elements, GL_STATIC_DRAW);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	vbo->unbind();
	
	TEX texture1 = TEX::Builder("test.png").nearest()->clampToEdge()->mipmapNearest()->buildTexture();
	TEX texture2 = TEX::Builder("checker.jpg").linear()->clampToEdge()->buildTexture();

	shader->use();

	texture1.bindActiveTexture(0);
	texture2.bindActiveTexture(1);

	shader->setUniform1i("tex1", 0);
	shader->setUniform1i("tex2", 1);

	glEnableVertexAttribArray(posAttrib);
	glEnableVertexAttribArray(colAttrib);
	glEnableVertexAttribArray(texAttrib);

	ebo->bind();*/
}

void DemoState::cleanUp(){
	
}

void DemoState::entered()
{
}

void DemoState::leaving()
{
}

void DemoState::update(float delta) {
	glClear(GL_COLOR_BUFFER_BIT);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
