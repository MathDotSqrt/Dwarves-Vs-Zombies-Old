#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

const static unsigned int DEFAULT_WIDTH = 1024, DEFAULT_HEIGHT = 768;
const static char* TITLE = "Dwavres Vs Zombies";

GLFWwindow* initGLFW() {
	//glewExperimental = true;										//Used to be needed for core profile?
	if (!glfwInit()) {
		fprintf(stderr, "Failed to init GLFW\n");					
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
		fprintf(stderr, "Failed to open GLFW window. Probably has to do with opengl 3.3 not being supported\n");
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	return window;
}

void initGLEW() {
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to init GLEW\n");

		exit(-2);
	}
}

int main(void) {
	GLFWwindow *window = initGLFW();
	initGLEW();

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	do {
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	return 0;
}