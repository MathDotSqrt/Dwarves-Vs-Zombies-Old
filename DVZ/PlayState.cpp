#include "PlayState.h"
#include "macrologger.h"
#include "QuadGeometry.h"
#include "Components.h"
#include "MovementSystem.h"
#include "BasicRenderSystem.h"
#include <cstdlib>  // For srand() and rand()
#include "OpenGLRenderer.h"

#include "Window.h"

PlayState::PlayState(GameStateManager *gsm) : GameState(gsm) {
}


PlayState::~PlayState() {
}

void PlayState::init() {
	LOG_INFO("init");
	this->manager.addSystem(engine, new MovementSystem(1));
	this->manager.addSystem(engine, new BasicRenderSystem(&this->scene, 0));

	Graphics::QuadGeometry quad;
	//for (int i = 0; i < 100; i++) {
		unsigned int meshID = this->scene.createBasicMesh(quad, 1, 0, 1);
		unsigned int instanceID = this->scene.createInstance(meshID, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(.1, .1, .1));

		auto entityID = this->engine.create();
		this->engine.assign<PositionComponent>(entityID, glm::vec3(0, 0, 0));
		this->engine.assign<RotationComponent>(entityID, glm::vec3(0, 0, 0));
		this->engine.assign<ScaleComponent>(entityID, glm::vec3(.1, .1, .1));
		this->engine.assign<VelocityComponent>(entityID, glm::vec3(0, 0, 0));
		this->engine.assign<RotationalVelocityComponent>(entityID, glm::vec3(0, 0, 4));
		this->engine.assign<RenderInstanceComponent>(entityID, instanceID);
	//}
}

void PlayState::cleanUp() {
	LOG_INFO("cleanUp");
	this->manager.deleteAllActiveSystems(this->engine);
}

void PlayState::entered() {
	LOG_INFO("entered");

}

void PlayState::leaving() {
	LOG_INFO("leaving");

}

void PlayState::update(float delta) {
	if (Window::isPressed('w')) {
		LOG_INFO("TEST");
	}

	this->manager.updateSystems(this->engine, delta);

	Graphics::OpenGLRenderer::preRender();
	Graphics::OpenGLRenderer::render(&scene);
}