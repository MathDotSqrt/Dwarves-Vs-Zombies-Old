//#define _HAS_STD_BYTE 0	//Visual Studio preprocessor value to turn off conflicting std::byte and byte for c++ 17
#include "PlayState.h"
#include "macrologger.h"
#include "QuadGeometry.h"
#include "Components.h"
#include "MovementSystem.h"
#include "InputSystem.h"
#include "BasicRenderSystem.h"

#include "Window.h"
#include "Client.h"
//#include "RakPeerInterface.h"

namespace {
	unsigned int cameraEntityID;
	Network::Client *client;
}

PlayState::PlayState(GameStateManager *gsm) : GameState(gsm) {

}


PlayState::~PlayState() {
}

void PlayState::init() {
	LOG_STATE("init");

	client = new Network::Client();
	client->connect("54.224.40.47", 60000);

	this->renderer.init(&this->scene);
	
	this->manager.addSystem(engine, new MovementSystem(2));
	this->manager.addSystem(engine, new InputSystem(1));
	this->manager.addSystem(engine, new BasicRenderSystem(&this->scene, 0));
	Graphics::QuadGeometry quad;
	unsigned int meshID = this->scene.createBasicMesh(quad, 1, .5, 1);
	unsigned int instanceID = this->scene.createInstance(meshID, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(.1, .1, .1));

	auto entityID = this->engine.create();
	this->engine.assign<PositionComponent>(entityID, glm::vec3(0, 0, -1));
	this->engine.assign<RotationComponent>(entityID, glm::quat(1, 0, 0, 0));
	this->engine.assign<ScaleComponent>(entityID, glm::vec3(.1, .1, .1));
	this->engine.assign<VelocityComponent>(entityID, glm::vec3(0, 0, -1));
	this->engine.assign<RotationalVelocityComponent>(entityID, glm::vec3(1, 0, 1));
	this->engine.assign<RenderInstanceComponent>(entityID, instanceID);

	cameraEntityID = this->engine.create();
	this->engine.assign<PositionComponent>(cameraEntityID, glm::vec3(0, 0, 0));
	this->engine.assign<RotationComponent>(cameraEntityID, glm::quat(1, 0, 0, 0));
	this->engine.assign<VelocityComponent>(cameraEntityID, glm::vec3(0, 0, 0));
	this->engine.assign<RotationalVelocityComponent>(cameraEntityID, glm::vec3(0, 0, 0));
	this->engine.assign<DirComponent>(cameraEntityID, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
	this->engine.assign<CameraInstanceComponent>(cameraEntityID, this->scene.getMainCameraID());
	this->engine.assign<InputComponent>(cameraEntityID);
}

void PlayState::cleanUp() {
	LOG_STATE("cleanUp");
	this->manager.deleteAllActiveSystems(this->engine);

	delete client;
}

void PlayState::entered() {
	LOG_STATE("entered");

}

void PlayState::leaving() {
	LOG_STATE("leaving");

}

void PlayState::update(float delta) {
	client->poll();

	this->manager.updateSystems(this->engine, delta);

	this->renderer.prerender();
	this->renderer.render();
}