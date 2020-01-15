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
		friend class Chunk;
		Util::Allocator::PoolAllocator voxel_allocator;                                               
		std::vector<Chunk> world;
	public:
		constexpr static int WORLD_X_LENGTH = 16;
		constexpr static int WORLD_Z_LENGTH = 16;
		constexpr static int MIN_CHUNK_X = 1 - (WORLD_X_LENGTH + 1) / 2;
		constexpr static int MIN_CHUNK_Z = 1 - (WORLD_Z_LENGTH + 1) / 2;
		constexpr static int MAX_CHUNK_X = (WORLD_Z_LENGTH) / 2;
		constexpr static int MAX_CHUNK_Z = (WORLD_Z_LENGTH) / 2;

		ChunkManager(Util::Allocator::IAllocator &parent);
		~ChunkManager();

		void update(EntityAdmin &admin, float delta);

		Chunk& getChunk(int cx, int cz);
		const Chunk& getChunk(int cx, int cz) const;

		void setBlock(int x, int y, int z, Block block);
		Block getBlock(int x, int y, int z) const;
	private:
		FlatVoxelContainer* allocFlatVoxel();	//todo figure out why alloc isnt working
		void freeFlatVoxel(FlatVoxelContainer *ptr);	//todo figure out why alloc isnt working
	};
}
/*SERVER*/
