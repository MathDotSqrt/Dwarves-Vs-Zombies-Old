#include "Chunk.h"
#include <assert.h>
#include <stdlib.h>
#include "VBO.h"
#include "preamble.glsl"
#include "macrologger.h"
#include "Timer.h"
using namespace Voxel;

Chunk::Chunk(int x, int y, int z) : 
	chunk_x(x), 
	chunk_y(y), 
	chunk_z(z) {
	this->currentState = Chunk::EMPTY;

}

//Chunk::Chunk(int x, int y, int z, Block *data) : 
//	chunk_x(x), 
//	chunk_y(y), 
//	chunk_z(z), 
//	data(data),
//	vbo(GL_ARRAY_BUFFER), 
//	ebo(GL_ELEMENT_ARRAY_BUFFER)
//	{
//	this->isMeshValid = false;
//	
//	//glm::vec3::value_type
//}

Chunk::~Chunk() {
	//free(data);
	//this->vbo.dispose();
	//this->ebo.dispose();
	//LOG_VOXEL("Chunk {%d, %d, %d} deleted", this->chunk_x, this->chunk_y, this->chunk_z);
}

void Chunk::generateTerrain() {
	std::lock_guard<std::shared_mutex> writeLock(this->chunkMutex);
	for (int bz = 0; bz < CHUNK_WIDTH_Z; bz++) {
		for (int by = 0; by < CHUNK_WIDTH_Y; by++) {
			for (int bx = 0; bx < CHUNK_WIDTH_X; bx++) {

				int x = this->chunk_x * CHUNK_WIDTH_X + bx;
				int y = this->chunk_y * CHUNK_WIDTH_Y + by;
				int z = this->chunk_z * CHUNK_WIDTH_Z + bz;

				if (y < (getHashCode() % (CHUNK_WIDTH_Y * 2))) {
					this->setBlockInternal(bx, by, bz, (chunk_x + chunk_z) % 2 == 0 ? Block(BlockType::BLOCK_TYPE_STONE) : Block(BlockType::BLOCK_TYPE_DIRT));
				}

				/*if ((x * x) % (z * z + 1) > y * y) {
					if(y == 32)
						this->getBlockInternal(bx, by, bz) = { BlockType::BLOCK_TYPE_GRASS };
					else if((x * (z % (y * y + 1))) % 2 == 0)
						this->getBlockInternal(bx, by, bz) = { BlockType::BLOCK_TYPE_DIRT };
					else
						this->getBlockInternal(bx, by, bz) = { BlockType::BLOCK_TYPE_STONE };
					
				}
				else
					this->getBlockInternal(bx, by, bz) = {BlockType::BLOCK_TYPE_DEFAULT };*/
			}
		}
	}

	this->currentState = Chunk::LAZY_LOADED;
}

int Chunk::toIndex(int x, int y, int z) {
	return x + CHUNK_WIDTH_X * (y + CHUNK_WIDTH_Y * z);
}

void Chunk::assertBlockIndex(int x, int y, int z) {
	assert(x >= 0 && x < CHUNK_WIDTH_X);
	assert(y >= 0 && y < CHUNK_WIDTH_Y);
	assert(z >= 0 && z < CHUNK_WIDTH_Z);
}

Block Chunk::getBlock(int x, int y, int z) {
	if (this->currentState == Chunk::ChunkState::EMPTY) {
		return Block();
	}

	std::shared_lock<std::shared_mutex> lock(this->chunkMutex);
	this->assertBlockIndex(x, y, z);
	return data[this->toIndex(x, y, z)];
}

void Chunk::setBlock(int x, int y, int z, Block block) {
	if (this->currentState != Chunk::ChunkState::EMPTY) {
		return;
	}

	std::lock_guard<std::shared_mutex> lock(this->chunkMutex);

	this->assertBlockIndex(x, y, z);

	if (this->data[this->toIndex(x, y, z)] != block) {
		this->data[this->toIndex(x, y, z)] = block;

		if(this->currentState == Chunk::VALID)		//only dirty state if it was valid, not if empty or lazy
			this->currentState = Chunk::DIRTY_MESH;	//todo only dirty chunk if there is an adjecant block that is transparent
	}
}

int Chunk::expand(int x) {
	x &= 0x3FF;
	x = (x | (x << 16)) & 4278190335;
	x = (x | (x << 8)) & 251719695;
	x = (x | (x << 4)) & 3272356035;
	x = (x | (x << 2)) & 1227133513;
	return x;
}

int Chunk::getHashCode() {
	return expand(chunk_x) + (expand(chunk_y) << 1) + (expand(chunk_z) << 2);
}