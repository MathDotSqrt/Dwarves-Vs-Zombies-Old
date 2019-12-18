#pragma once

namespace Voxel { class ChunkManager; }

namespace Graphics {
	class ChunkRenderDataManager {
	public:
		ChunkRenderDataManager(int renderRadius);
		~ChunkRenderDataManager();

		void update(Voxel::ChunkManager &manager);

	private:
		int renderRadius;
	};
}



