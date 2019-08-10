#include "ChunkManager.h"

using namespace Voxel;

ChunkManager::ChunkManager()
{
}


ChunkManager::~ChunkManager()
{
}

Chunk* ChunkManager::getChunk(int cx, int cy, int cz) {
	return this->chunkSet[this->hashcode(cx, cy, cz)];
}

Chunk* ChunkManager::generateChunk(int cx, int cy, int cz) {
	Chunk *chunk = nullptr;
	
	if (!isChunkMapped(cx, cy, cz)) {
		chunk = new Chunk(cx, cy, cz);
		chunkSet[hashcode(cx, cy, cz)] = chunk;
	}

	return chunk;
}

Chunk* ChunkManager::setChunk(int cx, int cy, int cz, Block *data) {
	Chunk *chunk;
	
	if (this->isChunkMapped(cx, cy, cz)) {
		chunk = this->getChunk(cx, cy, cz);
		chunk->setBlockData(data);
	}
	else {
		chunk = new Chunk(cx, cy, cz, data);
		this->chunkSet[this->hashcode(cx, cy, cz)] = chunk;
	}

	return chunk;
}

bool ChunkManager::isChunkMapped(int cx, int cy, int cz) {
	//check to see if chunkset actually inserts a nullptr when checking for an invalid chunk
	//Might cause lots of rehashing
	//return this->getChunk(cx, cy, cz) == nullptr;

	std::unordered_map<int, Chunk*>::const_iterator iter;
	iter = this->chunkSet.find(hashcode(cx, cy, cz));

	return iter != this->chunkSet.end();
}

Block& ChunkManager::getBlock(int x, int y, int z) {
	int cx = x >> Chunk::CHUNK_SHIFT_X;
	int cy = y >> Chunk::CHUNK_SHIFT_Y;
	int cz = z >> Chunk::CHUNK_SHIFT_Z;

	Chunk* chunk = this->getChunk(cx, cy, cz);

	assert(chunk);

	int bx = x & Chunk::CHUNK_BLOCK_POS_MASK_X;
	int by = y & Chunk::CHUNK_BLOCK_POS_MASK_Y;
	int bz = z & Chunk::CHUNK_BLOCK_POS_MASK_Z;
	return chunk->getBlock(bx, by, bz);
}

void ChunkManager::setBlock(int x, int y, int z, Block &block) {
	int cx = x >> Chunk::CHUNK_SHIFT_X;
	int cy = y >> Chunk::CHUNK_SHIFT_Y;
	int cz = z >> Chunk::CHUNK_SHIFT_Z;

	Chunk* chunk = this->getChunk(cx, cy, cz);

	assert(chunk);

	int bx = x & Chunk::CHUNK_BLOCK_POS_MASK_X;
	int by = y & Chunk::CHUNK_BLOCK_POS_MASK_Y;
	int bz = z & Chunk::CHUNK_BLOCK_POS_MASK_Z;

	chunk->setBlock(bx, by, bz, block);
}

bool ChunkManager::isBlockMapped(int x, int y, int z) {
	int cx = x >> Chunk::CHUNK_SHIFT_X;
	int cy = y >> Chunk::CHUNK_SHIFT_Y;
	int cz = z >> Chunk::CHUNK_SHIFT_Z;

	return this->isChunkMapped(cx, cy, cz);
}

int ChunkManager::expand(int x) {
	x &= 0x3FF;
	x = (x | (x << 16)) & 4278190335;
	x = (x | (x << 8))  & 251719695;
	x = (x | (x << 4))  & 3272356035;
	x = (x | (x << 2))  & 1227133513;
	return x;
}

int ChunkManager::hashcode(int i, int j, int k) {
	//z order curve
	return expand(i) + (expand(j) << 1) + (expand(k) << 2);
}