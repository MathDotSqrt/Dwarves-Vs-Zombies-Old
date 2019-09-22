#include "VoxelSystem.h"
#include "engine.h"
#include "components.h"
#include "macrologger.h"
#include "ChunkManager.h"
#include "Scene.h"

VoxelSystem::VoxelSystem(int priority) : chunkLoaderThread(&VoxelSystem::chunkLoader, this), System(priority){
	LOG_SYSTEM("NUMBER OF THREADS: %d", std::thread::hardware_concurrency());
}


VoxelSystem::~VoxelSystem() {
	if (this->shouldLoadChunks) {
		this->shouldLoadChunks = false;
		this->chunkLoaderThread.join();
	}
	this->shouldLoadChunks = false;
}

void VoxelSystem::addedToEngine(Engine *engine) {

}

void VoxelSystem::removedFromEngine(Engine *engine) {

}

void VoxelSystem::update(Engine *engine, float delta) {
	Voxel::ChunkManager *manager = engine->getChunkManager();
	PositionComponent p = engine->get<PositionComponent>(engine->getPlayer());
	glm::vec3 playerPos = p.pos;

	manager->update(playerPos.x, playerPos.y, playerPos.z);

	int bx = manager->getBlockX(playerPos.x);
	int by = manager->getBlockY(playerPos.y);
	int bz = manager->getBlockZ(playerPos.z);

	if (manager->isBlockMapped(bx, by, bz)) {
		Voxel::Block b(Voxel::BlockType::BLOCK_TYPE_PURPLE);
		manager->setBlock(bx, by, bz, b);
	}
}

void VoxelSystem::chunkLoader() {
	LOG_SYSTEM("Starting chunk loading thread...");
	while (this->shouldLoadChunks) {
		LOG_SYSTEM("THREAD");
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	LOG_SYSTEM("Thread stopped");
}

void VoxelSystem::loadChunk(Engine *engine, int cx, int cy, int cz) {
	Voxel::Chunk *chunk = engine->getChunkManager()->generateChunk(cx, 0, cz);
	chunk->generateTerrain();
	chunk->generateMesh();
}