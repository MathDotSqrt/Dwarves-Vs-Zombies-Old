#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

namespace Window {
	void createWindow(int width, int height, const char *title);
	void destroyWindow();
	void update();

	void addResizeCallback(void(*resize_func)(int newWidth, int newHeight));

	bool isPressed(char c);

	bool shouldClose();
	int getWidth();
	int getHeight();
	GLFWwindow *getWindowPtr();

}

