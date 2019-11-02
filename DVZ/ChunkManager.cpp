#include "ChunkManager.h"
#include <cmath>
#include <string>
#include <algorithm>
#include <random>
#include "Timer.h"

using namespace Voxel;

ChunkManager::ChunkManager(Util::Allocator::IAllocator &parent) :
	chunkRecycler(CHUNK_ALLOC_SIZE, parent),
	renderDataRecycler(CHUNK_RENDER_DATA_RECYCLE_SIZE, parent),
	meshRecycler(CHUNK_MESH_RECYCLE_SIZE, parent),
	chunkMesherAllocator(CHUNK_MESHER_ALLOC_SIZE, parent)
	{

	generatorThread = std::thread(&ChunkManager::chunkGeneratorThread, this);
	for (int i = 0; i < CHUNK_THREAD_POOL_SIZE; i++) {
		mesherThread[i] = std::thread(&ChunkManager::chunkMeshingThread, this);
	}


	this->chunkMesherArray = Util::Allocator::allocateArray<ChunkMesher>(this->chunkMesherAllocator, CHUNK_THREAD_POOL_SIZE);

	this->chunkSet.max_load_factor(1.0f);
	this->loadChunks(0, 0, 0, LOAD_DISTANCE);
	this->meshChunks(0, 0, 0, RENDER_DISTANCE);
}


ChunkManager::~ChunkManager() {
	//deleting all chunks
	this->runThreads = false;
	this->chunkSet.clear();
	Util::Allocator::freeArray<ChunkMesher>(this->chunkMesherAllocator, this->chunkMesherArray);

	this->generatorThread.join();

	for (int i = 0; i < CHUNK_THREAD_POOL_SIZE; i++) {
		mesherThread[i].join();
	}
}

void ChunkManager::update(float x, float y, float z) {
	Util::Performance::Timer update("Chunk Update");
	int chunkX = this->getChunkX(x);
	int chunkY = this->getChunkY(y);
	int chunkZ = this->getChunkZ(z);
	if (chunkX != this->currentChunkX || chunkY != this->currentChunkY || chunkZ != this->currentChunkZ) {
		this->currentChunkX = chunkX;
		this->currentChunkY = chunkY;
		this->currentChunkZ = chunkZ;
		Util::Performance::Timer loadChunks("LoadChunks");

		this->loadChunks(chunkX, chunkY, chunkZ, LOAD_DISTANCE);
		this->meshChunks(chunkX, chunkY, chunkZ, RENDER_DISTANCE);
	}

	updateAllChunks(chunkX, chunkY, chunkZ);

	enqueueChunks();
	dequeueChunkRenderData();

	visibleChunkList.clear();
	for (auto iter = renderableChunkSet.begin(); iter != renderableChunkSet.end(); iter++) {
		ChunkRefHandle &chunk = iter->second.first;
		ChunkRenderDataHandle &handle = iter->second.second;
		if (chunk->getMeshState() != MeshState::NONE_MESH) {
			visibleChunkList.push_back(handle.get());
		}
	}
}

ChunkRefHandle ChunkManager::getChunk(int cx, int cy, int cz) {
	auto iter = this->chunkSet.find(hashcode(cx, cy, cz));

	if (iter != this->chunkSet.end()) {
		ChunkRefCount *chunkRefCountPair = &iter->second;
		Chunk* chunkPtr = chunkRefCountPair->first.get();
		RefCount *refCount = &chunkRefCountPair->second;
		return ChunkRefHandle(chunkPtr, ChunkDestructor(refCount));
	}

	ChunkHandle chunk = this->chunkRecycler.getUniqueNew(cx, cy, cz);
	chunk->reinitializeChunk(cx, cy, cz);
	int chunkHashCode = chunk->getHashCode();

	//pair.first = std::move(chunk);
	//this->chunkSet.emplace(chunkHashCode, std::move(pair));
	//this->chunkSet[chunkHashCode] = std::make_pair(chunk, std::move(a));

	this->chunkSet[chunkHashCode].first = std::move(chunk);
	this->chunkSet[chunkHashCode].second = 0;

	return getChunkIfMapped(cx, cy, cz);
}

ChunkRefHandle ChunkManager::getChunkIfMapped(int cx, int cy, int cz) {
	auto iter = this->chunkSet.find(hashcode(cx, cy, cz));

	if (iter == this->chunkSet.end()) {
		return ChunkRefHandle();
	}

	ChunkRefCount &chunkRefCountPair = iter->second;
	
	Chunk* chunkPtr = chunkRefCountPair.first.get();
	RefCount *refCountPtr = &chunkRefCountPair.second;
	return ChunkRefHandle(chunkPtr, ChunkDestructor(refCountPtr));
}

ChunkRefHandle ChunkManager::getNullChunk() {
	return ChunkRefHandle();
}

ChunkNeighbors ChunkManager::getChunkNeighbors(const ChunkRefHandle &chunk) {
	if (!chunk) {
		return ChunkNeighbors();
	}
	
	int cx = chunk->getChunkX(), cy = chunk->getChunkY(), cz = chunk->getChunkZ();
	return {
		getChunkIfMapped(cx, cy, cz),		//we make a new reference of a chunk within chunkset 
		getChunkIfMapped(cx, cy, cz+1),
		getChunkIfMapped(cx, cy, cz-1),
		getChunkIfMapped(cx-1, cy, cz),
		getChunkIfMapped(cx+1, cy, cz),
		getChunkIfMapped(cx, cy+1, cz),
		getChunkIfMapped(cx, cy-1, cz)
	};
}

ChunkRefHandle ChunkManager::copyChunkRefHandle(const ChunkRefHandle& handle) {
	return getChunk(handle->chunk_x, handle->chunk_y, handle->chunk_z);
}

bool ChunkManager::isChunkMapped(int cx, int cy, int cz) {
	//check to see if chunkset actually inserts a nullptr when checking for an invalid chunk
	//Might cause lots of rehashing
	//return this->getChunk(cx, cy, cz) == nullptr;
	auto iter = this->chunkSet.find(hashcode(cx, cy, cz));
	return iter != this->chunkSet.end();
}

bool ChunkManager::isBlockMapped(int x, int y, int z) {
	int cx = x >> CHUNK_SHIFT_X;
	int cy = y >> CHUNK_SHIFT_Y;
	int cz = z >> CHUNK_SHIFT_Z;

	return this->isChunkMapped(cx, cy, cz);
}

Block ChunkManager::getBlock(int x, int y, int z) {
	int cx = x >> CHUNK_SHIFT_X;
	int cy = y >> CHUNK_SHIFT_Y;
	int cz = z >> CHUNK_SHIFT_Z;

	ChunkRefHandle chunk = this->getChunk(cx, cy, cz);

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

	ChunkRefHandle chunk = this->getChunk(cx, cy, cz);

	assert(chunk);

	int bx = x & CHUNK_BLOCK_POS_MASK_X;
	int by = y & CHUNK_BLOCK_POS_MASK_Y;
	int bz = z & CHUNK_BLOCK_POS_MASK_Z;

	chunk->setBlock(bx, by, bz, block);
}

//todo potential bug if chunk has weird width
Block ChunkManager::getBlock(float x, float y, float z) {
	//x /= BLOCK_RENDER_SIZE;
	//y /= BLOCK_RENDER_SIZE;
	//z /= BLOCK_RENDER_SIZE;

	return this->getBlock((int)x, (int)y, (int)z);
}

void ChunkManager::setBlock(float x, float y, float z, Block block) {
	//x /= BLOCK_RENDER_SIZE;
	//y /= BLOCK_RENDER_SIZE;
	//z /= BLOCK_RENDER_SIZE;

	this->setBlock((int)x, (int)y, (int)z, block);
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

ChunkPtr ChunkManager::newChunk(int cx, int cy, int cz) {
	//test if there is a free chunk.
	ChunkPtr chunk_ptr = this->chunkRecycler.getNew(cx, cy, cz);
	chunk_ptr->reinitializeChunk(cx, cy, cz);		//todo think of a better way to reinit chunks

	return chunk_ptr;
}

bool ChunkManager::isChunkLoaded(int cx, int cy, int cz) {
	auto iter = this->loadedChunkSet.find(this->hashcode(cx, cy, cz));
	return iter != this->loadedChunkSet.end();
}

bool ChunkManager::isChunkRenderable(int cx, int cy, int cz) {
	auto iter = this->renderableChunkSet.find(this->hashcode(cx, cy, cz));
	return iter != this->renderableChunkSet.end();
}

void ChunkManager::loadChunks(int chunkX, int chunkY, int chunkZ, int distance) {
	
	//Unloading chunks that are out of load distance
	auto iter = loadedChunkSet.begin();
	while (iter != loadedChunkSet.end()) {
		ChunkRefHandle &chunk = iter->second;
		int diffX = std::abs(chunk->getChunkX() - chunkX);
		int diffZ = std::abs(chunk->getChunkZ() - chunkZ);

		const int DELETE_RANGE = distance / 2;

		if (diffX > (DELETE_RANGE) || diffZ > (DELETE_RANGE)) {
			iter = loadedChunkSet.erase(iter);
		}
		else {
			iter++;
		}
	}
	
	needsLoadingCache.clear();

	for (int x = -distance / 2; x < distance / 2; x++) {
		for (int z = -distance / 2; z < distance / 2; z++) {
			int cx = chunkX + x;
			int cy = chunkY;
			int cz = chunkZ + z;

			if (!isChunkLoaded(cx, cy, cz)) {
				Util::Performance::Timer chunkSubmit("Chunk Needs");
				needsLoadingCache.emplace_back(getChunk(cx, cy, cz));
			}
		}
	}


	auto compareLambda = [chunkX, chunkY, chunkZ](const ChunkRefHandle& lhs, const ChunkRefHandle& rhs) {
		int distL = std::abs(chunkX - lhs->getChunkX()) + std::abs(chunkY - lhs->getChunkY()) + std::abs(chunkZ - lhs->getChunkZ());
		int distR = std::abs(chunkX - rhs->getChunkX()) + std::abs(chunkY - rhs->getChunkY()) + std::abs(chunkZ - rhs->getChunkZ());

		return distL > distR;
	};
	std::sort(needsLoadingCache.begin(), needsLoadingCache.end(), compareLambda);
}

void ChunkManager::meshChunks(int chunkX, int chunkY, int chunkZ, int distance) {
	auto renderableIter = renderableChunkSet.begin();
	while (renderableIter != renderableChunkSet.end()) {
		ChunkRefHandle &chunk = renderableIter->second.first;
		int diffX = std::abs(chunk->getChunkX() - chunkX);
		int diffZ = std::abs(chunk->getChunkZ() - chunkZ);

		const int DELETE_RANGE = distance / 2;

		if (diffX > (DELETE_RANGE) || diffZ > (DELETE_RANGE)) {
			renderableIter = renderableChunkSet.erase(renderableIter);
		}
		else {
			renderableIter++;
		}
	}
	
	needsMeshCache.clear();

	for (int x = -distance / 2; x < distance / 2; x++) {
		for (int z = -distance / 2; z < distance / 2; z++) {
			int cx = chunkX + x;
			int cy = chunkY;
			int cz = chunkZ + z;

			if (!isChunkRenderable(cx, cy, cz)) {
				needsMeshCache.emplace_back(getChunk(cx, cy, cz));
			}
		}
	}

	auto compareLambda = [chunkX, chunkY, chunkZ](const ChunkRefHandle& lhs, const ChunkRefHandle& rhs) {
		int distL = std::abs(chunkX - lhs->getChunkX()) + std::abs(chunkY - lhs->getChunkY()) + std::abs(chunkZ - lhs->getChunkZ());
		int distR = std::abs(chunkX - rhs->getChunkX()) + std::abs(chunkY - rhs->getChunkY()) + std::abs(chunkZ - rhs->getChunkZ());

		return distL > distR;
	};

	std::sort(needsMeshCache.begin(), needsMeshCache.end(), compareLambda);
}

void ChunkManager::updateAllChunks(int playerCX, int playerCY, int playerCZ) {
	auto iter = this->chunkSet.begin();
	while (iter != this->chunkSet.end()) {
		ChunkRefCount &chunkRefCountPair = iter->second;
		ChunkHandle &chunk = chunkRefCountPair.first;
		RefCount &referenceCount = chunkRefCountPair.second;

		if (referenceCount == 0) {
			iter = this->chunkSet.erase(iter);
			continue;
		}
		
		iter++;
	}

	//auto renderIter = renderableChunkSet.begin();
	//while (renderIter != renderableChunkSet.end()) {
	//	ChunkRefHandle &chunk = renderIter->second.first;

	//	if (chunk->getMeshState() == Chunk::MeshState::DIRTY) {
	//		
	//	}

	//	renderIter++;
	//}
}

void ChunkManager::enqueueChunks() {
	for (int i = 0; i < 16 && needsLoadingCache.size() > 0; i++) {
		ChunkRefHandle chunk = std::move(needsLoadingCache.back());
		loadedChunkSet.emplace(chunk->getHashCode(), copyChunkRefHandle(chunk));

		needsLoadingCache.pop_back();
		this->chunkGenerationQueue.enqueue(std::move(chunk));
	}

	for (int i = 0; i < 16 && needsMeshCache.size() > 0; i++) {
		size_t chunkIndex = needsMeshCache.size() - 1;
		while (needsMeshCache[chunkIndex]->getBlockState() == BlockState::NONE) {
			if (chunkIndex == 0) {
				return;	//we are done
			}
			chunkIndex--;
		}

		ChunkRefHandle chunk = std::move(needsMeshCache[chunkIndex]);
		ChunkNeighbors chunkNeighbors = getChunkNeighbors(chunk);

		int hashCode = chunk->getHashCode();
		ChunkRenderDataPair pair = std::make_pair(std::move(chunk), renderDataRecycler.getUniqueNew());
		renderableChunkSet.emplace(hashCode, std::move(pair));
	
		ChunkNeighborGeometryPair p = std::make_pair(std::move(chunkNeighbors), meshRecycler.getUniqueNew());
		chunkMeshingQueue.enqueue(std::move(p));
		needsMeshCache.erase(needsMeshCache.begin() + chunkIndex);

	}
}

void ChunkManager::dequeueChunkRenderData() {
	ChunkGeometryPair element;
	for (int i = 0; this->chunkMeshedQueue.try_dequeue(element); i++) {
		ChunkRefHandle &chunk = element.first;
		if (isChunkRenderable(chunk->chunk_x, chunk->chunk_y, chunk->chunk_z)) {
			ChunkRenderDataHandle &data = renderableChunkSet[chunk->getHashCode()].second;

			data->cx = chunk->chunk_x;
			data->cy = chunk->chunk_y;
			data->cz = chunk->chunk_z;
			data->bufferGeometry(element.second.get());

			chunk->flagMeshValid();
		}
		
	}
}

//todo fix bug where all handles are in queue and the program wants to close causing a 
//assert in IAllocator for not freeing memory
void ChunkManager::chunkGeneratorThread() {
	while (this->runThreads) {
		ChunkRefHandle chunk;
		bool dequeued = this->chunkGenerationQueue.wait_dequeue_timed(chunk, std::chrono::milliseconds(500));
		if (dequeued) {
			if (chunk && chunk->getBlockState() == BlockState::NONE) {
				chunk->generateTerrain();
			}
		}
	}

}

std::atomic<int> threadCount = 0;
void ChunkManager::chunkMeshingThread() {
	//static id for each thread
	thread_local int workerID = threadCount++;

	ChunkNeighborGeometryPair pair;
	while (this->runThreads) {
		bool dequeued = this->chunkMeshingQueue.wait_dequeue_timed(pair, std::chrono::milliseconds(500));
		if (dequeued) {
			ChunkNeighbors &neighbors = pair.first;
			ChunkGeometryHandle &geometry = pair.second;
			//if (neighbors.middle->getChunkState() == Chunk::ChunkState::EMPTY)
			//	neighbors.middle->generateTerrain();
			this->chunkMesherArray[workerID].loadChunkDataAsync(neighbors);
			this->chunkMesherArray[workerID].createChunkMesh(geometry.get());

			int cx = neighbors.middle->getChunkX();
			int cy = neighbors.middle->getChunkY();
			int cz = neighbors.middle->getChunkZ();

			ChunkGeometryPair queuePair = std::make_pair(std::move(neighbors.middle), std::move(geometry));
			this->chunkMeshedQueue.enqueue(std::move(queuePair));
		}
	}
}

int ChunkManager::expand(int x) {
	x &= 0x3FF;
	x = (x | (x << 16)) & 4278190335;
	x = (x | (x << 8)) & 251719695;
	x = (x | (x << 4)) & 3272356035;
	x = (x | (x << 2)) & 1227133513;
	return x;
}

//todo figure out if this hashcode perserves locality with negative integers
int ChunkManager::hashcode(int i, int j, int k) {
	//z order curve
	return expand(i) + (expand(j) << 1) + (expand(k) << 2);
}