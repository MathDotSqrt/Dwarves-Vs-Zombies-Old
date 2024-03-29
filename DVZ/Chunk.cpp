#include "Chunk.h"
#include "ChunkManager.h"
#include "macrologger.h"
#include "Timer.h"
#include "PerlinNoise.h"

#include <assert.h>
#include <stdlib.h>

using namespace Voxel;

Chunk::Chunk(int x, int y, int z, ChunkManager *manager) : 
	chunk_x(x), 
	chunk_y(y), 
	chunk_z(z),
	manager(manager){

	blockState = BlockState::NONE;
	meshState = MeshState::NONE_MESH;
}

Chunk::~Chunk() {}

void Chunk::generateTerrain() {
	static Util::PerlinNoise noise(3);
	std::lock_guard<std::shared_mutex> writeLock(this->chunkMutex);

	memset(&this->blockData, (uint8)BlockType::BLOCK_TYPE_DEFAULT, sizeof(this->blockData));
	memset(&this->lightData, (uint8)0, sizeof(this->lightData));

	for (int bz = 0; bz < CHUNK_WIDTH_Z; bz++) {
		for (int bx = 0; bx < CHUNK_WIDTH_X; bx++) {
			int x = this->chunk_x * CHUNK_WIDTH_X + bx;
			int z = this->chunk_z * CHUNK_WIDTH_Z + bz;

			//if ((chunk_x + chunk_z) % 2 == 0) {
			//	setBlockInternal(bx, 0, bz, Block(BlockType::BLOCK_TYPE_PURPLE));
			//}
			//else {
			//	setBlockInternal(bx, 0, bz, Block(BlockType::BLOCK_TYPE_DIRT));

			//}

			double height = pow(noise.octaveNoise0_1(x / 140.0, z / 140.0, 6), 4.5) * 155;

			for (int by = 0; by < CHUNK_WIDTH_Y; by++) {

				int y = this->chunk_y * CHUNK_WIDTH_Y + by;
				
				if (y == 0) {
					setBlockInternal(bx, by, bz, Block(BlockType::BLOCK_TYPE_STONE));
				}
				else if (y < height) {
					
					setBlockInternal(bx, by, bz, Block(BlockType::BLOCK_TYPE_DIRT));

				}
				else if (y < height + 1) {
					double sandValue = noise.octaveNoise0_1(x / 30.0, z / 30.0, 1);
					if ((y < 10) && sandValue > .6) {
						setBlockInternal(bx, by, bz, Block(BlockType::BLOCK_TYPE_SAND));

					}
					else {
						setBlockInternal(bx, by, bz, Block(BlockType::BLOCK_TYPE_GRASS));
						if (noise.noise0_1(x * .15, z * .15, 0) > 0.75) {
							setBlockInternal(bx, by+1, bz, Block(BlockType::BLOCK_TYPE_ROSE));

						}
					}
					
				}
				else {
					//setBlockInternal(bx, by, bz, Block(BlockType::BLOCK_TYPE_DEFAULT));
				}
			}
		}
	}

	blockState = BlockState::LOADED;
	
	if (meshState != MeshState::NONE_MESH) {
		meshState = MeshState::DIRTY;
	}
}

void Chunk::decodeRLEncoding(const RLEncoding &encoding) {
	std::lock_guard<std::shared_mutex> write_lock(chunkMutex);
	
	int index = 0;
	for (auto pair : encoding) {
		auto count = pair.first;
		Block block(pair.second);
		while (count) {
			assert(index < CHUNK_VOLUME);
			blockData[index] = block;
			index++;
			count--;
		}
	}

	blockState = BlockState::LOADED;
	meshState = MeshState::DIRTY;
}

void Chunk::flagMeshValid() {
	std::lock_guard<std::shared_mutex> lock(chunkMutex);
	meshState = MeshState::VALID;
}

void Chunk::flagMeshRemoved() {
	std::lock_guard<std::shared_mutex> lock(chunkMutex);
	meshState = MeshState::NONE_MESH;
}

bool Chunk::isEmpty() {
	std::shared_lock<std::shared_mutex> lock(chunkMutex);
	return blockState == BlockState::NONE || blockState == BlockState::LOADED_AND_EMPTY;
}

BlockState Chunk::getBlockState() {
	std::shared_lock<std::shared_mutex> lock(chunkMutex);
	return blockState;
}

MeshState Chunk::getMeshState() {
	std::shared_lock<std::shared_mutex> lock(chunkMutex);
	return meshState;
}

BlockState Chunk::tryGetBlockState() {
	if (chunkMutex.try_lock_shared()) {
		BlockState state = this->blockState;
		chunkMutex.unlock_shared();
		return state;
	}
	return BlockState::LOCKED;
}

MeshState Chunk::tryGetMeshState() {
	if (chunkMutex.try_lock_shared()) {
		MeshState state = this->meshState;
		chunkMutex.unlock_shared();
		return state;
	}
	return MeshState::LOCKED;
}

void Chunk::flagDirtyMesh() {
	std::lock_guard<std::shared_mutex> lock(chunkMutex);
	flagDirtyMeshInternal();
}

void Chunk::flagDirtyMeshInternal(){
	if (meshState != MeshState::NONE_MESH) {
		meshState = MeshState::DIRTY;
	}
}
//
//constexpr int Chunk::toIndex(int x, int y, int z) {
//	return x + CHUNK_WIDTH_X * (y + CHUNK_WIDTH_Y * z);
//}

void Chunk::assertBlockIndex(int x, int y, int z) const {
	assert(x >= 0 && x < CHUNK_WIDTH_X);
	assert(y >= 0 && y < CHUNK_WIDTH_Y);
	assert(z >= 0 && z < CHUNK_WIDTH_Z);
}

Block Chunk::getBlock(int x, int y, int z) {
	std::shared_lock<std::shared_mutex> lock(this->chunkMutex);
	if (this->blockState == BlockState::NONE) {
		return Block();
	}

	this->assertBlockIndex(x, y, z);
	return blockData[this->toIndex(x, y, z)];
}

void Chunk::setBlock(int x, int y, int z, Block block) {
	std::unique_lock<std::shared_mutex> lock(this->chunkMutex);
	if (this->blockState == BlockState::NONE) {
		return;
	}

	this->assertBlockIndex(x, y, z);

	if (blockData[this->toIndex(x, y, z)] == block)
		return;

	blockData[this->toIndex(x, y, z)] = block;

	if (block.getLightType() == LightType::LIGHT_TYPE_SOURCE) {
		queueLightInternal(x, y, z, Light(0, 0xf));
	}
	else if (block.getLightType() == LightType::LIGHT_TYPE_TRANSPARENT) {
		uint8 bLight = getBrightestNeighbor(x, y, z).getBlockLight();
		if(bLight) queueLightInternal(x, y, z, Light(0, bLight - 1));
	}

	if (meshState == MeshState::VALID) { //only dirty state if it was valid, not if empty or lazy
		meshState = MeshState::DIRTY;	 //todo only dirty chunk if there is an adjecant block that is transparent

		flagDirtyMeshInternal();
		lock.unlock();

		if (x == 0) {					//todo update adjacent meshes even if current mesh is dirty
			ChunkRefHandle left = manager->getChunkIfMapped(chunk_x - 1, chunk_y, chunk_z);
			left->flagDirtyMesh();
		}
		if (x == CHUNK_WIDTH_X - 1){
			ChunkRefHandle right = manager->getChunkIfMapped(chunk_x + 1, chunk_y, chunk_z);
			right->flagDirtyMesh();
		}

		if (z == 0) {
			ChunkRefHandle back = manager->getChunkIfMapped(chunk_x, chunk_y, chunk_z - 1);
			back->flagDirtyMesh();
		}
		if (z == CHUNK_WIDTH_Z - 1) {
			ChunkRefHandle front = manager->getChunkIfMapped(chunk_x, chunk_y, chunk_z + 1);
			front->flagDirtyMesh();
		}
	}
}

void Chunk::setBlock(int index, Block block) {
	//todo optimize out modulus
	//the order is how i order them in the array
	int x = index % CHUNK_WIDTH_X;			
	int z = (index / CHUNK_WIDTH_X) % CHUNK_WIDTH_Z;
	int y = index / (CHUNK_WIDTH_X * CHUNK_WIDTH_Z);

	setBlock(x, y, z, block);
}

Light Chunk::getLight(int x, int y, int z) {
	std::shared_lock<std::shared_mutex> lock(chunkMutex);
	
	if (blockState == BlockState::NONE)
		return Light(0, 0);

	assertBlockIndex(x, y, z);
	return lightData[toIndex(x, y, z)];
}

void Chunk::setLight(int x, int y, int z, Light light) {
	std::unique_lock<std::shared_mutex> lock(chunkMutex);
	//optimize when to add to chunk lighting update queue

	assertBlockIndex(x, y, z);
	int index = toIndex(x, y, z);
	//lightData[index] = light;

	queueLightInternal(x, y, z, light);

	flagDirtyMeshInternal();
	manager->queueDirtyChunk(chunk_x, chunk_y, chunk_z);
	//if (lightData[index] != light) {
	//	LightNode node = { light, index, chunk_x, chunk_y, chunk_z };
	//	manager->lightUpdates[getHashCode()].push(node);
	//}
}

void Chunk::setSunLight(int x, int y, int z, uint8 value) {
	
}

void Chunk::setBlockLight(int x, int y, int z, uint8 value) {

}

constexpr bool Chunk::isIndexInBounds(int x, int y, int z){
	return x >= 0 && x < CHUNK_WIDTH_X
		&& y >= 0 && y < CHUNK_WIDTH_Y
		&& z >= 0 && z < CHUNK_WIDTH_Z;
}

Light Chunk::getBrightestNeighbor(int x, int y, int z) const{
	uint8 maxBlockLight = 0;

	if (isIndexInBounds(x - 1, y, z)) {
		uint8 b = getLightInternal(x - 1, y, z).getBlockLight();
		maxBlockLight = b > maxBlockLight ? b : maxBlockLight;
	}
	if (isIndexInBounds(x + 1, y, z)) {
		uint8 b = getLightInternal(x + 1, y, z).getBlockLight();
		maxBlockLight = b > maxBlockLight ? b : maxBlockLight;
	}
	if (isIndexInBounds(x, y - 1, z)) {
		uint8 b = getLightInternal(x, y + 1, z).getBlockLight();
		maxBlockLight = b > maxBlockLight ? b : maxBlockLight;
	}
	if (isIndexInBounds(x, y + 1, z)) {
		uint8 b = getLightInternal(x, y + 1, z).getBlockLight();
		maxBlockLight = b > maxBlockLight ? b : maxBlockLight;
	}

	if (isIndexInBounds(x, y, z - 1)) {
		uint8 b = getLightInternal(x, y, z - 1).getBlockLight();
		maxBlockLight = b > maxBlockLight ? b : maxBlockLight;
	}
	if (isIndexInBounds(x, y, z + 1)) {
		uint8 b = getLightInternal(x, y, z + 1).getBlockLight();
		maxBlockLight = b > maxBlockLight ? b : maxBlockLight;
	}

	return Light(0, maxBlockLight);
}

void Chunk::reinitializeChunk(int cx, int cy, int cz) {
	//todo maybe put in lock gaurd here
	std::lock_guard<std::shared_mutex>  lock(chunkMutex);
	this->chunk_x = cx;
	this->chunk_y = cy;
	this->chunk_z = cz;

	blockState = BlockState::NONE;
	meshState = MeshState::NONE_MESH;
}

int Chunk::getHashCode() {
	return Chunk::calcHashCode(chunk_x, chunk_y, chunk_z);
}