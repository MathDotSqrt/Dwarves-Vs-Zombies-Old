#pragma once
#include "Common.h"
#include "ChunkRenderData.h"
#include "glm.hpp"
#include "ChunkRefHandle.h"
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
	public:
		const static int32 RENDER_RADIUS = 25;

		ChunkRenderDataManager();
		~ChunkRenderDataManager();

		void update(glm::vec3 pos, glm::vec3 rot, ChunkManager &manager);

	private:
		ChunkRenderData renderable[2 * RENDER_RADIUS][2 * RENDER_RADIUS];
		std::vector<RenderDataCopy> visible;
		std::vector<ChunkRefHandle> needsMeshCache;
	};
}



