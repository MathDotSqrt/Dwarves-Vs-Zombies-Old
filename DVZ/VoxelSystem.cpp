#include "VoxelSystem.h"
#include "engine.h"
#include "components.h"
#include "macrologger.h"
#include "ChunkManager.h"
#include "ChunkRenderDataManager.h"
#include "Scene.h"
#include "Window.h"

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
	

}

void VoxelSystem::chunkLoader() {
	
}

void VoxelSystem::loadChunk(Engine *engine, int cx, int cy, int cz) {
	//Voxel::ChunkHandle chunk = engine->getChunkManager()->generateChunk(cx, 0, cz);
	//chunk->generateTerrain();
	//chunk->generateMesh();
}