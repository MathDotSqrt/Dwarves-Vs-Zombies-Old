#pragma once
#include "entt.hpp"
#include "LinearAllocator.h"
#include <vector>

namespace SLNet {
	class RakPeerInterface;
}

namespace Voxel {
	class ChunkManager;
}

class EntityAdmin {
public:
	typedef void(*function_ptr)(EntityAdmin &admin, float delta);

	
	entt::registry registry;
	EntityAdmin();
	~EntityAdmin();

	void update(float delta);
	void addSystemUpdateFunction(function_ptr func);

	SLNet::RakPeerInterface* getPeer();
	Voxel::ChunkManager& getChunkManager();

private:
	SLNet::RakPeerInterface *peer;
	std::vector<function_ptr> updateFunctions;
	Util::Allocator::LinearAllocator allocator;

};

