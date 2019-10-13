#include "ChunkManager.h"
#include <cmath>
#include <string>
#include <algorithm>
#include <random>
#include "Timer.h"



using namespace Voxel;

void chunk_gen_thread(ChunkHandle chunk) {
	if (chunk && chunk->isEmpty()) {
		chunk->generateTerrain();
	}
}


std::atomic<int> count = 0;
void chunk_mesh_thread(ChunkNeighbors neighbors, Chunk::BlockGeometry *geometry, ChunkMesher *mesher, moodycamel::ConcurrentQueue<std::pair<Chunk::BlockGeometry*, glm::ivec3>> *queue) {
	//static id for each thread
	const thread_local int workerID = count++;
	//std::this_thread::sleep_for(std::chrono::seconds(1));

	if (neighbors.middle->getChunkState() == Chunk::ChunkState::EMPTY)
		neighbors.middle->generateTerrain();
	mesher[workerID].loadChunkDataAsync(neighbors);
	mesher[workerID].createChunkMesh(*geometry);

	neighbors.middle->flagValid();

	int cx = neighbors.middle->getChunkX();
	int cy = neighbors.middle->getChunkY();
	int cz = neighbors.middle->getChunkZ();

	queue->enqueue(std::pair<Chunk::BlockGeometry*, glm::ivec3>(geometry, glm::vec3(cx, cy, cz)));
}

ChunkManager::ChunkManager(Util::Allocator::IAllocator &parent) :
	pool(CHUNK_THREAD_POOL_SIZE),
	chunkPoolAllocator(sizeof(Chunk), __alignof(Chunk), CHUNK_ALLOC_SIZE, parent),
	chunkMesherAllocator(CHUNK_MESHER_ALLOC_SIZE, parent),
	renderDataRecycler(CHUNK_RENDER_DATA_RECYCLE_SIZE, parent),
	meshRecycler(CHUNK_MESH_RECYCLE_SIZE, parent)
	{

	this->chunkMesherArray = Util::Allocator::allocateArray<ChunkMesher>(this->chunkMesherAllocator, CHUNK_THREAD_POOL_SIZE);

	for (int x = -RENDER_DISTANCE / 2 - 1; x < RENDER_DISTANCE / 2 + 1; x++) {
		for (int z = -RENDER_DISTANCE / 2 - 1; z < RENDER_DISTANCE / 2 + 1; z++) {
			int cx = x;
			int cy = 0;
			int cz = z;

			bool needsChunk = false;
			needsChunk = !this->isChunkMapped(cx, cy, cz);
			if (needsChunk) {
				ChunkHandle chunk = Util::Allocator::allocateHandle<Chunk>(this->chunkPoolAllocator, cx, cy, cz);
				this->pool.submit(chunk_gen_thread, chunk);
				this->chunkSet[this->hashcode(cx, 0, cz)] = chunk;
			}
		}
	}

}


ChunkManager::~ChunkManager() {
	//deleting all chunks
	this->pool.stop();
	this->chunkSet.clear();
	Util::Allocator::freeArray<ChunkMesher>(this->chunkMesherAllocator, this->chunkMesherArray);
	//ChunkManager::ChunkIterator iter = this->begin();
	/*while (iter != this->end()) {
		Util::Allocator::free(this->chunkPoolAllocator, iter->second);
		iter++;
	}*/
}

void ChunkManager::update(float x, float y, float z) {
	Util::Performance::Timer chunkTimer("Chunk Update");

	/*static auto meshingFunc = [this](ChunkNeighbors neighbors, Chunk::BlockGeometry* geometry) {
		this->chunkMeshingThread(neighbors, geometry);
	};*/
	
	int chunkX = this->getChunkX(x);
	int chunkY = this->getChunkY(y);
	int chunkZ = this->getChunkZ(z);

	if (chunkX != this->currentChunkX || chunkY != this->currentChunkY || chunkZ != this->currentChunkZ) {
		Util::Performance::Timer timer("Chunk gen submit");
		this->currentChunkX = chunkX;
		this->currentChunkY = chunkY;
		this->currentChunkZ = chunkZ;

		for (int x = -RENDER_DISTANCE/2 - 1; x < RENDER_DISTANCE/2 + 1; x++) {
			for (int z = -RENDER_DISTANCE/2 - 1; z < RENDER_DISTANCE/2 + 1; z++) {

				int cx = chunkX + x;
				int cy = chunkY;
				int cz = chunkZ + z;
				
				bool needsChunk = false;
				needsChunk = !this->isChunkMapped(cx, cy, cz);
				if (needsChunk) {
					ChunkHandle chunk = Util::Allocator::allocateHandle<Chunk>(this->chunkPoolAllocator, cx, cy, cz);
					this->pool.submit(chunk_gen_thread, chunk);
					this->chunkSet[this->hashcode(cx, chunkY, cz)] = chunk;
				}
			}
		}
	}

	{
	Util::Performance::Timer chunkIterTimer("Chunk Iter");
	ChunkIterator iter = this->begin();
	while (iter != this->end()) {
		ChunkHandle chunk = iter->second;
		int diffX = std::abs(chunk->getChunkX() - chunkX);
		int diffZ = std::abs(chunk->getChunkZ() - chunkZ);
		
		const int DELETE_RANGE = RENDER_DISTANCE / 2;

		if (diffX > (DELETE_RANGE +1)|| diffZ > (DELETE_RANGE+1)) {
			//removes chunk and returns an iterator pointing to the next chunk
			iter = this->removeChunk(iter);
		}
		else if (diffX < DELETE_RANGE && diffZ < DELETE_RANGE) {
			if (!chunk->isValid() && !this->chunkQueuedSet[iter->second->getHashCode()]) {
				this->pool.submit(chunk_mesh_thread, this->getChunkNeighbors(chunk), this->meshRecycler.getNew(), this->chunkMesherArray, &this->chunkMeshQueue);
				this->chunkQueuedSet[chunk->getHashCode()] = true;
			}
			iter++;
		}
		else {
			iter++;
		}
	}
	}
	Util::Performance::Timer chunkDequeueTimer("Chunk Dequeue");
	std::pair<Chunk::BlockGeometry*, glm::ivec3> element;
	for (int i = 0; i < 1 && this->chunkMeshQueue.try_dequeue(element); i++) {
		glm::ivec3 chunkCoord = element.second;
		ChunkRenderData *data = renderDataRecycler.getNew();
		data->cx = chunkCoord.x;
		data->cy = chunkCoord.y;
		data->cz = chunkCoord.z;
		data->bufferGeometry(element.first);

		int hashcode = this->hashcode(chunkCoord.x, chunkCoord.y, chunkCoord.z);
		this->renderDataSet[hashcode] = data;
		this->meshRecycler.recycle(element.first);

		this->chunkQueuedSet[hashcode] = false;
	}
}


//todo fix bug where all handles are in queue and the program wants to close causing a 
//assert in IAllocator for not freeing memory
void ChunkManager::chunkGeneratorThread(ChunkHandle chunk) {
	if (chunk && chunk->isEmpty()) {
		chunk->generateTerrain();
	}
}

//std::atomic<int> count = 0;
void ChunkManager::chunkMeshingThread(ChunkNeighbors neighbors, Chunk::BlockGeometry *geometry) {
	//static id for each thread
	//counter++
	thread_local int workerID = -1;
	//std::this_thread::sleep_for(std::chrono::seconds(1));
	
	if(neighbors.middle->getChunkState() == Chunk::ChunkState::EMPTY)
		neighbors.middle->generateTerrain();
	this->chunkMesherArray[workerID].loadChunkDataAsync(neighbors);
	this->chunkMesherArray[workerID].createChunkMesh(*geometry);
	
	neighbors.middle->flagValid();

	int cx = neighbors.middle->getChunkX();
	int cy = neighbors.middle->getChunkY();
	int cz = neighbors.middle->getChunkZ();

	this->chunkMeshQueue.enqueue(std::pair<Chunk::BlockGeometry*, glm::ivec3>(geometry, glm::vec3(cx, cy, cz)));
}



ChunkManager::ChunkIterator ChunkManager::removeChunk(const ChunkManager::ChunkIterator& iter) {
	//delete iter->second;
	//Util::Allocator::free<Chunk>(this->chunkPoolAllocator, iter->second);
	//iter->second = nullptr;
	
	int cx = iter->second->getChunkX();
	int cy = iter->second->getChunkY();
	int cz = iter->second->getChunkZ();
	auto r_iter = this->renderDataSet.find(this->hashcode(cx, cy, cz));
	if (r_iter != this->renderDataSet.end()) {
		this->renderDataRecycler.recycle(r_iter->second);
		this->renderDataSet.erase(r_iter);
	}

	return this->chunkSet.erase(iter);
}

void ChunkManager::removeChunk(int cx, int cy, int cz) {
	auto chunk = this->chunkSet.find(this->hashcode(cx, cy, cz));
	if (chunk != this->chunkSet.end()) {
		this->chunkSet.erase(chunk);
	}
}

ChunkHandle ChunkManager::getChunk(int cx, int cy, int cz) {
	return this->chunkSet[this->hashcode(cx, cy, cz)];
}

ChunkHandle ChunkManager::getChunkIfMapped(int cx, int cy, int cz) {
	std::unordered_map<int, ChunkHandle>::const_iterator iter;
	iter = this->chunkSet.find(hashcode(cx, cy, cz));
	return iter != this->chunkSet.end() ? iter->second : nullptr;
}

ChunkNeighbors ChunkManager::getChunkNeighbors(ChunkHandle chunk) {
	int cx = chunk->getChunkX(), cy = chunk->getChunkY(), cz = chunk->getChunkZ();
	
	return {
		chunk, 
		getChunkIfMapped(cx, cy, cz+1),
		getChunkIfMapped(cx, cy, cz-1),
		getChunkIfMapped(cx-1, cy, cz),
		getChunkIfMapped(cx+1, cy, cz),
		getChunkIfMapped(cx, cy+1, cz),
		getChunkIfMapped(cx, cy-1, cz)
	};
}


bool ChunkManager::isChunkMapped(int cx, int cy, int cz) {
	//check to see if chunkset actually inserts a nullptr when checking for an invalid chunk
	//Might cause lots of rehashing
	//return this->getChunk(cx, cy, cz) == nullptr;
	std::unordered_map<int, ChunkHandle>::const_iterator iter;
	iter = this->chunkSet.find(hashcode(cx, cy, cz));

	return iter != this->chunkSet.end();
}


Block ChunkManager::getBlock(int x, int y, int z) {
	int cx = x >> CHUNK_SHIFT_X;
	int cy = y >> CHUNK_SHIFT_Y;
	int cz = z >> CHUNK_SHIFT_Z;

	ChunkHandle chunk = this->getChunk(cx, cy, cz);

	assert(chunk);

	int bx = x & CHUNK_BLOCK_POS_MASK_X;
	int by = y & CHUNK_BLOCK_POS_MASK_Y;
	int bz = z & CHUNK_BLOCK_POS_MASK_Z;
	return chunk->getBlock(bx, by, bz);
}

void ChunkManager::setBlock(int x, int y, int z, Block block) {
	int cx = x >> CHUNK_SHIFT_X;
	int cy = y >> CHUNK_SHIFT_Y;
	int cz = z >> CHUNK_SHIFT_Z;

	ChunkHandle chunk = this->getChunk(cx, cy, cz);

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

Block ChunkManager::getBlock(float x, float y, float z) {
	x /= BLOCK_RENDER_SIZE;
	y /= BLOCK_RENDER_SIZE;
	z /= BLOCK_RENDER_SIZE;

	return this->getBlock((int)x, (int)y, (int)z);
}

void ChunkManager::setBlock(float x, float y, float z, Block block) {
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