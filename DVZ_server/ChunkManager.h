#pragma once
#include "Common.h"
#include "PoolAllocator.h"
#include "Chunk.h"

#include <unordered_map>
#include <queue>

class EntityAdmin;

/*SERVER*/
namespace Voxel {
	class ChunkManager {
	private:
		Util::Allocator::PoolAllocator chunk_allocator;
		std::unordered_map<int32, Chunk> chunkSet;
	public:
		ChunkManager(Util::Allocator::IAllocator &parent);
		~ChunkManager();

		void update(EntityAdmin &admin, float delta);
	};
}
/*SERVER*/
