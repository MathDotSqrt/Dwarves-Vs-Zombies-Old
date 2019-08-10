#pragma once
#include <unordered_map>
#include "Chunk.h"

namespace Voxel{

class ChunkManager {
private:
	std::unordered_map<int, Chunk*> chunkSet;
	int expand(int x);

public:
	ChunkManager();
	~ChunkManager();

	int hashcode(int i, int j, int k);

	inline Chunk* getChunk(int cx, int cy, int cz);
	Chunk* generateChunk(int cx, int cy, int cz);
	inline Chunk* setChunk(int cx, int cy, int cz, Block *data);
	inline bool isChunkMapped(int cx, int cy, int cz);

	inline Block& getBlock(int x, int y, int z);
	inline void setBlock(int x, int y, int z, Block &block);
	inline bool isBlockMapped(int x, int y, int z);

};

}

