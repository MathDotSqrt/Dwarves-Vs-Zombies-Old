#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "VBO.h"
#include "macrologger.h"

const static unsigned int DEFAULT_WIDTH = 1024, DEFAULT_HEIGHT = 768;
const static char* TITLE = "Dwavres Vs Zombies";

static const GLfloat vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
};

GLFWwindow* initGLFW() {
	glewExperimental = GL_TRUE;										//Used to be needed for core profile?
	if (!glfwInit()) {
		LOG_ERROR("Failed to init GLFW");
		exit(-1);
	}

	glfwWindowHint(GLFW_SAMPLES, 4);								//Anti Aliasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					//Opengl version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);			//To make macOS happy (Probably won't port)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//We don't want old opengl

	GLFWwindow *window;
	window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, TITLE, NULL, NULL);

	if (window == NULL) {
		LOG_ERROR("Failed to open GLFW window. Probably has to do with opengl 3.3 not being supported");
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window);									//Displays window and makes it active
	return window;
}

void initGLEW() {
	if (glewInit() != GLEW_OK) {
		LOG_ERROR("Failed to init GLEW");

		exit(-2);
	}
}

int main(void) {
	GLFWwindow *window = initGLFW();
	initGLEW();

	VBO vbo(GL_ARRAY_BUFFER);
	vbo.bufferData(sizeof(vertex_buffer_data), (void*)vertex_buffer_data, GL_STATIC_DRAW);
	vbo.disposeAll();

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	do {
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
	
	glfwTerminate();

	return 0;
}