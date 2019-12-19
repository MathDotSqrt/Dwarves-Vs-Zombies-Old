#pragma once
#include "Common.h"
#include "ChunkRenderData.h"
#include "glm.hpp"
#include "ChunkRefHandle.h"
#include "ConcurrentQueue.h"
#include <vector>


namespace Voxel {
	class ChunkManager;

	struct RenderDataCopy {
		glm::vec3 pos;
		uint32 vaoID;
		uint32 vboID;
		size_t indexCount;
	};

	class ChunkRenderDataManager {
	private:
		const static int32 RENDER_RADIUS = 1;
		const static int32 RENDER_CHUNK_WIDTH = 2 * RENDER_RADIUS + 1;

		int currentCX;
		int currentCY;
		int currentCZ;

		ChunkRenderData renderable[RENDER_CHUNK_WIDTH][RENDER_CHUNK_WIDTH];
		std::vector<RenderDataCopy> visible;
		std::vector<ChunkRefHandle> needsMeshCache;

		moodycamel::ConcurrentQueue<std::pair<ChunkNeighbors, ChunkGeometry>> chunkMeshingQueue;
	public:

		ChunkRenderDataManager();
		~ChunkRenderDataManager();

		void update(glm::vec3 pos, glm::vec3 rot, ChunkManager &manager);
	
	private:
		void newChunk(int playerCX, int playerCY, int playerCZ, ChunkManager &manager);
		void enqueueChunks();

		ChunkRenderData& getRenderableChunk(int cx, int cz);
		bool isChunkRenderable(int cx, int cz);
		bool isChunkRenderable(int cx, int cz, const ChunkRenderData& data);
	
	};
}



