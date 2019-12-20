#pragma once
#include "Common.h"
#include "ChunkRenderData.h"
#include "glm.hpp"
#include "ChunkRefHandle.h"
#include "ConcurrentQueue.h"
#include "LinearAllocator.h"
#include "Recycler.h"
#include "ChunkManager.h"
#include "TEX.h"
#include <vector>
#include <thread>


namespace Voxel {

	struct RenderDataCopy {
		glm::vec3 pos;
		uint32 vaoID;
		float startTime;
		size_t indexCount;
	};

	class ChunkRenderDataManager {
	private:
		typedef Util::Recycler<ChunkGeometry>::UniqueHandle ChunkGeometryHandle;
		typedef std::pair<ChunkNeighbors, ChunkGeometryHandle> ChunkNeighborGeometryPair;
		typedef std::pair<ChunkRefHandle, ChunkGeometryHandle> ChunkGeometryPair;


		const static int32 RENDER_RADIUS = 25;
		const static int32 RENDER_CHUNK_WIDTH = 2 * RENDER_RADIUS + 1;
		const static int32 GEOMETRY_ALLOC_SIZE = 100 * sizeof(ChunkGeometry);

		//todo create a set and monotor state changes here instead of chunk manager cods
		ChunkRenderData renderable[RENDER_CHUNK_WIDTH][RENDER_CHUNK_WIDTH];
		std::vector<RenderDataCopy> visibleChunks;
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

		int currentCX = -1000;
		int currentCY = -1000;
		int currentCZ = -1000;
	public:

		Graphics::TEX debugTEX = Graphics::TEX::Builder("WorldTerrain2.png")
			.rgb()
			.linear()
			.mipmapLinear()
			.repeat()
			.buildTextureAtlasArray(2, 2);

		ChunkRenderDataManager(Util::Allocator::IAllocator &parent);
		~ChunkRenderDataManager();

		void update(glm::vec3 pos, glm::vec3 rot, ChunkManager &manager);
		std::vector<RenderDataCopy>::const_iterator begin();
		std::vector<RenderDataCopy>::const_iterator end();
	
	private:
		void createVisibleList(glm::vec3 rot);
		void newChunk(int playerCX, int playerCY, int playerCZ, ChunkManager &manager);
		void updateDirtyChunks(ChunkManager &manager);
		void enqueueChunks(ChunkManager &manager);
		void dequeueChunks(ChunkManager &manager);
		void theadedMesher();


		RenderDataCopy makeRenderDataCopy(const ChunkRenderData &data);
		ChunkRenderData& getRenderableChunk(int cx, int cz);
		ChunkRenderData& getRenderableChunk(const ChunkRefHandle &handle);
		bool isChunkRenderable(int cx, int cz);
		bool isChunkRenderable(int cx, int cz, const ChunkRenderData& data);
	
	};
}



