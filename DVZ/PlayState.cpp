//#define _HAS_STD_BYTE 0	//Visual Studio preprocessor value to turn off conflicting std::byte and byte for c++ 17
#include "PlayState.h"
#include "macrologger.h"

#include "Components.h"
#include "MovementSystem.h"
#include "InputSystem.h"
#include "BasicRenderSystem.h"
#include "NetPlayerSystem.h"
#include "VoxelSystem.h"
#include "ChunkManager.h"

#include "QuadGeometry.h"
#include "ModelGeometry.h"
#include "gtx/string_cast.hpp"

PlayState::PlayState(GameStateManager *gsm) : GameState(gsm) {

}


PlayState::~PlayState() {
}

void PlayState::init() {
	LOG_STATE("init");
	Graphics::Scene *scene = e.getScene();

	/*PLAYER*/
	entt::entity playerID = this->e.addPlayer(1, 0, 0);
	unsigned int pointLightInstanceID = this->e.getScene()->createPointLightInstance();
	this->e.assign<PointLightComponent>(playerID, pointLightInstanceID, glm::vec3(1, 1, 1), 60.0f);
	/*PLAYER*/
	
	Graphics::Geometry model = Graphics::CreateModel("tree.obj");
	Graphics::NormalMaterial material;
	unsigned int meshID = scene->createMesh(model, material);
	unsigned int renderID = scene->createRenderInstance(meshID);

	entt::entity test = this->e.create();
	this->e.assign<PositionComponent>(test, glm::vec3(0, 0, 0));
	this->e.assign<RotationComponent>(test, glm::quat(glm::vec3(0, 0, 0)));
	this->e.assign<ScaleComponent>(test, glm::vec3(1, 1, 1));
	this->e.assign<RotationalVelocityComponent>(test, glm::vec3(0, 1, 0));
	this->e.assign<RenderInstanceComponent>(test, renderID);

	/*NET*/
	//e.attemptConnection("54.224.40.47", 60000);	//AWS
	e.attemptConnection("127.0.0.1", 60000);		//LOCAL
	/*NET*/

	/*SYSTEM*/
	this->e.addSystem(new InputSystem(0));
	this->e.addSystem(new MovementSystem(1));
	this->e.addSystem(new NetPlayerSystem(.1f, 2));
	//this->e.addSystem(new VoxelSystem(3));
	this->e.addSystem(new BasicRenderSystem(100));
	/*SYSTEM*/

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