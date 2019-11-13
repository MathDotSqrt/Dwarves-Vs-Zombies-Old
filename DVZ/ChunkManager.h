#pragma once

#include "Block.h"
#include "Chunk.h"
#include "ChunkRefHandle.h"
#include "ChunkRenderData.h"
#include "ChunkMesher.h"

#include "LinearAllocator.h"
#include "PoolAllocator.h"
#include "AllocatorHandle.h"

#include "concurrentqueue.h"
#include "ThreadPool.h"
#include <thread>
#include <unordered_map>

namespace Voxel{



struct ChunkNeighbors {
	ChunkRefHandle middle;
	ChunkRefHandle front;
	ChunkRefHandle back;
	ChunkRefHandle left;
	ChunkRefHandle right;
	ChunkRefHandle up;
	ChunkRefHandle down;
};

class ChunkManager {
public:							//fix bug of things not destructing in order
	friend class ChunkMesher;

	static const int CHUNK_ALLOC_SIZE = 1000 * 1024 * 1024;
	static const int CHUNK_MESH_RECYCLE_SIZE = 4 * 1024 * 1024;
	static const int CHUNK_RENDER_DATA_RECYCLE_SIZE = 4 * 1024 * 1024;
	static const int CHUNK_MESHER_ALLOC_SIZE = 8 * 1024 * 1024;
	static const int CHUNK_THREAD_POOL_SIZE = 1;	//dont change this until i fix allocate array
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
	ChunkNeighbors getChunkNeighbors(int cx, int cy, int cz);
	ChunkNeighbors getChunkNeighbors(const ChunkRefHandle &chunk);

	ChunkRefHandle copyChunkRefHandle(const ChunkRefHandle& handle);

	ChunkRefHandle loadChunk(int cx, int cy, int cz);
	bool loadChunkAsync(int cx, int cy, int cz);
	bool loadChunkAsync(int cx, int cy, int cz, void *callback);
	
	bool isChunkMapped(int cx, int cy, int cz);
	bool isBlockMapped(int x, int y, int z);

	Block getBlock(int x, int y, int z);
	void setBlock(int x, int y, int z, Block block);
	Block getBlock(float x, float y, float z);
	void setBlock(float x, float y, float z, Block block);

	Block getBlockRay(glm::vec3 start, glm::vec3 end);
	void setBlockRay(glm::vec3 start, glm::vec3 end, Block block);

	int getChunkX(float x);
	int getChunkY(float y);
	int getChunkZ(float z);

	std::vector<ChunkRenderData*>& getVisibleChunks() {
		return this->visibleChunkList;
	}
private:
	ChunkPtr newChunk(int cx, int cy, int cz);
	//void removeChunk(int cx, int cy, int cz);	//fully removes chunk

	bool isChunkLoaded(int cx, int cy, int cz);
	bool isChunkRenderable(int cx, int cy, int cz);
	bool isChunkVisible(int cx, int cy, int cz);


	void loadChunks(int chunkX, int chunkY, int chunkZ, int loadDistance);
	void meshChunks(int chunkX, int chunkY, int chunkZ, int renderDistance);
	void updateAllChunks(int playerCX, int playerCY, int playerCZ);
	void enqueueChunks();
	void dequeueChunkRenderData();

	void chunkGeneratorThread();
	void chunkMeshingThread();

	constexpr int hashcode(int i, int j, int k) const;
	constexpr int expand(int x) const;

	int currentChunkX = 0;
	int currentChunkY = 0;
	int currentChunkZ = 0;
	
	typedef ChunkDestructor::RefCount RefCount;
	typedef std::pair<ChunkHandle, RefCount> ChunkRefCount;
	typedef Util::Recycler<ChunkRenderData>::UniqueHandle ChunkRenderDataHandle;
	typedef Util::Recycler<ChunkGeometry>::UniqueHandle ChunkGeometryHandle;
	typedef std::pair<ChunkRefHandle, ChunkRenderDataHandle>  ChunkRenderDataPair;
	typedef std::pair<ChunkNeighbors, ChunkGeometryHandle> ChunkNeighborGeometryPair;
	typedef std::pair<ChunkRefHandle, ChunkGeometryHandle> ChunkGeometryPair;

	Util::Recycler<Chunk> chunkRecycler;
	Util::Recycler<ChunkGeometry> meshRecycler;
	Util::Recycler<ChunkRenderData> renderDataRecycler;

	std::unordered_map<int, ChunkRefCount> chunkSet;
	std::unordered_map<int, ChunkRefHandle> loadedChunkSet;
	std::unordered_map<int, ChunkRenderDataPair> renderableChunkSet;
	std::vector<ChunkRefHandle> needsLoadingCache;
	std::vector<ChunkRefHandle> needsMeshCache;
	std::vector<ChunkRenderData*> visibleChunkList;
	moodycamel::ConcurrentQueue<ChunkRefHandle> chunkGenerationQueue;
	moodycamel::ConcurrentQueue<ChunkNeighborGeometryPair> chunkMeshingQueue;
	moodycamel::ConcurrentQueue<ChunkGeometryPair> chunkMeshedQueue;
	
	Util::Allocator::LinearAllocator chunkMesherAllocator;
	ChunkMesher *chunkMesherArray;
	ChunkMesher *mainChunkMesher;


	std::atomic<bool> runThreads = true;
	std::thread generatorThread;
	std::thread mesherThread[CHUNK_THREAD_POOL_SIZE];

	//moodycamel::BlockingConcurrentQueue<ChunkHandle> chunkGenQueue;
	//moodycamel::BlockingConcurrentQueue<std::pair<ChunkNeighbors, ChunkGeometry*>> chunkMeshingQueue;
	//moodycamel::ConcurrentQueue<std::pair<ChunkGeometry*, glm::ivec3>> chunkMeshQueue;

	//chunkSet
	//generateChunkList
	//meshChunkList
	//greedyMeshChunkList
	//renderableChunkSet
	//visibleChunkList
	


	/*std::unordered_map<int, ChunkHandle> chunkSet;
	std::unordered_map<int, ChunkRenderData*> renderDataSet;
	std::unordered_map<int, bool> chunkQueuedSet;*/

	

};

}

