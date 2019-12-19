#include "ChunkRenderDataManager.h"
#include "ChunkManager.h"
#include "macrologger.h"
#include "ChunkMesher.h"
#include "Window.h"

using namespace Voxel;

ChunkRenderDataManager::ChunkRenderDataManager(Util::Allocator::IAllocator &parent) : 
	chunkMesherAllocator(2 * sizeof(ChunkMesher), parent),
	geometryRecycler(GEOMETRY_ALLOC_SIZE, parent),
	meshingThread(std::thread(&ChunkRenderDataManager::theadedMesher, this))
{
	for (int i = 0; i < RENDER_CHUNK_WIDTH; i++) {
		for (int j = 0; j < RENDER_CHUNK_WIDTH; j++) {
			auto &handle = renderable[i][j];
			handle.cx = -1000;
			handle.cy = -1000;
			handle.cz = -1000;
		}
	}
	mainMesher = Util::Allocator::allocateNew<ChunkMesher>(chunkMesherAllocator);
	mesher = Util::Allocator::allocateNew<ChunkMesher>(chunkMesherAllocator);
}


ChunkRenderDataManager::~ChunkRenderDataManager() {
	Util::Allocator::free(chunkMesherAllocator, mainMesher);
	Util::Allocator::free(chunkMesherAllocator, mesher);

	isRunning = false;
	meshingThread.join();
}

void ChunkRenderDataManager::update(glm::vec3 pos, glm::vec3 rot, ChunkManager &manager) {
	int cx = manager.getChunkX(pos.x);
	int cy = manager.getChunkX(pos.y);
	int cz = manager.getChunkX(pos.z);

	if (cx != currentCX || cy != currentCY || cz != currentCZ) {
		currentCX = cx;
		currentCY = cy;
		currentCZ = cz;

		newChunk(cx, cy, cz, manager);
	}

	updateDirtyChunks(manager);
	enqueueChunks(manager);
	dequeueChunks(manager);

	visibleChunks.clear();
	for (int _cz = currentCZ - RENDER_RADIUS; _cz < currentCZ + RENDER_RADIUS; _cz++) {
		for (int _cx = currentCX - RENDER_RADIUS; _cx < currentCX + RENDER_RADIUS; _cx++) {
			ChunkRenderData &data = getRenderableChunk(_cx, _cz);
			if (isChunkRenderable(_cx, _cz, data)) {
				visibleChunks.push_back(makeRenderDataCopy(data));
			}
		}
	}
}

std::vector<RenderDataCopy>::const_iterator ChunkRenderDataManager::begin() {
	return visibleChunks.begin();
}

std::vector<RenderDataCopy>::const_iterator ChunkRenderDataManager::end() {
	return visibleChunks.end();
}

void ChunkRenderDataManager::updateDirtyChunks(ChunkManager &manager) {
	const int MAX_DQ = 4;
	
	ChunkQueueSet &dirtyChunks = manager.getDirtyChunkQueue();
	for (int i = 0; i < MAX_DQ && dirtyChunks.size(); i++) {
		ChunkRefHandle &handle = dirtyChunks.front();
		if (handle->getMeshState() == MeshState::DIRTY) {
			ChunkNeighbors n = manager.getChunkNeighbors(handle);
			mainMesher->loadChunkData(n);

			ChunkGeometry *geometry = geometryRecycler.getNew();
			mainMesher->createChunkMesh(geometry);

			ChunkRenderData &data = getRenderableChunk(handle);
			data.bufferGeometry(geometry);
			handle->flagMeshValid();

			geometryRecycler.recycle(geometry);
		}
		dirtyChunks.pop();
	}
}

void ChunkRenderDataManager::newChunk(int playerCX, int playerCY, int playerCZ, ChunkManager &manager){
	needsMeshCache.clear();
	for (int cz = playerCZ - RENDER_RADIUS; cz < playerCZ + RENDER_RADIUS; cz++) {
		for (int cx = playerCX - RENDER_RADIUS; cx < playerCX + RENDER_RADIUS; cx++) {
			auto &chunk_handle = getRenderableChunk(cx, cz);
			bool isRenderable = isChunkRenderable(cx, cz, chunk_handle);
			bool isChunkQueued = std::find(queuedChunks.begin(), queuedChunks.end(), Chunk::calcHashCode(cx, 0, cz)) != queuedChunks.end();
			if (!isRenderable && !isChunkQueued) {
				needsMeshCache.emplace_back(manager.getChunk(cx, 0, cz));
			}
		}
	}

	std::sort(needsMeshCache.begin(), needsMeshCache.end(), 
		[playerCX, playerCY, playerCZ](const ChunkRefHandle &rhs, const ChunkRefHandle &lhs) {
		int distL = std::abs(playerCX - lhs->getChunkX()) + std::abs(playerCY - lhs->getChunkY()) + std::abs(playerCZ - lhs->getChunkZ());
		int distR = std::abs(playerCX - rhs->getChunkX()) + std::abs(playerCY - rhs->getChunkY()) + std::abs(playerCZ - rhs->getChunkZ());

		return distL < distR;
	});
}



void ChunkRenderDataManager::enqueueChunks(ChunkManager &manager) {
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

		ChunkNeighbors chunkNeighbors = manager.getChunkNeighbors(needsMeshCache[chunkIndex]);

		int hashCode = chunkNeighbors.middle->getHashCode();
		queuedChunks.push_back(hashCode);

		ChunkGeometryHandle geometryHandle = geometryRecycler.getUniqueNew();
		chunkMeshingQueue.enqueue(std::make_pair(std::move(chunkNeighbors), std::move(geometryHandle)));
		needsMeshCache.erase(needsMeshCache.begin() + chunkIndex);
	}
}

void ChunkRenderDataManager::dequeueChunks(ChunkManager &manager) {
	ChunkGeometryPair element;
	double time = Window::getTime();

	for (int i = 0; i < 10 && this->chunkMeshedQueue.try_dequeue(element); i++) {
		ChunkRefHandle &chunk = element.first;
		
		auto hashcode = chunk->getHashCode();
		auto found = std::find(queuedChunks.begin(), queuedChunks.end(), hashcode);
		queuedChunks.erase(found);

		int cx = chunk->getChunkX();
		int cy = chunk->getChunkY();
		int cz = chunk->getChunkZ();

		auto &data = getRenderableChunk(cx, cz);

		if (!isChunkRenderable(cx, cz, data)) {
			data.cx = cx;
			data.cy = cy;
			data.cz = cz;
			data.startTime = time;
			data.bufferGeometry(element.second.get());

			chunk->flagMeshValid();
		}
	}
}

void ChunkRenderDataManager::theadedMesher() {
	ChunkNeighborGeometryPair pair;

	while (isRunning) {
		bool dequeued = chunkMeshingQueue.try_dequeue(pair);
		if (dequeued) {
			ChunkNeighbors &neighbors = pair.first;
			ChunkGeometryHandle &geometry = pair.second;

			mesher->loadChunkDataAsync(neighbors);
			mesher->createChunkMesh(geometry.get());

			chunkMeshedQueue.enqueue(std::make_pair(std::move(neighbors.middle), std::move(geometry)));
		}
	}
}

RenderDataCopy ChunkRenderDataManager::makeRenderDataCopy(const ChunkRenderData &data) {
	RenderDataCopy copy;
	copy.pos = glm::vec3(data.cx, data.cy, data.cz);
	copy.vaoID = data.vao.getID();
	copy.startTime = (float)data.startTime;
	copy.indexCount = data.indexCount;
	return copy;
}

ChunkRenderData& ChunkRenderDataManager::getRenderableChunk(int cx, int cz) {
	const int width = ChunkRenderDataManager::RENDER_CHUNK_WIDTH;

	int c = (cx + RENDER_RADIUS) % width;
	int r = (cz + RENDER_RADIUS) % width;
	int new_r = r >= 0 ? r : r + width;
	int new_c = c >= 0 ? c : c + width;
	//assert(new_c >= 0 && new_c < width);
	//assert(new_r >= 0 && new_r < width);
	return renderable[new_r][new_c];
}

ChunkRenderData& ChunkRenderDataManager::getRenderableChunk(const ChunkRefHandle &handle) {
	return getRenderableChunk(handle->getChunkX(), handle->getChunkZ());
}

bool ChunkRenderDataManager::isChunkRenderable(int cx, int cz) {
	auto &render_data = getRenderableChunk(cx, cz);
	return render_data.cx == cx && render_data.cz == cz;
}

bool ChunkRenderDataManager::isChunkRenderable(int cx, int cz, const ChunkRenderData &handle) {
	return cx == handle.getChunkX() && cz == handle.getChunkZ();
}