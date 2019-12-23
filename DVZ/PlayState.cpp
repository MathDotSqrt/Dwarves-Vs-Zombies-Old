//#define _HAS_STD_BYTE 0	//Visual Studio preprocessor value to turn off conflicting std::byte and byte for c++ 17
#include "PlayState.h"
#include "macrologger.h"

#include "Components.h"
#include "MovementSystem.h"
#include "InputSystem.h"
#include "BasicRenderSystem.h"
#include "NetPlayerSystem.h"
#include "VoxelSystem.h"
#include "ShaderUpdaterSystem.h"

#include "ModelGeometry.h"
#include "TEX.h"

PlayState::PlayState(GameStateManager *gsm) : GameState(gsm) {

}


PlayState::~PlayState() {
}

void PlayState::init() {
	LOG_STATE("init");
	Graphics::Scene *scene = e.getScene();
	Graphics::TEX texture = Graphics::TEX::Builder("terrain.png").buildTexture();

	/*PLAYER*/
	entt::entity playerID = e.addPlayer(0, 0, 0);
	unsigned int pointLightInstanceID = e.getScene()->createPointLightInstance();
	e.assign<PointLightComponent>(playerID, pointLightInstanceID, glm::vec3(1, 1, 1), 60.0f);
	/*PLAYER*/


	
	/*TREE*/
	Graphics::Geometry model = Graphics::CreateModel("tree.obj");
	Graphics::NormalMaterial material;
	unsigned int meshID = scene->createMesh(model, material);
	unsigned int renderID = scene->createRenderInstance(meshID);

	entt::entity tree = e.create();
	e.assign<PositionComponent>(tree, glm::vec3(0, 0, 0));
	e.assign<RotationComponent>(tree, glm::quat(glm::vec3(0, 0, 0)));
	e.assign<ScaleComponent>(tree, glm::vec3(1, 1, 1));
	e.assign<RotationalVelocityComponent>(tree, glm::vec3(0, 1, 0));
	e.assign<RenderInstanceComponent>(tree, renderID);
	/*TREE*/

	/*WALKER*/
	Graphics::Geometry walkerModel = Graphics::CreateModel("SpunkWalker.obj");
	Graphics::BasicLitMaterial walkerMaterial = { {1, 0, 1}, {1, 1, 1}, 10};
	uint32 walkerMeshID = scene->createMesh(walkerModel, walkerMaterial);
	uint32 walkerRenderID = scene->createRenderInstance(walkerMeshID);

	entt::entity walker = e.create();
	e.assign<PositionComponent>(walker, glm::vec3(10, 10, 10));
	e.assign<RotationComponent>(walker, glm::quat(glm::vec3(0, 0, 0)));
	e.assign<ScaleComponent>(walker, glm::vec3(1));
	e.assign<VelocityComponent>(walker, glm::vec3(-10, .1f, 2));
	e.assign<RenderInstanceComponent>(walker, walkerRenderID);
	/*WALKER*/

	/*SUN CAMERA*/
	entt::entity sunID = e.create();
	e.assign<PositionComponent>(sunID, glm::vec3(-0, 20, 25));
	e.assign<RotationComponent>(sunID, glm::quat(glm::vec3(-.5, 0, 0)));
	e.assign<ScaleComponent>(sunID, glm::vec3(1, 1, 1));
	e.assign<DirComponent>(sunID, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
	Graphics::Scene::Camera sunCamera = {
		glm::vec3(100, 10, 0),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0),
		70, 1, .1f, 1000
	};
	auto cameraID = scene->createCameraInstance(sunCamera);
	scene->setSunCameraID(cameraID);
	e.assign<CameraInstanceComponent>(sunID, cameraID);
	e.assign<RenderInstanceComponent>(sunID, scene->createRenderInstance(walkerMeshID));
	/*SUN CAMERA*/

	/*NET*/
	//e.attemptConnection("54.224.40.47", 60000);	//AWS
	e.attemptConnection("127.0.0.1", 60000);		//LOCAL
	/*NET*/

	/*SYSTEM*/
	e.addSystem(new InputSystem(0));
	e.addSystem(new ShaderUpdaterSystem(1.0f, 100));
	e.addSystem(new MovementSystem(200));
	//this->e.addSystem(new NetPlayerSystem(.1f, 2));
	e.addSystem(new VoxelSystem(300));
	e.addSystem(new BasicRenderSystem(500));
	/*SYSTEM*/

	//Graphics::TEX t = Graphics::TEX::Builder("terrain_debug.png").nearest().clampToEdge().buildTextureAtlasArray(16, 16);

	LOG_SYSTEM("init");
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

