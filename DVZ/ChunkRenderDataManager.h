#pragma once
#include "Common.h"
#include "ChunkRenderData.h"
#include "glm.hpp"
#include "ChunkRefHandle.h"
#include "ConcurrentQueue.h"
#include "LinearAllocator.h"
#include "Recycler.h"
#include "ChunkManager.h"
#include <vector>
#include <thread>


namespace Voxel {

	struct RenderDataCopy {
		glm::vec3 pos;
		uint32 vaoID;
		uint32 vboID;
		size_t indexCount;
	};

	class ChunkRenderDataManager {
	private:
		typedef Util::Recycler<ChunkGeometry>::UniqueHandle ChunkGeometryHandle;
		typedef std::pair<ChunkNeighbors, ChunkGeometryHandle> ChunkNeighborGeometryPair;
		typedef std::pair<ChunkRefHandle, ChunkGeometryHandle> ChunkGeometryPair;


		const static int32 RENDER_RADIUS = 1;
		const static int32 RENDER_CHUNK_WIDTH = 2 * RENDER_RADIUS + 1;
		const static int32 GEOMETRY_ALLOC_SIZE = 100 * sizeof(ChunkGeometry);

		ChunkRenderData renderable[RENDER_CHUNK_WIDTH][RENDER_CHUNK_WIDTH];
		std::vector<RenderDataCopy> visible;
		std::vector<ChunkRefHandle> needsMeshCache;
		std::vector<uint32> queuedChunks;

		std::atomic<bool> isRunning = true;
		std::thread meshingThread;
		moodycamel::ConcurrentQueue<ChunkNeighborGeometryPair> chunkMeshingQueue;
		moodycamel::ConcurrentQueue<ChunkGeometryPair> chunkMeshedQueue;
		Util::Recycler<ChunkGeometry> geometryRecycler;
		Util::Allocator::LinearAllocator chunkMesherAllocator;
		ChunkMesher *mainMesher;
		ChunkMesher *mesher;

		int currentCX;
		int currentCY;
		int currentCZ;
	public:

		ChunkRenderDataManager(Util::Allocator::IAllocator &parent);
		~ChunkRenderDataManager();

		void update(glm::vec3 pos, glm::vec3 rot, ChunkManager &manager);
	
	private:
		void newChunk(int playerCX, int playerCY, int playerCZ, ChunkManager &manager);
		void updateDirtyChunks(ChunkManager &manager);
		void enqueueChunks(ChunkManager &manager);
		void dequeueChunks(ChunkManager &manager);
		void theadedMesher();

		ChunkRenderData& getRenderableChunk(int cx, int cz);
		ChunkRenderData& getRenderableChunk(const ChunkRefHandle &handle);
		bool isChunkRenderable(int cx, int cz);
		bool isChunkRenderable(int cx, int cz, const ChunkRenderData& data);
	
	};
}



