#include "Chunk.h"
#include <assert.h>

using namespace Voxel;

Chunk::Chunk(int x, int y, int z) : chunk_x(x), chunk_y(y), chunk_z(z){
	data = new Block[Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE];
}


Chunk::~Chunk() {
	delete data;
	vao.dispose();
}

void Chunk::generateTerrain() {
	for (int z = 0; z < Chunk::CHUNK_SIZE; z++) {
		for (int y = 0; y < Chunk::CHUNK_SIZE; y++) {
			for (int x = 0; x < Chunk::CHUNK_SIZE; x++) {
				if (y > 8)
					this->getBlock(x, y, z) = { BlockType::BLOCK_TYPE_DEFAULT, false };
				else
					this->getBlock(x, y, z) = {BlockType::BLOCK_TYPE_DIRT, true};
			}
		}
	}
}

void Chunk::generateMesh() {
	
}

inline Block& Chunk::getBlock(int x, int y, int z) {
	assert(x >= 0 && x < Chunk::CHUNK_SIZE);
	assert(y >= 0 && y < Chunk::CHUNK_SIZE);
	assert(z >= 0 && z < Chunk::CHUNK_SIZE);
	return data[x + Chunk::CHUNK_SIZE * (y + Chunk::CHUNK_SIZE * z)];
}