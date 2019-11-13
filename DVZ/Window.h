#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "IResizable.h"


namespace Window {
	const unsigned int UP = GLFW_KEY_UP;
	const unsigned int DOWN = GLFW_KEY_DOWN;
	const unsigned int LEFT = GLFW_KEY_LEFT;
	const unsigned int RIGHT = GLFW_KEY_RIGHT;
	const unsigned int SPACE = GLFW_KEY_SPACE;
	const unsigned int LSHIFT = GLFW_KEY_LEFT_SHIFT;
	const unsigned int LCTRL = GLFW_KEY_LEFT_CONTROL;
	
	void createWindow(int width, int height, const char *title);
	void destroyWindow();
	void update();

	void addResizeCallback(IResizable *resizable);

	bool isPressed(char c);
	bool isPressed(const unsigned int c);

	double getTime();

	bool shouldClose();
	int getWidth();
	int getHeight();
	GLFWwindow *getWindowPtr();

}

