#include "GameStateManager.h"
#include "PlayState.h"

#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION //stb_image requires this for some fucking reason
#include <stb_image.h>
#include "macrologger.h"
#include "Window.h"

#include "Components.h"

const static unsigned int DEFAULT_WIDTH = 1024, DEFAULT_HEIGHT = 768;
const static char* TITLE = "Dwavres Vs Zombies";

void initGLEW() {
	if (glewInit() != GLEW_OK) {
		LOG_ERROR("Failed to init GLEW");

		exit(-2);
	}
}

void run() {
	glewExperimental = GL_TRUE;
	Window::createWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, TITLE);
	initGLEW();

	
	//LOG_INFO("Created Window %d %d", GameWindow.getWidth(), GameWindow.getHeight());
	//initGLEW();

	GameStateManager gsm;
	gsm.enterState<PlayState>();

	int numFrames = 0;
	double lastTime = glfwGetTime();
	double currentTime = glfwGetTime();
	double delta = 1 / 60.0;
	do {
		currentTime = glfwGetTime();
		gsm.update(delta);

		numFrames++;

		if (currentTime - lastTime >= 1.0) {
			LOG_INFO("Milliseconds per Frame: %f", 1000.0 / numFrames);

			numFrames = 0;
			lastTime += 1.0;
		}
		
		delta = glfwGetTime() - currentTime;
		Window::update();
	} while (!Window::shouldClose());

	gsm.disposeAllStates();
	Window::destroyWindow();
}

int main(void) {
	run();
	return 0;
}