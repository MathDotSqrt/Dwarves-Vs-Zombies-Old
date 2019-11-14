#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "IResizable.h"


namespace Window {
	const unsigned int ESC = GLFW_KEY_ESCAPE;

	const unsigned int UP = GLFW_KEY_UP;
	const unsigned int DOWN = GLFW_KEY_DOWN;
	const unsigned int LEFT = GLFW_KEY_LEFT;
	const unsigned int RIGHT = GLFW_KEY_RIGHT;
	const unsigned int SPACE = GLFW_KEY_SPACE;
	const unsigned int LSHIFT = GLFW_KEY_LEFT_SHIFT;
	const unsigned int LCTRL = GLFW_KEY_LEFT_CONTROL;

	const unsigned int ONE = GLFW_KEY_1;
	const unsigned int TWO = GLFW_KEY_2;
	const unsigned int THREE = GLFW_KEY_3;
	const unsigned int FOUR = GLFW_KEY_4;

	const unsigned int RIGHT_CLICK = GLFW_MOUSE_BUTTON_RIGHT;
	const unsigned int LEFT_CLICK = GLFW_MOUSE_BUTTON_LEFT;

	
	void createWindow(int width, int height, const char *title);
	void destroyWindow();
	void update();

	void addResizeCallback(IResizable *resizable);

	bool isPressed(char c);
	bool isPressed(const unsigned int c);
	bool isMousePressed(const unsigned int b);

	bool getMousePos(double &x, double &y);

	double getTime();

	bool shouldClose();
	int getWidth();
	int getHeight();
	GLFWwindow *getWindowPtr();

}

