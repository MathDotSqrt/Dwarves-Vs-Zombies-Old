#pragma once
#include <set>
#include "entt.hpp"
#include "RakPeerInterface.h"
#include "System.h"	//todo remove this
#include "LinearAllocator.h"

#define MEM_ALLOC_SIZE 1024 * 1024 * 1024

namespace Graphics {
	class OpenGLRenderer;
	class Scene;
	class ModelGeometry;
}

namespace Voxel {
	class ChunkManager;
	class ChunkRenderDataManager;
}

class Engine : public entt::registry {
private:
	//MAIN PLAYER
	entt::entity main;
	//MAIN PLAYER

	//NET
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

	entt::entity addPlayer(float x, float y, float z);
	
	entt::entity getNetEntity(entt::entity netID);
	
	void addSystem(System *system);
	void removeSystem(System *system);
	void updateSystems(float delta);
	void deleteAllActiveSystems();

	entt::entity getPlayer();

	//todo refactor all of this
	Util::Allocator::LinearAllocator& getAllocator();
	Voxel::ChunkManager* getChunkManager();
	Voxel::ChunkRenderDataManager* getChunkRenderDataManager();
	Graphics::OpenGLRenderer* getRenderer();
	Graphics::Scene* getScene();
};

