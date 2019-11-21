#include "ChunkManager.h"
#include "Timer.h"
#include "Window.h"
#include <cmath>
#include <string>
#include <algorithm>
#include <random>

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
	this->mainChunkMesher = Util::Allocator::allocateNew<ChunkMesher>(this->chunkMesherAllocator);
	this->chunkLightEngine = Util::Allocator::allocateNew<ChunkLightEngine>(chunkMesherAllocator);

	this->chunkSet.max_load_factor(1.0f);
	this->loadChunks(0, 0, 0, LOAD_DISTANCE);
	this->meshChunks(0, 0, 0, RENDER_DISTANCE);
}


ChunkManager::~ChunkManager() {
	//deleting all chunks
	this->runThreads = false;

	this->generatorThread.join();

	for (int i = 0; i < CHUNK_THREAD_POOL_SIZE; i++) {
		mesherThread[i].join();
	}

	//this->chunkSet.clear();
	Util::Allocator::freeArray<ChunkMesher>(this->chunkMesherAllocator, this->chunkMesherArray);
	Util::Allocator::free<ChunkMesher>(this->chunkMesherAllocator, this->mainChunkMesher);
	Util::Allocator::free<ChunkLightEngine>(chunkMesherAllocator, chunkLightEngine);

	
}

void ChunkManager::update(float x, float y, float z) {
	Util::Performance::Timer updateTimer("Chunk Update");
	
	int chunkX = this->getChunkX(x);
	int chunkY = this->getChunkY(y);
	int chunkZ = this->getChunkZ(z);
	if (chunkX != this->currentChunkX || chunkY != this->currentChunkY || chunkZ != this->currentChunkZ) {
		this->currentChunkX = chunkX;
		this->currentChunkY = chunkY;
		this->currentChunkZ = chunkZ;

		Util::Performance::Timer updateTimer("Queue Load/Mesh");
		this->loadChunks(chunkX, chunkY, chunkZ, LOAD_DISTANCE);
		this->meshChunks(chunkX, chunkY, chunkZ, RENDER_DISTANCE);
	}

	{
		Util::Performance::Timer updateAllChunksTimer("Updating all chunks");
		updateAllChunks(chunkX, chunkY, chunkZ);
	}

	{
		Util::Performance::Timer updateDirtyChunk("Updating dirty chunks");
		updateDirtyChunks();
	}

	{
		Util::Performance::Timer enqueueChunksTimer("Enqueue Chunks");
		enqueueChunks();
	}
	{
		Util::Performance::Timer dqChunksTimer("DQ Chunks");
		dequeueChunkRenderData();
	}

	{
	Util::Performance::Timer visibleChunkTimer("Build Visible List");
	visibleChunkList.clear();
	auto b = renderableChunkSet.begin();
	auto e = renderableChunkSet.end();
	for (auto iter = b; iter != e; iter++) {
		ChunkRefHandle &chunk = iter->second.first;
		MeshState state = chunk->meshState;	//todo this might give undefined behavior
		if (state != MeshState::NONE_MESH && state != MeshState::LOCKED) {
			ChunkRenderDataHandle &handle = iter->second.second;
			ChunkRenderData *ptr = handle.get();
			visibleChunkList.push_back(ptr);
		}
	}
	}
}

ChunkRefHandle ChunkManager::getChunk(int cx, int cy, int cz) {
	{
		std::shared_lock readLock(chunkSetMutex);
		auto iter = this->chunkSet.find(hashcode(cx, cy, cz));

		if (iter != this->chunkSet.end()) {
			ChunkRefCount *chunkRefCountPair = &iter->second;
			Chunk* chunkPtr = chunkRefCountPair->first.get();
			RefCount *refCount = &chunkRefCountPair->second;
			return ChunkRefHandle(chunkPtr, ChunkDestructor(refCount));
		}
	}

	std::lock_guard writeLock(chunkSetMutex);

	ChunkHandle chunk = this->chunkRecycler.getUniqueNew(cx, cy, cz, this);
	chunk->reinitializeChunk(cx, cy, cz);
	int chunkHashCode = chunk->getHashCode();

	//pair.first = std::move(chunk);
	//this->chunkSet.emplace(chunkHashCode, std::move(pair));
	//this->chunkSet[chunkHashCode] = std::make_pair(chunk, std::move(a));

	ChunkRefCount &pair = chunkSet[chunkHashCode];

	pair.first = std::move(chunk);
	pair.second = 0;

	Chunk* chunkPtr = pair.first.get();
	RefCount *refCountPtr = &pair.second;
	return ChunkRefHandle(chunkPtr, ChunkDestructor(refCountPtr));;
}

ChunkRefHandle ChunkManager::getChunkIfMapped(int cx, int cy, int cz) {
	std::shared_lock readLock(chunkSetMutex);
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

ChunkNeighbors ChunkManager::getChunkNeighbors(int cx, int cy, int cz) {

	return {
		getChunkIfMapped(cx - 1, cy, cz - 1),
		getChunkIfMapped(cx    , cy, cz - 1),
		getChunkIfMapped(cx + 1, cy, cz - 1),

		getChunkIfMapped(cx - 1, cy, cz),
		getChunkIfMapped(cx    , cy, cz),
		getChunkIfMapped(cx + 1, cy, cz),

		getChunkIfMapped(cx - 1, cy, cz + 1),
		getChunkIfMapped(cx    , cy, cz + 1),
		getChunkIfMapped(cx + 1, cy, cz + 1)
	};
}

ChunkNeighbors ChunkManager::getChunkNeighbors(const ChunkRefHandle &chunk) {
	if (!chunk) {
		return ChunkNeighbors();
	}
	
	int cx = chunk->getChunkX(), cy = chunk->getChunkY(), cz = chunk->getChunkZ();
	return getChunkNeighbors(cx, cy, cz);
}

ChunkRefHandle ChunkManager::copyChunkRefHandle(const ChunkRefHandle& handle) {
	return getChunk(handle->chunk_x, handle->chunk_y, handle->chunk_z);
}

bool ChunkManager::isChunkMapped(int cx, int cy, int cz) const{
	//check to see if chunkset actually inserts a nullptr when checking for an invalid chunk
	//Might cause lots of rehashing
	//return this->getChunk(cx, cy, cz) == nullptr;
	std::shared_lock<std::shared_mutex> readLock(chunkSetMutex);

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
	return this->getBlock((int)x, (int)y, (int)z);
}

void ChunkManager::setBlock(float x, float y, float z, Block block) {
	this->setBlock((int)x, (int)y, (int)z, block);
}

void ChunkManager::setLight(int x, int y, int z, Light light) {
	int cx = x >> CHUNK_SHIFT_X;
	int cy = y >> CHUNK_SHIFT_Y;
	int cz = z >> CHUNK_SHIFT_Z;

	ChunkRefHandle chunk = this->getChunk(cx, cy, cz);

	assert(chunk);

	int bx = x & CHUNK_BLOCK_POS_MASK_X;
	int by = y & CHUNK_BLOCK_POS_MASK_Y;
	int bz = z & CHUNK_BLOCK_POS_MASK_Z;
	chunk->setLight(bx, by, bz, light);

}

BlockRayCast ChunkManager::castRay(glm::vec3 start, glm::vec3 dir, float radius) {
	const Block AIR;
	dir = glm::normalize(dir);

	int X = (int)floor(start.x);
	int Y = (int)floor(start.y);
	int Z = (int)floor(start.z);
	int stepX = dir.x >= 0 ? 1 : -1;
	int stepY = dir.y >= 0 ? 1 : -1;
	int stepZ = dir.z >= 0 ? 1 : -1;

	float tMaxX = intbound(start.x, dir.x);
	float tMaxY = intbound(start.y, dir.y);
	float tMaxZ = intbound(start.z, dir.z);

	float tDeltaX = stepX / dir.x;
	float tDeltaY = stepY / dir.y;
	float tDeltaZ = stepZ / dir.z;

	glm::ivec3 norm;

	Block block;

	do {
		// tMaxX stores the t-value at which we cross a cube boundary along the
		// X axis, and similarly for Y and Z. Therefore, choosing the least tMax
		// chooses the closest cube boundary. Only the first case of the four
		// has been commented in detail.
		if (tMaxX < tMaxY) {
			if (tMaxX < tMaxZ) {
				if (tMaxX > radius) break;
				// Update which cube we are now in.
				X += stepX;
				// Adjust tMaxX to the next X-oriented boundary crossing.
				tMaxX += tDeltaX;

				// Record the normal vector of the cube face we entered.
				norm = glm::ivec3(-stepX, 0, 0);
			}
			else {
				if (tMaxZ > radius) break;
				Z += stepZ;
				tMaxZ += tDeltaZ;
				norm = glm::ivec3(0, 0, -stepZ);

			}
		}
		else {
			if (tMaxY < tMaxZ) {
				if (tMaxY > radius) break;
				Y += stepY;
				tMaxY += tDeltaY;
				norm = glm::ivec3(0, -stepY, 0);

			}
			else {
				// Identical to the second case, repeated for simplicity in
				// the conditionals.
				if (tMaxZ > radius) break;
				Z += stepZ;
				tMaxZ += tDeltaZ;
				norm = glm::ivec3(0, 0, -stepZ);

			}
		}

		block = getBlock(X, Y, Z);
	} while (block == AIR);

	BlockRayCast cast = {
		block, 
		X, Y, Z, 
		X + norm.x, Y + norm.y, Z + norm.z
	};

	return cast;
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

void ChunkManager::queueDirtyChunk(int cx, int cy, int cz) {
	ChunkRefHandle chunk = getChunkIfMapped(cx, cy, cz);
	if (chunk) {
		std::lock_guard writeLock(meshQueueMutex);
		mainMeshQueue.emplace(std::move(chunk));
	}
}

void ChunkManager::queueDirtyChunk(ChunkRefHandle &&chunk) {
	//ChunkRefHandle chunk = getChunkIfMapped(cx, cy, cz);
	if (chunk) {
		std::lock_guard writeLock(meshQueueMutex);
		mainMeshQueue.emplace(std::move(chunk));
	}
}

ChunkPtr ChunkManager::newChunk(int cx, int cy, int cz) {
	//test if there is a free chunk.
	ChunkPtr chunk_ptr = this->chunkRecycler.getNew(cx, cy, cz, this);
	chunk_ptr->reinitializeChunk(cx, cy, cz);		//todo think of a better way to reinit chunks

	return chunk_ptr;
}

bool ChunkManager::isChunkLoaded(int cx, int cy, int cz) const {
	auto iter = this->loadedChunkSet.find(this->hashcode(cx, cy, cz));
	return iter != this->loadedChunkSet.end();
}

bool ChunkManager::isChunkRenderable(int cx, int cy, int cz) const {
	auto iter = this->renderableChunkSet.find(this->hashcode(cx, cy, cz));
	return iter != this->renderableChunkSet.end();
}

void ChunkManager::sortChunks(int chunkX, int chunkY, int chunkZ, std::vector<ChunkRefHandle> &vector) {
	auto lambda = [chunkX, chunkY, chunkZ](const ChunkRefHandle &rhs, const ChunkRefHandle &lhs) {
		int distL = std::abs(chunkX - lhs->getChunkX()) + std::abs(chunkY - lhs->getChunkY()) + std::abs(chunkZ - lhs->getChunkZ());
		int distR = std::abs(chunkX - rhs->getChunkX()) + std::abs(chunkY - rhs->getChunkY()) + std::abs(chunkZ - rhs->getChunkZ());

		return distL < distR;
	};

	std::sort(vector.begin(), vector.end(), lambda);
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
				needsLoadingCache.emplace_back(getChunk(cx, cy, cz));
			}
		}
	}


	auto compareLambda = [chunkX, chunkY, chunkZ](const ChunkRefHandle& lhs, const ChunkRefHandle& rhs) {
		int distL = std::abs(chunkX - lhs->getChunkX()) + std::abs(chunkY - lhs->getChunkY()) + std::abs(chunkZ - lhs->getChunkZ());
		int distR = std::abs(chunkX - rhs->getChunkX()) + std::abs(chunkY - rhs->getChunkY()) + std::abs(chunkZ - rhs->getChunkZ());

		return distL > distR;
	};
	//std::sort(needsLoadingCache.begin(), needsLoadingCache.end(), compareLambda);
	sortChunks(chunkX, chunkY, chunkZ, needsLoadingCache);
}

void ChunkManager::meshChunks(int chunkX, int chunkY, int chunkZ, int distance) {
	auto renderableIter = renderableChunkSet.begin();
	while (renderableIter != renderableChunkSet.end()) {
		ChunkRefHandle &chunk = renderableIter->second.first;
		int diffX = std::abs(chunk->getChunkX() - chunkX);
		int diffZ = std::abs(chunk->getChunkZ() - chunkZ);

		const int DELETE_RANGE = distance / 2;

		if (diffX > (DELETE_RANGE) || diffZ > (DELETE_RANGE)) {
			//todo replace flag
			renderableIter->second.first->flagMeshRemoved();
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

	//std::sort(needsMeshCache.begin(), needsMeshCache.end(), compareLambda);
	sortChunks(chunkX, chunkY, chunkZ, needsMeshCache);
}

void ChunkManager::updateAllChunks(int playerCX, int playerCY, int playerCZ) {
	
//	std::vector<ChunkNeighbors> neighbors;
	
	std::lock_guard writeLock(chunkSetMutex);

	int num_erased = 0;
	const int MAX_ERASE = 100;

	auto iter = this->chunkSet.begin();
	while (iter != this->chunkSet.end()) {
		ChunkRefCount &chunkRefCountPair = iter->second;
		ChunkHandle &chunk = chunkRefCountPair.first;
		RefCount &referenceCount = chunkRefCountPair.second;

		if (num_erased <= MAX_ERASE && referenceCount == 0) {
			iter = this->chunkSet.erase(iter);
			num_erased++;
			continue;
		}
		
		//if (iter->second.first->meshState == MeshState::DIRTY) {
		//	ChunkHandle &handle = iter->second.first;
		//	neighbors.emplace_back(getChunkNeighbors(handle->chunk_x, handle->chunk_y, handle->chunk_z));
		//}


		iter++;
	}

	//Util::Performance::Timer timer("Chunk Main ReMeshing");
	//while (neighbors.size() > 0) {
	//	ChunkNeighbors n = std::move(neighbors.back());
	//	mainChunkMesher->loadChunkData(n);
	//	ChunkGeometryHandle geometry = meshRecycler.getUniqueNew();
	//	mainChunkMesher->createChunkMesh(geometry.get());

	//	ChunkRefHandle &chunk = n.middle;
	//	ChunkRenderDataHandle &renderData = renderableChunkSet[chunk->getHashCode()].second;
	//	renderData->bufferGeometry(geometry.get());
	//	chunk->flagMeshValid();

	//	neighbors.pop_back();
	//	break;
	//}
	
}

void ChunkManager::updateDirtyChunks() {
	constexpr int MAX_DQ = 4;

	//if (mainMeshQueue.size() > 0) {
	//	ChunkRefHandle &handle = mainMeshQueue.front();
	//	ChunkNeighborGeometryPair p = std::make_pair(std::move(getChunkNeighbors(handle)), meshRecycler.getUniqueNew());
	//	chunkMeshingQueue.enqueue(std::move(p));
	//	mainMeshQueue.pop();
	//}

	std::lock_guard writeLock(meshQueueMutex);
	for (int i = 0; i < MAX_DQ && mainMeshQueue.size() > 0; i++) {
		ChunkRefHandle &handle = mainMeshQueue.front();
		if (handle->getMeshState() == MeshState::DIRTY) {
			ChunkNeighbors n = getChunkNeighbors(handle);
			
			chunkLightEngine->loadLightData(n);
			chunkLightEngine->computeChunkLighting(n);
			mainChunkMesher->loadChunkData(n);

			ChunkGeometry *geometry = meshRecycler.getNew();
			mainChunkMesher->createChunkMesh(geometry);

			ChunkRenderDataHandle &renderData = renderableChunkSet[handle->getHashCode()].second;
			renderData->bufferGeometry(geometry);
			handle->flagMeshValid();

			meshRecycler.recycle(geometry);
		}
		mainMeshQueue.pop();
	}
}

void ChunkManager::enqueueChunks() {
	for (int i = 0; i < 4 && needsLoadingCache.size() > 0; i++) {
		ChunkRefHandle chunk = std::move(needsLoadingCache.back());
		loadedChunkSet.emplace(chunk->getHashCode(), copyChunkRefHandle(chunk));

		needsLoadingCache.pop_back();
		this->chunkGenerationQueue.enqueue(std::move(chunk));
		//chunk->generateTerrain();
	}

	for (int i = 0; i < 4 && needsMeshCache.size() > 0; i++) {
		size_t chunkIndex = needsMeshCache.size() - 1;
		BlockState chunkState = needsMeshCache[chunkIndex]->tryGetBlockState();
		while (chunkState == BlockState::NONE || chunkState == BlockState::LOCKED) {
			if (chunkIndex == 0) {
				return;	//we are done
			}
			chunkIndex--;
			chunkState = needsMeshCache[chunkIndex]->tryGetBlockState();
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
	double time = Window::getTime();
	for (int i = 0; i < 10 && this->chunkMeshedQueue.try_dequeue(element); i++) {
		ChunkRefHandle &chunk = element.first;
		if (isChunkRenderable(chunk->chunk_x, chunk->chunk_y, chunk->chunk_z)) {			//TODO: test if renderdata is new or not
			ChunkRenderDataHandle &data = renderableChunkSet[chunk->getHashCode()].second;

			data->cx = chunk->chunk_x;
			data->cy = chunk->chunk_y;
			data->cz = chunk->chunk_z;
			data->startTime = time;
			data->bufferGeometry(element.second.get());

			chunk->flagMeshValid();
		}
		
	}
}

//todo fix bug where all handles are in queue and the program wants to close causing a 
//assert in IAllocator for not freeing memory
void ChunkManager::chunkGeneratorThread() {
	ChunkRefHandle chunk;

	while (this->runThreads && false) {
		
	}

}

std::atomic<int> threadCount = 0;
void ChunkManager::chunkMeshingThread() {
	//static id for each thread
	thread_local int workerID = threadCount++;

	ChunkRefHandle chunk;
	ChunkNeighborGeometryPair pair;
	while (this->runThreads) {

		for (int i = 0; i < 10; i++) {
			bool dequeued = this->chunkGenerationQueue.try_dequeue(chunk);
			if (dequeued) {
				if (chunk && chunk->getBlockState() == BlockState::NONE) {
					chunk->generateTerrain();
				}
			}
			else {
				break;
			}
		}

		for (int i = 0; i < 1; i++) {
			bool dequeuedRender = this->chunkMeshingQueue.try_dequeue(pair);
			if (dequeuedRender) {
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
			else {
				break;
			}
		}
	}
}

float ChunkManager::intbound(float s, float ds) const{
	// Find the smallest positive t such that s+t*ds is an integer.
	
	//if ds < 0
	// return intbound(-s, -ds)
	int sign = ds >= 0 ? 1 : -1;
	s = sign * s;
	ds = fabsf(ds);

	s = fmodf(s, 1.0f);
	// problem is now s+t*ds = 1
	return (1 - s) / ds;
}

constexpr int ChunkManager::expand(int x) const {
	x &= 0x3FF;
	x = (x | (x << 16)) & 4278190335;
	x = (x | (x << 8)) & 251719695;
	x = (x | (x << 4)) & 3272356035;
	x = (x | (x << 2)) & 1227133513;
	return x;
}

//todo figure out if this hashcode perserves locality with negative integers
constexpr int ChunkManager::hashcode(int i, int j, int k) const {
	//z order curve
	return expand(i) + (expand(j) << 1) + (expand(k) << 2);
}