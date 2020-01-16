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

		bool operator==(const ChunkModEvent &other) const {
			return index == other.index && block == other.block;
		}
	};

	class Chunk {
	public:
		constexpr static size_t MAX_INDEX = CHUNK_VOLUME;
		constexpr static size_t MOD_BUFFER_SIZE = 32;

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
		
		//Tests if modification buffer contains all data necessary to send only block deltas to client. 
		//mod_delta is how many modifications in the past to look back.
		//if mod_delta is larger than buffer size, return false because we dont have enough data to 
		//send block place packets.
		//if we occure any FullModified ChunkModEvents within mod_delta, also return false
		bool isChunkModHistoryComplete(size_t mod_delta) const;		//todo rename this

		int getModCount() const;
		const Util::RingBuffer<ChunkModEvent, MOD_BUFFER_SIZE>& getModBuffer() const;

	private:
		void setBlockInternal(const int x, const int y, const int z, Block block);
		void setBlockInternal(const int index, Block block);

		int mod_count = 0;
		Util::RingBuffer<ChunkModEvent, MOD_BUFFER_SIZE> mod_buffer;

		ChunkManager &manager;
		FlatVoxelContainer *flat;
	};
}
	