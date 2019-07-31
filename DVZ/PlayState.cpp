//#define _HAS_STD_BYTE 0	//Visual Studio preprocessor value to turn off conflicting std::byte and byte for c++ 17
#include "PlayState.h"
#include "macrologger.h"
#include "QuadGeometry.h"
#include "ModelGeometry.h"
#include "Components.h"
#include "MovementSystem.h"
#include "InputSystem.h"
#include "BasicRenderSystem.h"
#include "NetPlayerSystem.h"

PlayState::PlayState(GameStateManager *gsm) : GameState(gsm) {

}


PlayState::~PlayState() {
}

void PlayState::init() {
	LOG_STATE("init");
	//e.attemptConnection("54.224.40.47", 60000);
	e.attemptConnection("127.0.0.1", 60000);

	this->e.addSystem(new InputSystem(0));
	this->e.addSystem(new MovementSystem(1));
	this->e.addSystem(new NetPlayerSystem(.1f, 2));
	this->e.addSystem(new BasicRenderSystem(100));
	this->e.addPlayer(0, 0, 0);


	Graphics::QuadGeometry quad;
	unsigned int meshID = this->e.getScene()->createBasicMesh(quad, 0, 1, 0);
	unsigned int renderID = this->e.getScene()->createRenderInstance(meshID, glm::vec3(0, -1, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));


	entt::entity floor = this->e.create();
	this->e.assign<PositionComponent>(floor, glm::vec3(0, -1, 0));
	this->e.assign<RotationComponent>(floor, glm::quat(glm::vec3(3.1415f / 2, 0, 0)));
	this->e.assign<ScaleComponent>(floor, glm::vec3(10, 10, 10));
	this->e.assign<RenderInstanceComponent>(floor, renderID);


	Graphics::ModelGeometry dragon("SpunkWalker.obj");

	for (int i = 0; i < 100; i++) {
		entt::entity obj = this->e.create();
		this->e.assign<PositionComponent>(obj, glm::vec3(i * 5 - 50 * 5, -1, -10));
		this->e.assign<RotationComponent>(obj, glm::quat(glm::vec3(0, 0, 0)));
		this->e.assign<ScaleComponent>(obj, glm::vec3(.3f, .3f, .3f));
		this->e.assign<RotationalVelocityComponent>(obj, glm::vec3(0, 1, 0));
		unsigned int meshID2;
		if (i % 2 == 0) {
			meshID2 = this->e.getScene()->createBasicMesh(dragon, i / 100.0f, i / 100.0f, .5f);
		}
		else {
			meshID2 = this->e.getScene()->createNormalMesh(dragon);
		}

		
		renderID = this->e.getScene()->createRenderInstance(meshID2);
		this->e.assign<RenderInstanceComponent>(obj, renderID);
	}
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