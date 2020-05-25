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
#include "Window.h"


PlayState::PlayState(GameStateManager *gsm) : GameState(gsm) {

}


PlayState::~PlayState() {
}

void PlayState::init() {
	LOG_STATE("init");
	auto &scene = e.ctx<Graphics::Scene>();
	auto &mesh_cache = e.ctx<ResourceManager::MeshCache>();
	auto model = mesh_cache.load<ResourceManager::MeshLoader>("tree"_hs, "res/tree.xobj");
	auto walkerModel = mesh_cache.load<ResourceManager::MeshLoader>("SpunkWalker"_hs, "res/SpunkWalker.xobj");

	/*PLAYER*/
	entt::entity playerID = e.addPlayer(0, 100, 0);
	/*PLAYER*/
	
	/*TREE*/
	Graphics::NormalMaterial material;
	unsigned int renderID = scene.createRenderInstance(model, material);

	entt::entity tree = e.create();
	e.assign<Component::Position>(tree, glm::vec3(0, 15, 0));
	e.assign<Component::Rotation>(tree, glm::quat(glm::vec3(0, 0, 0)));
	e.assign<Component::Scale>(tree, glm::vec3(.1, .1, .1));
	e.assign<Component::RotationalVelocity>(tree, glm::vec3(0, 1, 0));
	e.assign<Component::RenderInstance>(tree, renderID);
	/*TREE*/

	/*WALKER*/
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
	e.assign<Component::CameraInstance>(sunID, cameraID);
	e.assign<Component::RenderInstance>(sunID, scene.createRenderInstance(walkerModel, walkerMaterial));
	/*SUN CAMERA*/

	/*NET*/
	//e.attemptConnection("3.86.12.190", 60000);		//AWS
	e.attemptConnection("127.0.0.1", 60000);		//LOCAL
	/*NET*/

	

}

void PlayState::cleanUp() {
	LOG_STATE("cleanUp");
}

void PlayState::entered() {
	LOG_STATE("entered");
	/*SYSTEM*/
	e.addSystem(StatelessSystem(System::shader_update_system, std::chrono::seconds(1)));
	e.addSystem(StatelessSystem(System::netword_system));

	e.addSystem(StatelessSystem(System::collision_test_system));
	
	e.addSystem(StatelessSystem(System::movement_system));
	e.addSystem(StatelessSystem(System::friction_system));

	e.addSystem(StatelessSystem(System::input_system));

	e.addSystem(StatelessSystem(System::player_state_system));
	e.addSystem(StatelessSystem(System::sprint_system));

	e.addSystem(StatelessSystem(System::voxel_system));
	e.addSystem(StatelessSystem(System::send_packet_system));
	e.addSystem(StatelessSystem(System::render_system));
	
	/*SYSTEM*/
}

void PlayState::leaving() {
	LOG_STATE("leaving");

}



void PlayState::update(float delta) {
	static bool can_update = true;

	if (Window::isPressed('p')) {
		can_update = false;
	}
	else if (Window::isPressed('q')) {
		can_update = true;
	}
	if (Window::isPressed('t')) {
		e.replace<Component::Position>(e.getPlayer(), glm::vec3(10, 20, 10));
		e.replace<Component::Velocity>(e.getPlayer(), glm::vec3(0, 0, 0));
	}
	
	if(can_update)
		this->e.update(delta);
}

