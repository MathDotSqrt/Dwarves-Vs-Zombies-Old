#pragma once
#include "MathUtil.h"
#include "../DVZ_common/ChunkConstants.h"
#include "Allocator.h"
#include "PoolAllocator.h"
#include "Block.h"
#include "RingBuffer.h"


namespace Voxel {
	class ChunkManager;
	
	struct FlatVoxelContainer {
		Block blocks[CHUNK_VOLUME];
	};

	//a ring buffer of all chunk events will be stored as elements of ChunkModEvent
	//if ChunkModEvent::isFullChunkModified returns true: send entire chunk over wire
	struct ChunkModEvent {
		uint32 index;
		Block block;

		ChunkModEvent() : index(0), block(BlockType::BLOCK_TYPE_NUM_TYPES) {}
		ChunkModEvent(uint32 index, Block block) : index(index), block(block) {}

		bool isFullChunkModified() {
			//if block equals this impossible type. entire chunk has been modified
			return block == Block(BlockType::BLOCK_TYPE_NUM_TYPES);
		}
	};

	class Chunk {
	public:
		constexpr static size_t MAX_INDEX = CHUNK_VOLUME;
		
		const int cx;
		const int cy;
		const int cz;


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

		void flagEntireChunkModified();


		int getModCount() const;
		const Util::RingBuffer<ChunkModEvent, 32>& getModBuffer() const;

	private:
		void setBlockInternal(const int x, const int y, const int z, Block block);
		void setBlockInternal(const int index, Block block);

		int mod_count = 0;
		Util::RingBuffer<ChunkModEvent, 32> mod_buffer;

		ChunkManager &manager;
		FlatVoxelContainer *flat;
	};
}
	