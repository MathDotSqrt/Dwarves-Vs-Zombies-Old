#pragma once
#include "PoolAllocator.h"

/*SERVER*/
namespace Voxel {
	class ChunkManager {
	private:
		Util::Allocator::PoolAllocator chunk_allocator;
	
	public:
		ChunkManager(Util::Allocator::IAllocator &parent);
		~ChunkManager();
	};
}
/*SERVER*/
