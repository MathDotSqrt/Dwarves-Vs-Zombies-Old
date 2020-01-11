#pragma once
#include <set>
#include <vector>
#include <chrono>
#include "common.h"
#include "entt.hpp"
#include "RakPeerInterface.h"
#include "StatelessSystem.h"
#include "LinearAllocator.h"
#include "glm.hpp"

#define MEM_ALLOC_SIZE 1024 * 1024 * 1024
#define SERVER_PORT = 60000;

namespace Graphics {
	class OpenGLRenderer;
	class Scene;
}

namespace Voxel {
	class ChunkManager;
	class ChunkRenderDataManager;
	struct Block;
}

//todo rename this as client
class Engine : public entt::registry {
private:

	//MAIN PLAYER
	entt::entity main;
	//MAIN PLAYER

	//NET
	//SLNet::RakPeerInterface *peer;
	//NET

	//SYSTEMS
	Util::Allocator::LinearAllocator linearAlloc;
	/*Voxel::ChunkManager *chunkManager;
	Voxel::ChunkRenderDataManager *chunkRenderDataManager;
	Graphics::OpenGLRenderer *renderer;
	Graphics::Scene *scene;*/
	//SYSTEMS

	std::vector<StatelessSystem> systems;
public:
	Engine();
	~Engine();

	void update(float delta);

	void addSystem(StatelessSystem system);
	void updateSystems(float delta);

	bool attemptConnection(const char *ip, uint16 port);
	bool isConnected();

	entt::entity addPlayer(float x, float y, float z);
	entt::entity getPlayer();

	std::vector<std::pair<glm::i32vec3, Voxel::Block>>& getBlockPlaceBuffer();
	
};

