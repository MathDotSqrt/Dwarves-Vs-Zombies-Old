#pragma once
#include "Common.h"
#include "Block.h"
#include <vector>

namespace Voxel {
	class Chunk;
	
	typedef std::vector<std::pair<uint16, Block>> RLEncoding;
	const RLEncoding encode_chunk(const Chunk &chunk);
}