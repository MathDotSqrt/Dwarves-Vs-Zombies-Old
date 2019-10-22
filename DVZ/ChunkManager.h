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

	static const int CHUNK_ALLOC_SIZE = 200 * 1024 * 1024;
	static const int CHUNK_MESHER_ALLOC_SIZE = 8 * 1024 * 1024;
	static const int CHUNK_MESH_RECYCLE_SIZE = 2 * 1024 * 1024;
	static const int CHUNK_RENDER_DATA_RECYCLE_SIZE = 2 * 1024 * 1024;
	static const int CHUNK_THREAD_POOL_SIZE = 1;	//dont change this until i fix allocate array
	static const int RENDER_DISTANCE = 30;

	ChunkManager(Util::Allocator::IAllocator &parent);
	~ChunkManager();

	//todo add frustum
	void update(float x, float y, float z);

	void chunkGeneratorThread();
	void chunkMeshingThread();

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

	void newAllocChunk(int cx, int cy, int cz);

	inline ChunkIterator removeChunk(const ChunkIterator& iter);
	void removeChunk(int cx, int cy, int cz);

	int hashcode(int i, int j, int k);

	ChunkHandle getChunk(int cx, int cy, int cz);
	ChunkHandle getChunkIfMapped(int cx, int cy, int cz);
	ChunkNeighbors getChunkNeighbors(ChunkHandle chunk);
	ChunkNeighbors getChunkNeighbors(int cx, int cy, int cz);

	bool isChunkMapped(int cx, int cy, int cz);

	Block getBlock(int x, int y, int z);
	void setBlock(int x, int y, int z, Block block);
	bool isBlockMapped(int x, int y, int z);

	Block getBlock(float x, float y, float z);
	void setBlock(float x, float y, float z, Block block);

	int getBlockX(float x);
	int getBlockY(float y);
	int getBlockZ(float z);

	int getChunkX(float x);
	int getChunkY(float y);
	int getChunkZ(float z);



private:
	std::atomic<bool> runThreads = true;
	std::thread generatorThread;
	std::thread mesherThread[CHUNK_THREAD_POOL_SIZE];

	//Util::Threading::ThreadPool pool;
	Util::Allocator::PoolAllocator chunkPoolAllocator;			//todo fix chunk alloc assert bug
	Util::Allocator::LinearAllocator chunkMesherAllocator;

	Util::Recycler<ChunkRenderData> renderDataRecycler;
	Util::Recycler<ChunkGeometry> meshRecycler;

	moodycamel::BlockingConcurrentQueue<ChunkHandle> chunkGenQueue;
	moodycamel::BlockingConcurrentQueue<std::pair<ChunkNeighbors, ChunkGeometry*>> chunkMeshingQueue;
	moodycamel::ConcurrentQueue<std::pair<ChunkGeometry*, glm::ivec3>> chunkMeshQueue;
	

	std::unordered_map<int, ChunkHandle> chunkSet;
	std::unordered_map<int, ChunkRenderData*> renderDataSet;
	std::unordered_map<int, bool> chunkQueuedSet;

	ChunkMesher *chunkMesherArray;

	int currentChunkX = 0;
	int currentChunkY = 0;
	int currentChunkZ = 0;

	int expand(int x);

};

}

