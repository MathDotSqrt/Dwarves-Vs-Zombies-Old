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

class ChunkHandle;

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
	typedef Chunk* ChunkPtr;
	
	typedef std::unordered_map<int, ChunkHandle>::iterator ChunkIterator;
	typedef std::unordered_map<int, ChunkRenderData*>::iterator ChunkRenderDataIterator;

	static const int CHUNK_ALLOC_SIZE = 500 * 1024 * 1024;
	static const int CHUNK_MESH_RECYCLE_SIZE = 2 * 1024 * 1024;
	static const int CHUNK_RENDER_DATA_RECYCLE_SIZE = 2 * 1024 * 1024;
	static const int CHUNK_MESHER_ALLOC_SIZE = 8 * 1024 * 1024;
	static const int CHUNK_THREAD_POOL_SIZE = 1;	//dont change this until i fix allocate array
	static const int RENDER_DISTANCE = 30;

	ChunkManager(Util::Allocator::IAllocator &parent);
	~ChunkManager();

	//todo add frustum
	void update(float x, float y, float z);

	ChunkHandle getChunkReference();

	ChunkHandle loadChunk(int cx, int cy, int cz);
	bool loadChunkAsync(int cx, int cy, int cz);
	bool loadChunkAsync(int cx, int cy, int cz, void *callback);

	ChunkHandle getChunk(int cx, int cy, int cz);
	ChunkHandle getChunkIfMapped(int cx, int cy, int cz);
	ChunkNeighbors getChunkNeighbors(ChunkHandle chunk);

	bool isChunkMapped(int cx, int cy, int cz);
	bool isBlockMapped(int x, int y, int z);


	Block getBlock(int x, int y, int z);
	void setBlock(int x, int y, int z, Block block);
	Block getBlock(float x, float y, float z);
	void setBlock(float x, float y, float z, Block block);

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
private:
	ChunkHandle newChunk(int cx, int cy, int cz);
	ChunkIterator removeChunk(const ChunkIterator& iter);
	void removeChunk(int cx, int cy, int cz);


	void loadChunks(int chunkX, int chunkY, int chunkZ, int renderDistance);
	void updateAllChunks(int playerCX, int playerCY, int playerCZ);
	void dequeueChunkRenderData();

	void chunkGeneratorThread();
	void chunkMeshingThread();

	int hashcode(int i, int j, int k);

	struct ChunkDestructor {
		ChunkDestructor(ChunkManager *manager){
			this->manager = manager;
		}

		void operator()(Chunk* value) {
			manager->chunkRecycler.recycle(value);
		}

	private:
		ChunkManager *manager;
	};

	std::atomic<bool> runThreads = true;
	std::thread generatorThread;
	std::thread mesherThread[CHUNK_THREAD_POOL_SIZE];

	Util::Recycler<Chunk> chunkRecycler;
	Util::Recycler<ChunkGeometry> meshRecycler;
	Util::Recycler<ChunkRenderData> renderDataRecycler;
	
	Util::Allocator::LinearAllocator chunkMesherAllocator;
	
	moodycamel::BlockingConcurrentQueue<ChunkHandle> chunkGenQueue;
	moodycamel::BlockingConcurrentQueue<std::pair<ChunkNeighbors, ChunkGeometry*>> chunkMeshingQueue;
	moodycamel::ConcurrentQueue<std::pair<ChunkGeometry*, glm::ivec3>> chunkMeshQueue;

	//chunkSet
	//generateChunkList
	//meshChunkList
	//greedyMeshChunkList
	//renderableChunkSet
	//visibleChunkList
	//deleteConcurrentQueue

	std::unordered_map<int, ChunkHandle> chunkSet;
	std::unordered_map<int, ChunkRenderData*> renderDataSet;
	std::unordered_map<int, bool> chunkQueuedSet;

	ChunkMesher *chunkMesherArray;

	int currentChunkX = 0;
	int currentChunkY = 0;
	int currentChunkZ = 0;

	int expand(int x);

};

class ChunkHandle : std::unique_ptr<Chunk>{
public:
	friend class ChunkManager;

private:
	ChunkHandle(ChunkManager *manager, ChunkManager::ChunkPtr ptr) : unique_ptr(ptr){
	
	}

	Chunk *chunk_ptr;

};
}

