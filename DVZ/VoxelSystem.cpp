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
	Voxel::ChunkManager *manager = engine->getChunkManager();
	PositionComponent p = engine->get<PositionComponent>(engine->getPlayer());
	RotationComponent r = engine->get<RotationComponent>(engine->getPlayer());
	glm::vec3 playerPos = p.pos;
	glm::vec3 playerRot = glm::eulerAngles(r.rot);

	manager->update(playerPos.x, playerPos.y, playerPos.z);

	static bool badCode = true;

	bool left = Window::isMousePressed(Window::LEFT_CLICK);
	bool right = Window::isMousePressed(Window::RIGHT_CLICK);
	if (badCode && (left || right)) {
		DirComponent dir = engine->get<DirComponent>(engine->getPlayer());
		RotationComponent rot = engine->get<RotationComponent>(engine->getPlayer());
		
		glm::vec3 ray = glm::rotate(rot.rot, dir.forward);
		Voxel::BlockRayCast cast = manager->castRay(playerPos, ray, 10);

		if (left && cast.block != Voxel::Block()) {
			manager->setBlock(cast.nx, cast.ny, cast.nz, Voxel::Block(Voxel::BlockType::BLOCK_TYPE_PURPLE));

		}
		else if (right) {
			manager->setBlock(cast.x, cast.y, cast.z, Voxel::Block());
		}

		badCode = false;
	}
	else {
		badCode = !(left || right);
	}
	
	engine->getChunkRenderDataManager()->update(playerPos, playerRot, *manager);

}

void VoxelSystem::chunkLoader() {
	
}

void VoxelSystem::loadChunk(Engine *engine, int cx, int cy, int cz) {
	//Voxel::ChunkHandle chunk = engine->getChunkManager()->generateChunk(cx, 0, cz);
	//chunk->generateTerrain();
	//chunk->generateMesh();
}