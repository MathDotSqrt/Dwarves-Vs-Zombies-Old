#include "PlayState.h"
#include "macrologger.h"
#include "QuadGeometry.h"
#include "Components.h"
#include "MovementSystem.h"
#include "BasicRenderSystem.h"

#include <cstdlib>  // For srand() and rand()

PlayState::PlayState(GameStateManager *gsm) : GameState(gsm) {
}


PlayState::~PlayState() {
}

void PlayState::init() {
	LOG_INFO("init");

	this->renderer.init();
	this->manager.addSystem(engine, new MovementSystem(1));
	this->manager.addSystem(engine, new BasicRenderSystem(&this->scene, 0));


	QuadGeometry quad;

	for (int i = 0; i < 500; i++) {
		glm::vec3 pos(rand(), rand(), 0);
		pos /= RAND_MAX;

		unsigned int meshID = this->scene.createBasicMesh(quad, pos.x, pos.y, pos.z);

		unsigned int instanceID = this->scene.createInstance(meshID, pos, pos, glm::vec3(.1, .1, .1));
		auto entityID = engine.create();
		this->engine.assign<PositionComponent>(entityID, pos);
		this->engine.assign<RotationComponent>(entityID, pos);
		this->engine.assign<ScaleComponent>(entityID, glm::vec3(.1, .1, .1));
		this->engine.assign<VelocityComponent>(entityID, -pos);
		this->engine.assign<RenderInstanceComponent>(entityID, instanceID);
	}
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
	this->manager.updateSystems(this->engine, delta);

	this->renderer.render(&scene);
}