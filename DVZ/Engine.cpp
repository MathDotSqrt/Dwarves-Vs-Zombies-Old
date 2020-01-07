
#include "Engine.h"

#include <thread>
#include <list>
#include <algorithm>
#include <stdlib.h>

#include "Components.h"
#include "Scene.h"
#include "OpenGLRenderer.h"
#include "QuadGeometry.h"
#include "ChunkManager.h"
#include "ChunkRenderDataManager.h"
#include "Timer.h"

#include "ResourceManager.h"
#include "Mesh.h"

//#define LINEAR_ALLOC_SIZE 100

using namespace std;

Engine::Engine() : linearAlloc(MEM_ALLOC_SIZE, malloc(MEM_ALLOC_SIZE)){	//todo delete
	this->set<SLNet::RakPeerInterface*>(SLNet::RakPeerInterface::GetInstance())->ApplyNetworkSimulator(0, 0, 0);
	this->set<Graphics::Scene>();
	this->set<Graphics::OpenGLRenderer>(&this->ctx<Graphics::Scene>());
	this->set<Voxel::ChunkManager>(linearAlloc);
	this->set<Voxel::ChunkRenderDataManager>(linearAlloc);

	this->set<ResourceManager::TexCache>();
	this->set<ResourceManager::GeometryCache>();
	this->set<ResourceManager::MeshCache>();

	this->main = entt::null;

	this->set<int>();
	this->set<std::unordered_map<entt::entity, entt::entity>>();
}


Engine::~Engine(){
	auto peer = this->ctx<SLNet::RakPeerInterface*>();
	peer->Shutdown(100, 0, PacketPriority::HIGH_PRIORITY);
	SLNet::RakPeerInterface::DestroyInstance(peer);
}

void Engine::update(float delta) {
	{
		Util::Performance::Timer timer("Update Systems");
		this->updateSystems(delta);
	}

	{
		auto &renderer = this->ctx<Graphics::OpenGLRenderer>();
		auto &chunkRenderDataManager = this->ctx<Voxel::ChunkRenderDataManager>();
		Util::Performance::Timer render("Render");
		renderer.prerender();
		renderer.render(&chunkRenderDataManager);
	}

}


void Engine::addSystem(StatelessSystem system) { 
	systems.push_back(system);
}

void Engine::updateSystems(float delta) {
	for (auto &system : systems) {
		system.update(*this, delta);
	}
}


bool Engine::attemptConnection(const char *ip, uint16 port) {
	auto peer = this->ctx<SLNet::RakPeerInterface*>();
	peer->Startup(1, &SLNet::SocketDescriptor(), 1);
	auto result = peer->Connect(ip, port, nullptr, 0);
	return result = SLNet::ConnectionAttemptResult::CONNECTION_ATTEMPT_STARTED;
}

bool Engine::isConnected() {
	auto peer = this->ctx<SLNet::RakPeerInterface*>();
	return peer->NumberOfConnections() > 0;
}

entt::entity Engine::addPlayer(float x, float y, float z) {
	if (this->main != entt::null) {
		LOG_WARNING("Attempting to make two main entities...");
		return this->main;
	}

	entt::entity id = this->create();
	this->assign<PlayerComponent>(id);
	this->assign<PositionComponent>(id, glm::vec3(x, y, z));
	this->assign<RotationComponent>(id, glm::quat(1, 0, 0, 0));
	this->assign<VelocityComponent>(id, glm::vec3(0, 0, 0));
	this->assign<RotationalVelocityComponent>(id, glm::vec3(0, 0, 0));
	this->assign<DirComponent>(id, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
	this->assign<InputComponent>(id);
	this->assign<CameraInstanceComponent>(id, this->ctx<Graphics::Scene>().getMainCameraID());

	this->main = id;

	return id;
}

entt::entity Engine::getPlayer() {
	return this->main;
}

