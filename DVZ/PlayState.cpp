//#define _HAS_STD_BYTE 0	//Visual Studio preprocessor value to turn off conflicting std::byte and byte for c++ 17
#include "PlayState.h"
#include "macrologger.h"

#include "Components.h"
#include "StatelessSystem.h"
#include "GameSystems.h"
#include "CollisionSystems.h"

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
	entt::entity playerID = e.addPlayer(0, 20, 0);
	unsigned int pointLightInstanceID = scene.createPointLightInstance();
	e.assign<Component::PointLight>(playerID, pointLightInstanceID, glm::vec3(1, 1, 1), 60.0f);
	e.assign<const Physics::AABB>(playerID, Physics::AABB(glm::vec3(-.3f, -.3f, -.3f), glm::vec3(.3f, .3f, .3f)));
	/*PLAYER*/
	
	/*TREE*/
	auto model = mesh_cache.load<ResourceManager::MeshLoader>("tree"_hs, "res/tree.xobj");
	Graphics::NormalMaterial material;
	unsigned int renderID = scene.createRenderInstance(model, material);

	entt::entity tree = e.create();
	e.assign<Component::Position>(tree, glm::vec3(0, 15, 0));
	e.assign<Component::Rotation>(tree, glm::quat(glm::vec3(0, 0, 0)));
	e.assign<Component::Scale>(tree, glm::vec3(.1, .1, .1));
	e.assign<Component::RotationalVelocity>(tree, glm::vec3(0, 1, 0));
	e.assign<Component::RenderInstance>(tree, renderID);
	/*TREE*/

	//e.getBlockPlaceBuffer().push_back(std::make_pair(glm::vec3(0), Voxel::Block(Voxel::BlockType::BLOCK_TYPE_GLASS)));

	/*WALKER*/
	auto walkerModel = mesh_cache.load<ResourceManager::MeshLoader>("SpunkWalker"_hs, "res/SpunkWalker.xobj");
	Graphics::BasicLitMaterial walkerMaterial = { {1, 0, 1}, {1, 1, 1}, 10};
	uint32 walkerRenderID = scene.createRenderInstance(walkerModel, walkerMaterial);

	entt::entity walker = e.create();
	e.assign<Component::Position>(walker, glm::vec3(0, 15, 10));
	e.assign<Component::Rotation>(walker, glm::quat(glm::vec3(0, 0, 0)));
	e.assign<Component::Scale>(walker, glm::vec3(1));
	//e.assign<Component::Velocity>(walker, glm::vec3(-10, .1f, 2));
	e.assign<Component::RenderInstance>(walker, walkerRenderID);
	/*WALKER*/

	/*SUN CAMERA*/
	entt::entity sunID = e.create();
	e.assign<Component::Position>(sunID, glm::vec3(-0, 20, 25));
	e.assign<Component::Rotation>(sunID, glm::quat(glm::vec3(-.5, .1, 0)));
	e.assign<Component::Scale>(sunID, glm::vec3(1, 1, 1));
	e.assign<Component::Dir>(sunID, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
	e.assign<Component::RotationalVelocity>(sunID, glm::vec3(0, .1, 0));
	e.assign<Component::Velocity>(sunID, glm::vec3(.1, .1, 0));
	
	Graphics::Scene::Camera sunCamera = {
		glm::vec3(100, 10, 0),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0),
		70, 1, .1f, 1000
	};
	auto cameraID = scene.createCameraInstance(sunCamera);
	scene.setSunCameraID(cameraID);
	//scene->setMainCamera(cameraID);
	e.assign<Component::CameraInstance>(sunID, cameraID);
	e.assign<Component::RenderInstance>(sunID, scene.createRenderInstance(walkerModel, walkerMaterial));
	/*SUN CAMERA*/

	/*NET*/
	//e.attemptConnection("3.86.12.190", 60000);		//AWS
	e.attemptConnection("127.0.0.1", 60000);		//LOCAL
	/*NET*/

	/*SYSTEM*/
	e.addSystem(StatelessSystem(System::netword_system));
	e.addSystem(StatelessSystem(System::input_system));
	e.addSystem(StatelessSystem(System::shader_update_system, std::chrono::seconds(1)));

	e.addSystem(StatelessSystem(System::voxel_collision_system));
	e.addSystem(StatelessSystem(System::movement_system));
	e.addSystem(StatelessSystem(System::voxel_system));
	e.addSystem(StatelessSystem(System::send_packet_system));
	e.addSystem(StatelessSystem(System::render_system));
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

