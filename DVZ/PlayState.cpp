//#define _HAS_STD_BYTE 0	//Visual Studio preprocessor value to turn off conflicting std::byte and byte for c++ 17
#include "PlayState.h"
#include "macrologger.h"
#include "QuadGeometry.h"
#include "Components.h"
#include "MovementSystem.h"
#include "InputSystem.h"
#include "BasicRenderSystem.h"

PlayState::PlayState(GameStateManager *gsm) : GameState(gsm) {

}


PlayState::~PlayState() {
}

void PlayState::init() {
	LOG_STATE("init");
	e.attemptConnection("54.224.40.47", 60000);
	//e.attemptConnection("127.0.0.1", 60000);

	this->e.addSystem(new MovementSystem(1));
	this->e.addSystem(new InputSystem(0));
	this->e.addSystem(new BasicRenderSystem(2));
	this->e.addPlayer(0, 0, 0);

	entt::entity floor = this->e.create();
	this->e.assign<PositionComponent>(floor, glm::vec3(0, -1, 0));
	this->e.assign<RotationComponent>(floor, glm::quat(glm::vec3(3.1415f / 2, 0, 0)));
	this->e.assign<ScaleComponent>(floor, glm::vec3(10, 10, 10));


	Graphics::QuadGeometry quad;
	unsigned int meshID = this->e.getScene()->createBasicMesh(quad, 0, 1, 0);
	unsigned int renderID = this->e.getScene()->createRenderInstance(meshID, glm::vec3(0, -1, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	this->e.assign<RenderInstanceComponent>(floor, renderID);

}

void PlayState::cleanUp() {
	LOG_STATE("cleanUp");
}

void PlayState::entered() {
	LOG_STATE("entered");

}

void PlayState::leaving() {
	LOG_STATE("leaving");

}

void PlayState::update(float delta) {
	this->e.update(delta);
}