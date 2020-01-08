#pragma once
#include "MathUtil.h"
#include "../DVZ_common/ChunkConstants.h"
#include "VoxelContainer.h"


namespace Voxel {
	class Chunk {
	public:
		const int cx;
		const int cy;
		const int cz;

		Chunk(int cx, int cy, int cz, VoxelContainer *container);
		~Chunk();

		constexpr int32 getHashCode() {
			return Util::zorder_hash(cx, cy, cz);
		}
	};
}
	