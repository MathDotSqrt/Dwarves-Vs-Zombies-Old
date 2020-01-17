#pragma once
#include "Common.h"
#include "Block.h"
#include <vector>

namespace Voxel {
	class Chunk;
	
	typedef uint16 RLFreqType;
	typedef std::vector<std::pair<RLFreqType, Block>> RLEncoding;
	const RLEncoding encode_chunk(const Chunk &chunk);
}