#pragma once
#include <set>
#include "common.h"
#include "entt.hpp"
#include "RakPeerInterface.h"
#include "System.h"	//todo remove this
#include "LinearAllocator.h"

#define MEM_ALLOC_SIZE 1024 * 1024 * 1024
#define SERVER_PORT = 60000;

namespace Graphics {
	class OpenGLRenderer;
	class Scene;
	class ModelGeometry;
}

namespace Voxel {
	class ChunkManager;
	class ChunkRenderDataManager;
}

//todo rename this as client
class Engine : public entt::registry {
private:
	//MAIN PLAYER
	entt::entity main;
	//MAIN PLAYER

	//NET
	SLNet::RakPeerInterface *peer;
	//NET

	//SYSTEMS
	Util::Allocator::LinearAllocator linearAlloc;
	Voxel::ChunkManager *chunkManager;
	Voxel::ChunkRenderDataManager *chunkRenderDataManager;
	Graphics::OpenGLRenderer *renderer;
	Graphics::Scene *scene;
	std::set<System*, System::classcomp> systems;
	//SYSTEMS


public:
	Engine();
	~Engine();

	void update(float delta);
	void addSystem(System *system);
	void removeSystem(System *system);
	void updateSystems(float delta);
	void deleteAllActiveSystems();

	bool attemptConnection(const char *ip, uint16 port);
	bool isConnected();

	entt::entity addPlayer(float x, float y, float z);
	entt::entity getPlayer();
	Util::Allocator::LinearAllocator& getAllocator();
	Voxel::ChunkManager* getChunkManager();
	Voxel::ChunkRenderDataManager* getChunkRenderDataManager();
	Graphics::OpenGLRenderer* getRenderer();
	Graphics::Scene* getScene();
};

