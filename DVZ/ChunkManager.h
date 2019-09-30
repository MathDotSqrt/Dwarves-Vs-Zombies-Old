#pragma once
#include <unordered_map>
#include <thread>
#include <atomic>
#include "concurrentqueue.h"
#include "Block.h"
#include "Chunk.h"
#include "ThreadPool.h"

#include "PoolAllocator.h"
#include "LinearAllocator.h"

namespace Voxel{
	struct Chunk2 {
		int cx, cy, cz;
		Block *data;
	};


class ChunkManager {
public:
	static const int CHUNK_ALLOC_SIZE = 48 * 1024 * 1024;
	static const int CHUNK_MESHER_ALLOC_SIZE = 8 * 1024 * 1024;
	static const int CHUNK_THREAD_POOL_SIZE = 1;

	static const int RENDER_DISTANCE = 15;

private:
	Util::Allocator::PoolAllocator chunkAllocator;
	Util::Allocator::LinearAllocator chunkMesherAllocator;
	Util::Threading::ThreadPool pool;
	
	moodycamel::ConcurrentQueue<Chunk*> chunkReadyQueue;

public:
	typedef std::unordered_map<int, Chunk*>::iterator ChunkIterator;

	ChunkManager(Util::Allocator::IAllocator &parent);
	~ChunkManager();

	//todo add frustum
	void update(float x, float y, float z);

	void chunkLoader(Chunk *);

	inline ChunkIterator begin() {
		return this->chunkSet.begin();
	}

	inline ChunkIterator end() {
		return this->chunkSet.end();
	}

	inline ChunkIterator removeChunk(const ChunkIterator& iter);
	void removeChunk(int cx, int cy, int cz);

	int hashcode(int i, int j, int k);

	Chunk* getChunk(int cx, int cy, int cz);
	Chunk* generateChunk(int cx, int cy, int cz);
	//Chunk* setChunk(int cx, int cy, int cz, Block *data);

	bool isChunkMapped(int cx, int cy, int cz);
	bool isChunkQueued(int cx, int cy, int cz);

	Block& getBlock(int x, int y, int z);
	void setBlock(int x, int y, int z, Block &block);
	bool isBlockMapped(int x, int y, int z);

	Block& getBlock(float x, float y, float z);
	void setBlock(float x, float y, float z, Block &block);

	int getBlockX(float x);
	int getBlockY(float y);
	int getBlockZ(float z);

	int getChunkX(float x);
	int getChunkY(float y);
	int getChunkZ(float z);

private:
	std::unordered_map<int, Chunk*> chunkSet;
	std::unordered_map<int, Chunk*> chunkQueuedSet;
	int expand(int x);

};

}

