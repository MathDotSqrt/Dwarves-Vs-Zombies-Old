//#define _HAS_STD_BYTE 0	//Visual Studio preprocessor value to turn off conflicting std::byte and byte for c++ 17
#include "PlayState.h"
#include "macrologger.h"

#include "Components.h"
#include "StatelessSystem.h"
#include "GameSystems.h"

#include "Scene.h"
#include "ModelGeometry.h"
#include "TEX.h"

#include "ResourceManager.h"


PlayState::PlayState(GameStateManager *gsm) : GameState(gsm) {

}


PlayState::~PlayState() {
}

void PlayState::init() {
	LOG_STATE("init");
	auto &scene = e.ctx<Graphics::Scene>();
	auto &mesh_cache = e.ctx<ResourceManager::MeshCache>();

	/*PLAYER*/
	entt::entity playerID = e.addPlayer(0, 0, 0);
	unsigned int pointLightInstanceID = scene.createPointLightInstance();
	e.assign<PointLightComponent>(playerID, pointLightInstanceID, glm::vec3(1, 1, 1), 60.0f);
	/*PLAYER*/
	
	/*TREE*/
	auto model = mesh_cache.load<ResourceManager::MeshLoader>("tree"_hs, "res/tree.xobj");
	Graphics::NormalMaterial material;
	unsigned int renderID = scene.createRenderInstance(model, material);

	entt::entity tree = e.create();
	e.assign<PositionComponent>(tree, glm::vec3(0, 0, 0));
	e.assign<RotationComponent>(tree, glm::quat(glm::vec3(0, 0, 0)));
	e.assign<ScaleComponent>(tree, glm::vec3(1, 1, 1));
	e.assign<RotationalVelocityComponent>(tree, glm::vec3(0, 1, 0));
	e.assign<RenderInstanceComponent>(tree, renderID);
	/*TREE*/

	/*WALKER*/
	auto walkerModel = mesh_cache.load<ResourceManager::MeshLoader>("SpunkWalker"_hs, "res/SpunkWalker.xobj");
	Graphics::BasicLitMaterial walkerMaterial = { {1, 0, 1}, {1, 1, 1}, 10};
	uint32 walkerRenderID = scene.createRenderInstance(walkerModel, walkerMaterial);

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
	e.assign<RotationComponent>(sunID, glm::quat(glm::vec3(-.5, .1, 0)));
	e.assign<ScaleComponent>(sunID, glm::vec3(1, 1, 1));
	e.assign<DirComponent>(sunID, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
	e.assign<RotationalVelocityComponent>(sunID, glm::vec3(0, .1, 0));
	e.assign<VelocityComponent>(sunID, glm::vec3(.1, .1, 0));
	
	Graphics::Scene::Camera sunCamera = {
		glm::vec3(100, 10, 0),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0),
		70, 1, .1f, 1000
	};
	auto cameraID = scene.createCameraInstance(sunCamera);
	scene.setSunCameraID(cameraID);
	//scene->setMainCamera(cameraID);
	e.assign<CameraInstanceComponent>(sunID, cameraID);
	e.assign<RenderInstanceComponent>(sunID, scene.createRenderInstance(walkerModel, walkerMaterial));
	/*SUN CAMERA*/

	/*NET*/
	//e.attemptConnection("3.87.13.33", 60000);		//AWS
	e.attemptConnection("127.0.0.1", 60000);		//LOCAL
	/*NET*/

	/*SYSTEM*/
	e.addSystem(StatelessSystem(GameSystem::netword_system));
	e.addSystem(StatelessSystem(GameSystem::input_system));
	e.addSystem(StatelessSystem(GameSystem::shader_update_system, std::chrono::seconds(1)));
	e.addSystem(StatelessSystem(GameSystem::movement_system));
	e.addSystem(StatelessSystem(GameSystem::voxel_system));
	e.addSystem(StatelessSystem(GameSystem::send_packet_system));
	e.addSystem(StatelessSystem(GameSystem::render_system));
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

