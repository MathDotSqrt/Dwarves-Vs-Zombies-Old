#include "ChunkManager.h"
#include <cmath>
#include <string>
#include <algorithm>
#include <random>
#include "Timer.h"

#define CHUNK_ALLOC_SIZE 16 * 1024 * 1024
#define CHUNK_THREAD_POOL_SIZE 1

using namespace Voxel;

ChunkManager::ChunkManager(Util::Allocator::IAllocator &parent) 
	: chunkAllocator(sizeof(Chunk), __alignof(Chunk), CHUNK_ALLOC_SIZE, parent.allocate(CHUNK_ALLOC_SIZE)), 
	chunkReadyQueue(), 
	pool(CHUNK_THREAD_POOL_SIZE) {

}


ChunkManager::~ChunkManager() {
	//deleting all chunks
	this->pool.stop();
	ChunkManager::ChunkIterator iter = this->begin();
	while (iter != this->end()) {
		Util::Allocator::free(this->chunkAllocator, iter->second);
		iter++;
	}
}

void thread(Util::BlockingConcurrentQueue<Chunk*> *queue, Chunk *chunk) {
	chunk->generateTerrain();
	chunk->generateMesh();
	queue->push(chunk);
}

void ChunkManager::update(float x, float y, float z) {
	int chunkX = this->getChunkX(x);
	int chunkY = this->getChunkY(y);
	int chunkZ = this->getChunkZ(z);

	{
		Util::Performance::Timer chunkTimer("Chunk Updater");
		for (int x = -RENDER_DISTANCE/2; x < RENDER_DISTANCE/2; x++) {
			for (int z = -RENDER_DISTANCE/2; z < RENDER_DISTANCE/2; z++) {
				Util::Performance::Timer chunkTimer1("Chunk Test");

				int cx = chunkX + x;
				int cy = chunkY;
				int cz = chunkZ + z;
				
				bool needsChunk = false;
				needsChunk = !this->isChunkMapped(cx, cy, cz) && !this->isChunkQueued(cx, cy, cz);
				if (needsChunk) {
					Util::Performance::Timer chunkTimer1("Chunk Alloc");
					Chunk *chunk = nullptr;
					chunk = Util::Allocator::allocateNew<Chunk>(this->chunkAllocator, cx, cy, cz);
					this->pool.submit(thread, &this->chunkReadyQueue, chunk);
					this->chunkQueuedSet[this->hashcode(cx, chunkY, cz)] = chunk;
				}
			}
		}
	}

	Util::Performance::Timer timer("Chunk Dequeue");
	ChunkIterator iter = this->begin();
	while (iter != this->end()) {
		Chunk *chunk = iter->second;
		int diffX = std::abs(chunk->getChunkX() - chunkX);
		int diffZ = std::abs(chunk->getChunkZ() - chunkZ);
		
		const int DELETE_RANGE = RENDER_DISTANCE / 2 + 3;

		if (diffX > DELETE_RANGE || diffZ > DELETE_RANGE) {
			//removes chunk and returns an iterator pointing to the next chunk
			iter = this->removeChunk(iter);
		}
		else {
			if (!chunk->needsMeshUpdate()) {
				chunk->generateMesh();
				chunk->bufferDataToGPU();
			}
			iter++;
		}

	}

	for (int i = 0; i < 8 && !this->chunkReadyQueue.empty(); i++) {
		Chunk* chunk = this->chunkReadyQueue.pop();
		if (chunk != nullptr && !this->isChunkMapped(chunk->getChunkX(), chunk->getChunkY(), chunk->getChunkZ())) {
			chunk->bufferDataToGPU();
			this->chunkQueuedSet.erase(this->chunkQueuedSet.find(this->hashcode(chunk->getChunkX(), chunk->getChunkY(), chunk->getChunkZ())));

			this->chunkSet[this->hashcode(chunk->getChunkX(), chunk->getChunkY(), chunk->getChunkZ())] = chunk;
		}
		else {
			//chunk could not be added to chunk set, delete it
			Util::Allocator::free<Chunk>(this->chunkAllocator, chunk);
		}
	}
}

void ChunkManager::chunkLoader(Chunk *chunk) {
	chunk->generateTerrain();
	chunk->generateMesh();
	this->chunkReadyQueue.push(chunk);
}



ChunkManager::ChunkIterator ChunkManager::removeChunk(const ChunkManager::ChunkIterator& iter) {
	//delete iter->second;
	Util::Allocator::free<Chunk>(this->chunkAllocator, iter->second);
	iter->second = nullptr;

	return this->chunkSet.erase(iter);
}

void ChunkManager::removeChunk(int cx, int cy, int cz) {
	auto chunk = this->chunkSet.find(this->hashcode(cx, cy, cz));
	if (chunk != this->chunkSet.end()) {
		delete chunk->second;
		chunk->second = nullptr;

		this->chunkSet.erase(chunk);

	}
}

Chunk* ChunkManager::getChunk(int cx, int cy, int cz) {
	return this->chunkSet[this->hashcode(cx, cy, cz)];
}

Chunk* ChunkManager::generateChunk(int cx, int cy, int cz) {
	Chunk *chunk = nullptr;
	
	if (isChunkMapped(cx, cy, cz)) {
		chunk = chunkSet[hashcode(cx, cy, cz)];
	}
	else {
		chunk = new Chunk(cx, cy, cz);
		chunkSet[hashcode(cx, cy, cz)] = chunk;
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

bool ChunkManager::isChunkQueued(int cx, int cy, int cz) {
	//check to see if chunkset actually inserts a nullptr when checking for an invalid chunk
	//Might cause lots of rehashing
	//return this->getChunk(cx, cy, cz) == nullptr;

	std::unordered_map<int, Chunk*>::const_iterator iter;
	iter = this->chunkQueuedSet.find(hashcode(cx, cy, cz));

	return iter != this->chunkQueuedSet.end();
}

Block& ChunkManager::getBlock(int x, int y, int z) {
	int cx = x >> CHUNK_SHIFT_X;
	int cy = y >> CHUNK_SHIFT_Y;
	int cz = z >> CHUNK_SHIFT_Z;

	Chunk* chunk = this->getChunk(cx, cy, cz);

	assert(chunk);

	int bx = x & CHUNK_BLOCK_POS_MASK_X;
	int by = y & CHUNK_BLOCK_POS_MASK_Y;
	int bz = z & CHUNK_BLOCK_POS_MASK_Z;
	return chunk->getBlock(bx, by, bz);
}

void ChunkManager::setBlock(int x, int y, int z, Block &block) {
	int cx = x >> CHUNK_SHIFT_X;
	int cy = y >> CHUNK_SHIFT_Y;
	int cz = z >> CHUNK_SHIFT_Z;

	Chunk* chunk = this->getChunk(cx, cy, cz);

	assert(chunk);

	int bx = x & CHUNK_BLOCK_POS_MASK_X;
	int by = y & CHUNK_BLOCK_POS_MASK_Y;
	int bz = z & CHUNK_BLOCK_POS_MASK_Z;

	chunk->setBlock(bx, by, bz, block);
}

bool ChunkManager::isBlockMapped(int x, int y, int z) {
	int cx = x >> CHUNK_SHIFT_X;
	int cy = y >> CHUNK_SHIFT_Y;
	int cz = z >> CHUNK_SHIFT_Z;

	return this->isChunkMapped(cx, cy, cz);
}

Block& ChunkManager::getBlock(float x, float y, float z) {
	x /= BLOCK_RENDER_SIZE;
	y /= BLOCK_RENDER_SIZE;
	z /= BLOCK_RENDER_SIZE;

	return this->getBlock((int)x, (int)y, (int)z);
}

void ChunkManager::setBlock(float x, float y, float z, Block &block) {
	x /= BLOCK_RENDER_SIZE;
	y /= BLOCK_RENDER_SIZE;
	z /= BLOCK_RENDER_SIZE;

	this->setBlock((int)x, (int)y, (int)z, block);
}

int ChunkManager::getBlockX(float x) {
	return (int)(x / BLOCK_RENDER_SIZE);
}

int ChunkManager::getBlockY(float y) {
	return (int)(y / BLOCK_RENDER_SIZE);
}

int ChunkManager::getBlockZ(float z) {
	return (int)(z / BLOCK_RENDER_SIZE);
}

int ChunkManager::getChunkX(float x) {
	return (int)(x / CHUNK_RENDER_WIDTH_X);
}

int ChunkManager::getChunkY(float y) {
	return (int)(y / CHUNK_RENDER_WIDTH_Y);
}

int ChunkManager::getChunkZ(float z) {
	return (int)(z / CHUNK_RENDER_WIDTH_Z);
}

int ChunkManager::expand(int x) {
	x &= 0x3FF;
	x = (x | (x << 16)) & 4278190335;
	x = (x | (x << 8))  & 251719695;
	x = (x | (x << 4))  & 3272356035;
	x = (x | (x << 2))  & 1227133513;
	return x;
}

//todo figure out if this hashcode perserves locality with negative integers
int ChunkManager::hashcode(int i, int j, int k) {
	//z order curve
	return expand(i) + (expand(j) << 1) + (expand(k) << 2);
}