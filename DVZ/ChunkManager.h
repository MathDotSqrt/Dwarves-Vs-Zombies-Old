#pragma once
#include <unordered_map>
#include <thread>
#include <atomic>
#include "concurrentqueue.h"
#include "Block.h"
#include "Chunk.h"
#include "ChunkRenderData.h"
#include "ChunkMesher.h"

#include "ThreadPool.h"

#include "LinearAllocator.h"
#include "PoolAllocator.h"
#include "AllocatorHandle.h"

#include "Recycler.h"

namespace Voxel{

typedef Util::Allocator::Handle<Chunk> ChunkHandle;

struct ChunkNeighbors {
	ChunkHandle middle;
	ChunkHandle front;
	ChunkHandle back;
	ChunkHandle left;
	ChunkHandle right;
	ChunkHandle up;
	ChunkHandle down;
};

class ChunkManager {
public:
	typedef std::unordered_map<int, ChunkHandle>::iterator ChunkIterator;
	typedef std::unordered_map<int, ChunkRenderData*>::iterator ChunkRenderDataIterator;

	static const int CHUNK_ALLOC_SIZE = 40 * 1024 * 1024;
	static const int CHUNK_MESHER_ALLOC_SIZE = 8 * 1024 * 1024;
	static const int CHUNK_MESH_RECYCLE_SIZE = 2 * 1024 * 1024;
	static const int CHUNK_RENDER_DATA_RECYCLE_SIZE = 2 * 1024 * 1024;
	static const int CHUNK_THREAD_POOL_SIZE = 1;	//dont change this until i fix allocate array
	static const int RENDER_DISTANCE = 15;

	ChunkManager(Util::Allocator::IAllocator &parent);
	~ChunkManager();

	//todo add frustum
	void update(float x, float y, float z);

	void chunkLoader(ChunkNeighbors neighbors, Chunk::BlockGeometry*);

	inline ChunkIterator begin() {
		return this->chunkSet.begin();
	}

	inline ChunkIterator end() {
		return this->chunkSet.end();
	}

	inline ChunkRenderDataIterator beginRenderData() {
		return this->renderDataSet.begin();
	}
	inline ChunkRenderDataIterator endRenderData() {
		return this->renderDataSet.end();
	}

	inline ChunkIterator removeChunk(const ChunkIterator& iter);
	void removeChunk(int cx, int cy, int cz);

	int hashcode(int i, int j, int k);

	ChunkHandle getChunk(int cx, int cy, int cz);
	ChunkHandle getChunkIfMapped(int cx, int cy, int cz);
	ChunkNeighbors getChunkNeighbors(ChunkHandle chunk);
	ChunkNeighbors getChunkNeighbors(int cx, int cy, int cz);

	bool isChunkMapped(int cx, int cy, int cz);

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
	Util::Threading::ThreadPool pool;

	Util::Allocator::PoolAllocator chunkPoolAllocator;
	Util::Allocator::LinearAllocator chunkMesherAllocator;

	Util::Recycler<ChunkRenderData> renderDataRecycler;
	Util::Recycler<Chunk::BlockGeometry> meshRecycler;

	moodycamel::ConcurrentQueue <std::pair<Chunk::BlockGeometry*, glm::ivec3>> chunkMeshQueue;

	//moodycamel::ConcurrentQueue<Chunk*> chunkReadyQueue;

	std::unordered_map<int, ChunkHandle> chunkSet;
	std::unordered_map<int, ChunkRenderData*> renderDataSet;

	ChunkMesher *chunkMesherArray;

	int expand(int x);

};

}

