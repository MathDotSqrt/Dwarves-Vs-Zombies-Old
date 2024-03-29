
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "Window.h"
#include "macrologger.h"
#include "ctype.h"

namespace {
	GLFWwindow *window;
	int width, height;
	const char *title;

	bool has_focus = true;
	bool isMouseDisabled = false;

	std::vector<IResizable*> callbacks;

	void internalResizeCallBack(GLFWwindow *window, int newWidth, int newHeight) {
		for (auto func : callbacks) {
			func->resize(newWidth, newHeight);
		}
	}

	void internalFocusCallBack(GLFWwindow *window, int focused) {
		if (focused) {
			has_focus = true;
			isMouseDisabled = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


		}
		else {
			has_focus = false;
			isMouseDisabled = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	void internalMouseCallBack(GLFWwindow *window, int button, int action, int mods) {
		if (action == GLFW_PRESS) {
			isMouseDisabled = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
}

void Window::createWindow(int initWidth, int initHeight, const char* initTitle) {
	if (!glfwInit()) {
		LOG_ERROR("Failed to init GLFW");
		exit(-1);
	}
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);						//Double Buffering
	glfwWindowHint(GLFW_SAMPLES, 4);								//Anti Aliasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					//Opengl version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);			//To make macOS happy (Probably won't port)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//We don't want old opengl
	window = glfwCreateWindow(initWidth, initHeight, initTitle, NULL, NULL);

	if (window == NULL) {
		LOG_ERROR("Failed to open GLFW window. Probably has to do with opengl 3.3 not being supported");
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window);									//Displays window and makes it active
	//glfwSwapInterval(1);											//Wheather to enable or disable vsync
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, 0, 0);
	isMouseDisabled = true;
	width = initWidth;
	height = initHeight;
	title = initTitle;

	glfwSetWindowSizeCallback(window, internalResizeCallBack);
	glfwSetWindowFocusCallback(window, internalFocusCallBack);
	glfwSetMouseButtonCallback(window, internalMouseCallBack);
}

void Window::destroyWindow() {
	glfwTerminate();
}

void Window::update() {
	glfwPollEvents();
	
	if (Window::isPressed(Window::ESC)) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		isMouseDisabled = false;
	}

	glfwSwapBuffers(window);
}

void Window::addResizeCallback(IResizable *resizable) {
	callbacks.push_back(resizable);
}

bool Window::isFocused() {
	return isMouseDisabled;
}

bool Window::isPressed(char c) {
	return glfwGetKey(window, (int)toupper(c)) == GLFW_PRESS;
}

bool Window::isPressed(const unsigned int c) {
	return glfwGetKey(window, c) == GLFW_PRESS;
}

bool Window::isMousePressed(const unsigned int b) {
	return glfwGetMouseButton(window, b) == GLFW_PRESS;
}

bool Window::getMousePos(double &x, double &y) {
	glfwGetCursorPos(window, &x, &y);
	//LOG_ALWAYS("%lf %lf", x, y);

	return isMouseDisabled;
}

double Window::getTime() {
	return glfwGetTime();
}

bool Window::shouldClose() {
	return glfwWindowShouldClose(window);
}

int Window::getWidth() {
	return width;
}

int Window::getHeight() {
	return height;
}

GLFWwindow *Window::getWindowPtr() {
	return window;
}

//_Window::~_Window()
//{
//	glfwTerminate();
//}
//
//void _Window::createWindow(int width, int height, const char* title) {
	
//}
//
//void _Window::update() {
//	glfwSwapBuffers(GameWindow.window);
//	glfwPollEvents();
//}
//
//void _Window::addResizeCallback(void(*resize_func)(int newWidth, int newHeight)){
//	GameWindow.callbacks.push_back(resize_func);
//}
//
//void _Window::internalResizeCallback(GLFWwindow *window, int newWidth, int newHeight) {
//	for (auto func : GameWindow.callbacks) {
//		func(newWidth, newHeight);
//	}
//}
//
//bool _Window::shouldClose() {
//	return glfwWindowShouldClose(GameWindow.window);
//}
//
//int _Window::getWidth() {
//	return GameWindow.width;
//}
//
//int _Window::getHeight() {
//	return GameWindow.height;
//}
//
//GLFWwindow *_Window::getWindowPtr() {
//	return GameWindow.window;
//}