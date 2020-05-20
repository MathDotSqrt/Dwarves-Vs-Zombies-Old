#include <stdio.h>

#include "macrologger.h"
#include "GameStateManager.h"
#include "PlayState.h"

#include "ShaderSet.h"
#include "Window.h"
#include "Timer.h"

#define STB_IMAGE_IMPLEMENTATION //stb_image requires this for some fucking reason
#include "stb_image.h"

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
	bool info_pause = false;

	do {
		currentTime = glfwGetTime();
		//gsm.update((float)delta);
		gsm.update(1/60.0f);

		numFrames++;
		
		if (info_pause && Window::isPressed('o')) {
			info_pause = false;
			lastTime = currentTime;
		}
		else if (!info_pause && Window::isPressed('p')) {
			info_pause = true;
		}

		if (currentTime - lastTime >= 1.0 && !info_pause) {
			//LOG_INFO("Milliseconds per Frame: %f", 1000.0 / numFrames);
			//Util::Performance::getTimerInfo();

			numFrames = 0;
			lastTime += 1.0;
		}
		
		Window::update();
		delta = glfwGetTime() - currentTime;
	} while (!Window::shouldClose());

	//TODO ensure that VBO and VAO is getting disposed
	gsm.disposeAllStates();
	Window::destroyWindow();
	Graphics::Shader::disposeAll();
}


int main(void) {
	run();
	return 0;
}