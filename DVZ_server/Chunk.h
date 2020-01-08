#pragma once
#include "../DVZ_common/ChunkConstants.h"

namespace Voxel {
	class Chunk {
	public:
		const int cx;
		const int cy;
		const int cz;


		Chunk(int cx, int cy, int cz);
		~Chunk();
	};
}

