#pragma once
#include "Block.h"

namespace Voxel{



class Chunk {

private:
	Block *data;

public:
	static const int CHUNK_SIZE = 16;

	Chunk();
	~Chunk();
};

}

