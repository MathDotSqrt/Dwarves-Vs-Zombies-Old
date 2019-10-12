#include "VoxelSystem.h"
#include "engine.h"
#include "components.h"
#include "macrologger.h"
#include "ChunkManager.h"
#include "Scene.h"

VoxelSystem::VoxelSystem(int priority) : System(priority){
	LOG_SYSTEM("NUMBER OF THREADS: %d", std::thread::hardware_concurrency());
}


VoxelSystem::~VoxelSystem() {

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
		//manager->setBlock(bx, by, bz, b);
	}
}

void VoxelSystem::chunkLoader() {
	
}

void VoxelSystem::loadChunk(Engine *engine, int cx, int cy, int cz) {
	//Voxel::ChunkHandle chunk = engine->getChunkManager()->generateChunk(cx, 0, cz);
	//chunk->generateTerrain();
	//chunk->generateMesh();
}