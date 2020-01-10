#pragma once
#include "../DVZ_common/ChunkConstants.h"
#include "Block.h"

namespace Voxel {

	class VoxelContainer {
	private:
		Block blocks[CHUNK_VOLUME];

	public:
		VoxelContainer();
		~VoxelContainer();

		

		Block getBlock(int x, int y, int z) const;
		Block getBlock(int i) const;

		void setBlock(int x, int y, int z, Block block);
		void setBlock(int i, Block block);
	};
}



