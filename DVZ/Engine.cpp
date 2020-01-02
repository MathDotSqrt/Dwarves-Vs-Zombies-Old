#include "Engine.h"

#include <thread>
#include <list>
#include <algorithm>

#include "GamePacketID.h"
#include "BitStream.h"

#include "Components.h"
#include "System.h"
#include "Scene.h"
#include "OpenGLRenderer.h"
#include "QuadGeometry.h"
#include "ModelGeometry.h"
#include "ChunkManager.h"
#include "ChunkRenderDataManager.h"
#include "Timer.h"
#include <stdlib.h>

//#define LINEAR_ALLOC_SIZE 100

using namespace std;

Engine::Engine() : linearAlloc(MEM_ALLOC_SIZE, malloc(MEM_ALLOC_SIZE)){	//todo delete
	//todo linearly alloc all of this
	this->peer = SLNet::RakPeerInterface::GetInstance();

	this->scene = new Graphics::Scene();
	this->renderer = new Graphics::OpenGLRenderer();
	this->renderer->init(scene);
	this->chunkManager = new Voxel::ChunkManager(this->linearAlloc);
	this->chunkRenderDataManager = new Voxel::ChunkRenderDataManager(this->linearAlloc);

	this->main = entt::null;
}


Engine::~Engine(){
	delete this->scene;
	delete this->renderer;
	delete this->chunkManager;
	delete this->chunkRenderDataManager;

	this->deleteAllActiveSystems();
}

void Engine::update(float delta) {
	{
		Util::Performance::Timer timer("Update Systems");
		this->updateSystems(delta);
	}

	{
		Util::Performance::Timer render("Render");
		this->renderer->prerender();
		this->renderer->render(this->chunkRenderDataManager);
	}

}

void Engine::addSystem(System *system) {
	if (std::find(this->systems.begin(), this->systems.end(), system) == this->systems.end()) {
		//LOG_ENGINE("System: %d Index: %d", system->getPriority(), std::upper_bound(this->systems.begin(), this->systems.end(), system, system->operator<));
		this->systems.insert(system);	//TODO fix bug where multiple systems cannot have same priority
		
		//this->systems.push_back(system);
		system->addedToEngine(this);
	}
}

void Engine::removeSystem(System *system) {
	if (find(this->systems.begin(), this->systems.end(), system) != this->systems.end()) {
		system->removedFromEngine(this);
		//this->systems.erase(std::remove(systems.begin(), systems.end(), system));
		this->systems.erase(find(this->systems.begin(), this->systems.end(), system));
	}
}

void Engine::updateSystems(float delta) {
	for (auto *system : this->systems) {
		system->update(this, delta);
	}
}

void Engine::deleteAllActiveSystems() {
	for (System *system : this->systems) {
		system->removedFromEngine(this);
		delete system;
	}

	this->systems.clear();
}

bool Engine::attemptConnection(const char *ip, uint16 port) {
	peer->Startup(1, &SLNet::SocketDescriptor(), 1);
	auto result = peer->Connect(ip, port, nullptr, 0);
	return result = SLNet::ConnectionAttemptResult::CONNECTION_ATTEMPT_STARTED;
}

entt::entity Engine::addPlayer(float x, float y, float z) {
	if (this->main != entt::null) {
		LOG_WARNING("Attempting to make two main entities...");
		return this->main;
	}

	entt::entity id = this->create();
	this->assign<PositionComponent>(id, glm::vec3(x, y, z));
	this->assign<RotationComponent>(id, glm::quat(1, 0, 0, 0));
	this->assign<VelocityComponent>(id, glm::vec3(0, 0, 0));
	this->assign<RotationalVelocityComponent>(id, glm::vec3(0, 0, 0));
	this->assign<DirComponent>(id, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
	this->assign<InputComponent>(id);
	this->assign<CameraInstanceComponent>(id, this->scene->getMainCameraID());
	this->assign<NetworkComponent>(id);

	this->main = id;

	return id;
}

entt::entity Engine::getPlayer() {
	return this->main;
}

Util::Allocator::LinearAllocator& Engine::getAllocator() {
	return this->linearAlloc;
}

Voxel::ChunkManager* Engine::getChunkManager() {
	return this->chunkManager;
}

Voxel::ChunkRenderDataManager* Engine::getChunkRenderDataManager() {
	return chunkRenderDataManager;
}

Graphics::OpenGLRenderer* Engine::getRenderer() {
	return this->renderer;
}

Graphics::Scene* Engine::getScene() {
	return this->scene;
}
