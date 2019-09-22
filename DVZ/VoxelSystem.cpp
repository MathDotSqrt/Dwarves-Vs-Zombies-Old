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
	
	Voxel::ChunkManager *manager = engine->getChunkManager();
	
	PositionComponent &p = engine->get<PositionComponent>(engine->getPlayer());

	int chunkX = manager->getChunkX(p.pos.x);
	int chunkZ = manager->getChunkZ(p.pos.z);

	Graphics::BlockMaterial chunkMat = { {.95f, .7f, .8f}, 30 };

	for (int z = chunkZ - RENDER_DISTANCE / 2; z < chunkZ + RENDER_DISTANCE / 2; z++) {
		for (int x = chunkX - RENDER_DISTANCE / 2; x < chunkX + RENDER_DISTANCE / 2; x++) {
			this->loadChunk(engine, x, 0, z);
		}
	}
}

void VoxelSystem::removedFromEngine(Engine *engine) {

}

void VoxelSystem::update(Engine *engine, float delta) {
	Voxel::ChunkManager *manager = engine->getChunkManager();
	
	//get the chunk the player is currently in
	glm::vec3 playerPos = engine->get<PositionComponent>(engine->getPlayer()).pos;
	int chunkX = manager->getChunkX(playerPos.x);
	int chunkZ = manager->getChunkZ(playerPos.z);

	

	//iterates through all loaded chunks and removes chunks outside of render distance
	Voxel::ChunkManager::ChunkIterator iter = manager->begin();
	while(iter != manager->end()){
		Voxel::Chunk *chunk = iter->second;
		int cx = chunk->getChunkX() - chunkX;
		int cz = chunk->getChunkZ() - chunkZ;

		if (cx > RENDER_DISTANCE / 2 + 1 
			|| cx < -RENDER_DISTANCE / 2 - 1
			|| cz > RENDER_DISTANCE / 2 + 1
			|| cz < -RENDER_DISTANCE / 2 - 1) {
			
			//engine->getScene()->removeRenderInstance(chunk->getChunkRenderID());

			//removes chunk and returns an iterator pointing to the next chunk
			iter = manager->removeChunk(iter);
			
		}
		else {
			if (!chunk->needsMeshUpdate()) {
				chunk->generateMesh();
				//add scene VAO stuff
			}

			iter++;
		}
	}

	//loop with in the players renderdistance square to see if chunks need to be loaded/generated
	for (int z = chunkZ - RENDER_DISTANCE / 2; z < chunkZ + RENDER_DISTANCE / 2; z++) {
		for (int x = chunkX - RENDER_DISTANCE / 2; x < chunkX + RENDER_DISTANCE / 2; x++) {
			//load chunk if it is not loaded and within render distance
			if (!manager->isChunkMapped(x, 0, z)) {
				this->loadChunk(engine, x, 0, z);
			}
		}
	}

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