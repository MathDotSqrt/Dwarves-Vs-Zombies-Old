#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "IResizable.h"


namespace Window {
	void createWindow(int width, int height, const char *title);
	void destroyWindow();
	void update();

	void addResizeCallback(IResizable *resizable);

	bool isPressed(char c);

	bool shouldClose();
	int getWidth();
	int getHeight();
	GLFWwindow *getWindowPtr();

}

