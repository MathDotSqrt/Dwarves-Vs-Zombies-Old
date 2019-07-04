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

	this->e.connect();
	Graphics::OpenGLRenderer *renderer = e.getRenderer();
	Graphics::Scene *scene = e.getScene();
	
	

	auto entityID = this->e.create();
	cameraEntityID = this->e.create();

	this->e.addSystem(new MovementSystem(3));
	this->e.addSystem(new InputSystem(2));
	this->e.addSystem(new BasicRenderSystem(0));


	Graphics::QuadGeometry quad;
	unsigned int meshID = scene->createBasicMesh(quad, 1, .5, 1);
	unsigned int instanceID = scene->createInstance(meshID, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(.1, .1, .1));

	this->e.assign<PositionComponent>(entityID, glm::vec3(0, 0, -1));
	this->e.assign<RotationComponent>(entityID, glm::quat(1, 0, 0, 0));
	this->e.assign<ScaleComponent>(entityID, glm::vec3(.1, .1, .1));
	this->e.assign<VelocityComponent>(entityID, glm::vec3(0, 0, -1));
	this->e.assign<RotationalVelocityComponent>(entityID, glm::vec3(1, 0, 1));
	this->e.assign<RenderInstanceComponent>(entityID, instanceID);

	this->e.assign<PositionComponent>(cameraEntityID, glm::vec3(0, 0, 0));
	this->e.assign<RotationComponent>(cameraEntityID, glm::quat(1, 0, 0, 0));
	this->e.assign<VelocityComponent>(cameraEntityID, glm::vec3(0, 0, 0));
	this->e.assign<RotationalVelocityComponent>(cameraEntityID, glm::vec3(0, 0, 0));
	this->e.assign<DirComponent>(cameraEntityID, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
	this->e.assign<CameraInstanceComponent>(cameraEntityID, scene->getMainCameraID());
	this->e.assign<InputComponent>(cameraEntityID);
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