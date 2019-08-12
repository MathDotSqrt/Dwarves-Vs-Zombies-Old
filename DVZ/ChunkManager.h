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

	Chunk* getChunk(int cx, int cy, int cz);
	Chunk* generateChunk(int cx, int cy, int cz);
	Chunk* setChunk(int cx, int cy, int cz, Block *data);
	bool isChunkMapped(int cx, int cy, int cz);

	Block& getBlock(int x, int y, int z);
	void setBlock(int x, int y, int z, Block &block);
	bool isBlockMapped(int x, int y, int z);

	Block& getBlock(float x, float y, float z);
	void setBlock(float x, float y, float z, Block &block);

	int getBlockX(float x);
	int getBlockY(float y);
	int getBlockZ(float z);

	int getChunkX(float x);
	int getChunkY(float y);
	int getChunkZ(float z);
};

}

