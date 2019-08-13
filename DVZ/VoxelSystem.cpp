#include "VoxelSystem.h"
#include "engine.h"
#include "components.h"
#include "macrologger.h"
#include "ChunkManager.h"
#include "Scene.h"

VoxelSystem::VoxelSystem(int priority) : System(priority){

}


VoxelSystem::~VoxelSystem() {

}

void VoxelSystem::addedToEngine(Engine *engine) {
	
	Voxel::ChunkManager *manager = engine->getChunkManager();
	
	PositionComponent &p = engine->get<PositionComponent>(engine->getPlayer());

	int chunkX = manager->getChunkX(p.pos.x);
	int chunkZ = manager->getChunkZ(p.pos.z);

	Graphics::BlockMaterial chunkMat = { {.95f, .7f, .8f}, 30 };

	for (int z = chunkZ - RENDER_DISTANCE / 2; z < chunkZ + RENDER_DISTANCE / 2; z++) {
		for (int x = chunkX - RENDER_DISTANCE / 2; x < chunkX + RENDER_DISTANCE / 2; x++) {
			Voxel::Chunk *chunk = manager->generateChunk(x, 0, z);
			chunk->generateTerrain();
			chunk->generateMesh();

			Graphics::Geometry chunkGeometry(chunk->vao, chunk->indexCount);
			unsigned int chunkMesh = engine->getScene()->createMesh(chunkGeometry, chunkMat);
			Graphics::Transformation t = { 
				glm::vec3(x * Voxel::Chunk::CHUNK_RENDER_WIDTH_X, 0, z * Voxel::Chunk::CHUNK_RENDER_WIDTH_Z), 
				glm::vec3(0, 0, 0), 
				glm::vec3(1, 1, 1) 
			};
			unsigned int chunkInstance = engine->getScene()->createRenderInstance(chunkMesh, t);
		}
	}

}

void VoxelSystem::removedFromEngine(Engine *engine) {

}

void VoxelSystem::update(Engine *engine, float delta) {
	Voxel::ChunkManager *manager = engine->getChunkManager();

	glm::vec3 playerPos = engine->get<PositionComponent>(engine->getPlayer()).pos;

	int chunkX = manager->getChunkX(playerPos.x);
	int chunkZ = manager->getChunkZ(playerPos.z);

	Graphics::BlockMaterial chunkMat = { {.95f, .7f, .8f}, 30 };


	for (int z = chunkZ - RENDER_DISTANCE / 2; z < chunkZ + RENDER_DISTANCE / 2; z++) {
		for (int x = chunkX - RENDER_DISTANCE / 2; x < chunkX + RENDER_DISTANCE / 2; x++) {
			
			if (manager->isChunkMapped(x, 0, z)) {
				continue;
			}

			Voxel::Chunk *chunk = manager->generateChunk(x, 0, z);
			chunk->generateTerrain();
			chunk->generateMesh();

			Graphics::Geometry chunkGeometry(chunk->vao, chunk->indexCount);
			unsigned int chunkMesh = engine->getScene()->createMesh(chunkGeometry, chunkMat);
			Graphics::Transformation t = {
				glm::vec3(x * Voxel::Chunk::CHUNK_RENDER_WIDTH_X, 0, z * Voxel::Chunk::CHUNK_RENDER_WIDTH_Z),
				glm::vec3(0, 0, 0),
				glm::vec3(1, 1, 1)
			};
			unsigned int chunkInstance = engine->getScene()->createRenderInstance(chunkMesh, t);
		}
	}

	int bx = manager->getBlockX(playerPos.x);
	int by = manager->getBlockY(playerPos.y);
	int bz = manager->getBlockZ(playerPos.z);

	if (manager->isBlockMapped(bx, by, bz)) {
		Voxel::Block b(Voxel::BlockType::BLOCK_TYPE_PURPLE);
		manager->setBlock(bx, by, bz, b);
		Voxel::Chunk *chunk = manager->getChunk(chunkX, 0, chunkZ);
		if (!chunk->needsMeshUpdate()) {
			chunk->generateMesh();
		}

	}
}