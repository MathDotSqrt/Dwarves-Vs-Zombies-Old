#include <stdio.h>


#define STB_IMAGE_IMPLEMENTATION //stb_image requires this for some fucking reason
#include <stb_image.h>

#include "macrologger.h"
#include "GameStateManager.h"
#include "PlayState.h"
#include "Components.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.h"


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

	GameStateManager gsm;
	gsm.enterState<PlayState>();

	int numFrames = 0;
	double lastTime = glfwGetTime();
	double currentTime = glfwGetTime();
	double delta = 1 / 60.0;
	do {
		currentTime = glfwGetTime();
		gsm.update((float)delta);

		numFrames++;

		if (currentTime - lastTime >= 1.0) {
			LOG_INFO("Milliseconds per Frame: %f", 1000.0 / numFrames);

			numFrames = 0;
			lastTime += 1.0;
		}
		
		Window::update();
		delta = glfwGetTime() - currentTime;
	} while (!Window::shouldClose());

	//TODO ensure that VBO and VAO is getting disposed
	gsm.disposeAllStates();
	Window::destroyWindow();
}

int main(void) {
	run();
	return 0;
}