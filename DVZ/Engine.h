#pragma once
#include <set>
#include <vector>
#include <chrono>
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

class StatelessSystem {
private:
	typedef void(*function_ptr)(Engine &engine, float delta);
	typedef std::chrono::duration<float> FrameTime;

	function_ptr ptr;
	FrameTime intervalTime;
	FrameTime currentTimeDuration;
public:
	StatelessSystem(function_ptr ptr, FrameTime interval = FrameTime(0)) : ptr(ptr), intervalTime(interval), currentTimeDuration(0s){
	
	}

	void update(Engine &engine, float delta) {
		currentTimeDuration += FrameTime(delta);

		if (currentTimeDuration >= intervalTime) {
			currentTimeDuration = FrameTime(0s);

			ptr(engine, delta);
		}
	}
};

//todo rename this as client
class Engine : public entt::registry {
private:
	typedef void(*function_ptr)(Engine &engine, float delta);

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

	std::vector<std::pair<function_ptr, std::chrono::time_point<std::chrono::milliseconds>>> systems;
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
	SLNet::RakPeerInterface* getPeer();
};

