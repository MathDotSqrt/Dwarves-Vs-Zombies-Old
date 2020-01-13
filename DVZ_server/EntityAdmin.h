#pragma once
#include "entt.hpp"
#include "LinearAllocator.h"
#include "StatelessSystem.h"
#include <vector>

namespace SLNet {
	class RakPeerInterface;
}

namespace Voxel {
	class ChunkManager;
}

class EntityAdmin {
public:
	entt::registry registry;
	EntityAdmin();
	~EntityAdmin();

	void update(float delta);
	void addSystemUpdateFunction(StatelessSystem system);

	SLNet::RakPeerInterface* getPeer();
	Voxel::ChunkManager& getChunkManager();

private:
	SLNet::RakPeerInterface *peer;
	std::vector<StatelessSystem> updateFunctions;
	Util::Allocator::LinearAllocator allocator;

};

