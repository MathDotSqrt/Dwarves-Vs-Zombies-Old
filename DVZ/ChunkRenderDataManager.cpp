#include "ChunkRenderDataManager.h"
#include "ChunkManager.h"
#include "macrologger.h"

using namespace Voxel;

ChunkRenderDataManager::ChunkRenderDataManager() {
	for (int i = 0; i < RENDER_CHUNK_WIDTH; i++) {
		for (int j = 0; j < RENDER_CHUNK_WIDTH; j++) {
			auto &handle = renderable[i][j];
			handle.cx = -1000;
			handle.cy = -1000;
			handle.cz = -1000;
		}
	}
}


ChunkRenderDataManager::~ChunkRenderDataManager() {

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

}

void ChunkRenderDataManager::newChunk(int playerCX, int playerCY, int playerCZ, ChunkManager &manager){
	needsMeshCache.clear();
	for (int cz = playerCZ - RENDER_RADIUS; cz < playerCZ + RENDER_RADIUS; cz++) {
		for (int cx = playerCX - RENDER_RADIUS; cx < playerCX + RENDER_RADIUS; cx++) {
			auto &chunk_handle = getRenderableChunk(cx, cz);
			if (!isChunkRenderable(cx, cz, chunk_handle)) {
				needsMeshCache.emplace_back(manager.getChunk(cx, 0, cz));
			}
		}
	}

	auto lambda = [playerCX, playerCY, playerCZ](const ChunkRefHandle &rhs, const ChunkRefHandle &lhs) {
		int distL = std::abs(playerCX - lhs->getChunkX()) + std::abs(playerCY - lhs->getChunkY()) + std::abs(playerCZ - lhs->getChunkZ());
		int distR = std::abs(playerCX - rhs->getChunkX()) + std::abs(playerCY - rhs->getChunkY()) + std::abs(playerCZ - rhs->getChunkZ());

		return distL < distR;
	};

	std::sort(needsMeshCache.begin(), needsMeshCache.end(), lambda);
}

void ChunkRenderDataManager::enqueueChunks() {
	
}

ChunkRenderData& ChunkRenderDataManager::getRenderableChunk(int cx, int cz) {
	int c = (cx + RENDER_RADIUS) % ChunkRenderDataManager::RENDER_CHUNK_WIDTH;
	int r = (cz + RENDER_RADIUS) % ChunkRenderDataManager::RENDER_CHUNK_WIDTH;
	
	return renderable[r][c];
}

bool ChunkRenderDataManager::isChunkRenderable(int cx, int cz) {
	auto &render_data = getRenderableChunk(cx, cz);
	return render_data.cx == cx && render_data.cz == cz;
}

bool ChunkRenderDataManager::isChunkRenderable(int cx, int cz, const ChunkRenderData &handle) {
	return cx == handle.getChunkX() && cz == handle.getChunkZ();
}