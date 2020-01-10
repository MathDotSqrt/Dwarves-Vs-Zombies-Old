#pragma once
#include "Common.h"
#include "Block.h"
#include <vector>


namespace Voxel {
	typedef std::pair<uint16, Block> RLElement;
	typedef std::vector<RLElement> RLEncoding;

	
}