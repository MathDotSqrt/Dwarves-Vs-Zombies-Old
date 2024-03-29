#pragma once

#include "Block.h"
#include "Chunk.h"
#include "ChunkRefHandle.h"
#include "ChunkMesher.h"
#include "ChunkLightEngine.h"
#include "RLDecoder.h"

#include "LinearAllocator.h"
#include "PoolAllocator.h"
#include "AllocatorHandle.h"

#include "ChunkQueueSet.h"
#include "concurrentqueue.h"
#include "ThreadPool.h"
#include <thread>
#include <unordered_map>
#include <queue>

namespace Voxel{



//struct ChunkNeighbors {
//	ChunkRefHandle middle;
//	ChunkRefHandle front;
//	ChunkRefHandle back;
//	ChunkRefHandle left;
//	ChunkRefHandle right;
//	ChunkRefHandle up;
//	ChunkRefHandle down;
//};

struct ChunkNeighbors {
	ChunkRefHandle backLeft;
	ChunkRefHandle back;
	ChunkRefHandle backRight;
	
	ChunkRefHandle left;
	ChunkRefHandle middle;
	ChunkRefHandle right;
	
	ChunkRefHandle frontLeft;
	ChunkRefHandle front;
	ChunkRefHandle frontRight;

	constexpr const ChunkRefHandle& getChunk(int cx, int cz) const{
		ChunkRefHandle* handle = (ChunkRefHandle*)(this);
		cx += 1;
		cz += 1;
		return *(handle + (cx + cz * 3));
	}
};

struct BlockRayCast {
	Block block;
	int x, y, z;
	int nx, ny, nz;
};

class ChunkManager {
public:							//fix bug of things not destructing in order
	friend class Chunk;
	friend class ChunkMesher;
	friend class ChunkLightEngine;

	static const int CHUNK_ALLOC_SIZE = 1000 * 1024 * 1024;
	static const int CHUNK_MESH_RECYCLE_SIZE = 4 * 1024 * 1024;
	static const int CHUNK_RENDER_DATA_RECYCLE_SIZE = 4 * 1024 * 1024;
	static const int CHUNK_MESHER_ALLOC_SIZE = 8 * 1024 * 1024;
	static const int CHUNK_THREAD_POOL_SIZE = 1;	//dont change this until i fix allocate array
	//static const int RENDER_DISTANCE = 10;
	static const int RENDER_DISTANCE = 50;
	static const int LOAD_DISTANCE = RENDER_DISTANCE + 2;

public:
	ChunkManager(Util::Allocator::IAllocator &parent);
	~ChunkManager();

	//todo add frustum
	void update(float x, float y, float z);

	ChunkRefHandle getChunk(int cx, int cy, int cz);
	ChunkRefHandle getChunkIfMapped(int cx, int cy, int cz);
	ChunkRefHandle getNullChunk();
	ChunkNeighbors getChunkNeighborsIfMapped(int cx, int cy, int cz);
	ChunkNeighbors getChunkNeighborsIfMapped(const ChunkRefHandle &chunk);

	void loadChunkData(int cx, int cy, int cz, const RLEncoding &encoding);

	ChunkRefHandle copyChunkRefHandle(const ChunkRefHandle& handle);

	bool isBlockMapped(int x, int y, int z) const;
	bool isChunkMapped(int cx, int cy, int cz) const;
	bool isChunkNeighborhoodLoaded(int cx, int cy, int cz) const;
	bool isChunkNeighborhoodLoaded(const ChunkRefHandle &handle) const;

	Block getBlockIfMapped(int x, int y, int z) const;
	Block getBlockIfMapped(glm::i32vec3 vec) const;

	Block getBlock(int x, int y, int z);
	Block getBlock(glm::i32vec3 vec);
	void setBlock(int x, int y, int z, Block block);
	void setBlock(glm::i32vec3 vec, Block block);
	Block getBlock(float x, float y, float z);
	void setBlock(float x, float y, float z, Block block);

	void setLight(int x, int y, int z, Light light);
	ChunkQueueSet& getDirtyChunkQueue();

	BlockRayCast castRay(glm::vec3 start, glm::vec3 dir, float radius);

	int getChunkX(float x);
	int getChunkY(float y);
	int getChunkZ(float z);

private:
	bool isChunkMappedInternal(int cx, int cy, int cz);
	ChunkRefHandle getChunkIfMappedInternal(int cx, int cy, int cz);
	ChunkRefHandle getChunkIfNotMappedInternal(int cx, int cy, int cz);
	ChunkRefHandle getChunkInternal(int cx, int cy, int cz);

	void queueDirtyChunk(int cx, int cy, int cz);
	void queueDirtyChunk(ChunkRefHandle &&);
	
	ChunkPtr newChunk(int cx, int cy, int cz);
	bool isChunkLoaded(int cx, int cy, int cz) const;


	void loadChunks(int chunkX, int chunkY, int chunkZ, int loadDistance);
	void updateAllChunks(int playerCX, int playerCY, int playerCZ);
	void enqueueChunks();

	void chunkGeneratorThread();
	void chunkMeshingThread();

	float intbound(float s, float ds) const;
	constexpr int hashcode(int i, int j, int k) const;
	constexpr int expand(int x) const;

	int currentChunkX = 0;
	int currentChunkY = 0;
	int currentChunkZ = 0;
	
	typedef ChunkDestructor::RefCount RefCount;
	typedef std::pair<ChunkHandle, RefCount> ChunkRefCount;

	//todo see if i could just use shared_ptr
	std::unordered_map<int, ChunkRefCount> chunkSet;					//contains all chunks
	std::unordered_map<int, ChunkRefHandle> loadedChunkSet;				//subset of all chunks that are loaded
	std::vector<ChunkRefHandle> needsLoadingCache;						//subset of all chunks that could be loaded
	ChunkQueueSet dirtyChunks;						//subset of all chunks that should be meshed on main thread
	moodycamel::ConcurrentQueue<ChunkRefHandle> chunkGenerationQueue;
	
	Util::Recycler<Chunk> chunkRecycler;


	Util::Allocator::LinearAllocator chunkMesherAllocator;
	ChunkMesher *chunkMesherArray;
	ChunkMesher *mainChunkMesher;
	ChunkLightEngine *chunkLightEngine;

	std::atomic<bool> runThreads = true;
	std::thread generatorThread;
	std::thread mesherThread[CHUNK_THREAD_POOL_SIZE];

	mutable std::shared_mutex chunkSetMutex;		//gaurds chunkSet and mainMeshQueue
	mutable std::recursive_mutex meshQueueMutex;
	

};

}

