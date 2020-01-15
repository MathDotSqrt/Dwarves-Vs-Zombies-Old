#pragma once
#include "MathUtil.h"
#include "../DVZ_common/ChunkConstants.h"
#include "Allocator.h"
#include "PoolAllocator.h"
#include "Block.h"



namespace Voxel {
	class ChunkManager;
	
	struct FlatVoxelContainer {
		Block blocks[CHUNK_VOLUME];
	};


	class Chunk {
	public:
		constexpr static size_t MAX_INDEX = CHUNK_VOLUME;
		
		const int cx;
		const int cy;
		const int cz;

		int mod_count = 0;

		ChunkManager &manager;
		FlatVoxelContainer *flat;

		Chunk(int cx, int cy, int cz, ChunkManager &manager);	//todo disable move
		Chunk(const Chunk &other) = delete;
		Chunk(Chunk &&other);
		~Chunk();

		Chunk& operator=(const Chunk &other) = delete;
		Chunk& operator=(Chunk &&other) = delete;

		constexpr int32 getHashCode() const {
			return Util::zorder_hash(cx, cy, cz);
		}

		constexpr int32 toIndex(const int x, const int y, const int z) const {
			//return x + CHUNK_WIDTH_X * (y + CHUNK_WIDTH_Y * z);
			return x + CHUNK_WIDTH_X * (z + CHUNK_WIDTH_Z * y);
		}

		Block getBlock(const int x, const int y, const int z) const;
		Block getBlock(const int i) const;

		void setBlock(const int x, const int y, const int z, Block block);
		void setBlock(const int i, Block block);

		int getModCount() const;
	};
}
	