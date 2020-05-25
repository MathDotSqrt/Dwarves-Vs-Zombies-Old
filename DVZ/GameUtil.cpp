#include "GameUtil.h"
#include "Engine.h"
#include "ChunkManager.h"
#include "Components.h"
#include "Scene.h"

entt::entity GameUtil::spawn_client_spunk(Engine &engine, glm::vec3 pos) {
	using namespace Component;

	auto &scene = engine.ctx<Graphics::Scene>();
	auto &mesh_cache = engine.ctx<ResourceManager::MeshCache>();

	const glm::vec3 FORWARD(0, 0, -1);
	const glm::vec3 UP(0, 1, 0);
	const glm::vec3 RIGHT(1, 0, 0);

	const Physics::AABB aabb(glm::vec3(-.3f, -1.5f, -.3f), glm::vec3(.3f, .3f, .3f));
	//const auto instanceID = scene.createRenderInstance(mesh_cache.handle("SpunkWalker"_hs), Graphics::NormalMaterial());
	const auto lightID = scene.createPointLightInstance(Graphics::Scene::PointLight());

	const auto cameraID = scene.getMainCameraID();

	const auto id = engine.create();
	engine.assign<Player>(id);

	engine.assign<Position>(id, pos);
	engine.assign<Rotation>(id, glm::quat(1, 0, 0, 0));
	engine.assign<Scale>(id, glm::vec3(1));
	engine.assign<Velocity>(id, glm::vec3(0));
	engine.assign<Acceleration>(id, glm::vec3(0));
	engine.assign<RotationalVelocity>(id, glm::vec3(0));
	engine.assign<Input>(id);
	engine.assign<Dir>(id, FORWARD, UP, RIGHT);
	
	engine.assign<CameraInstance>(id, cameraID);
	engine.assign<PointLight>(id, lightID, glm::vec3(1), 60.0f);

	engine.assign<VoxelCollision>(id, VoxelCollision(aabb));

	return id;
}

entt::entity GameUtil::spawn_net_spunk(Engine &engine, glm::vec3 pos, glm::quat rot) {
	using namespace Component;
	
	auto &scene = engine.ctx<Graphics::Scene>();
	auto &mesh_cache = engine.ctx<ResourceManager::MeshCache>();

	const Physics::AABB aabb(glm::vec3(-.3f, -1.5f, -.3f), glm::vec3(.3f, .3f, .3f));
	const auto instanceID = scene.createRenderInstance(mesh_cache.handle("SpunkWalker"_hs), Graphics::NormalMaterial());
	const glm::vec3 offset(0, -1.3, 0);
	const glm::vec3 scale(.3f);


	const auto id = engine.create();
	engine.assign<Position>(id, pos);
	engine.assign<Rotation>(id, rot);
	engine.assign<Scale>(id, glm::vec3(1));
	engine.assign<VoxelCollision>(id, VoxelCollision(aabb));
	engine.assign<RenderInstance>(id, instanceID, offset, scale);

	return id;
}

glm::i32vec3 GameUtil::world_to_block_coord(glm::vec3 vec) {
	return glm::i32vec3(glm::floor(vec));
}

bool GameUtil::attempt_place_block_net(Engine &engine, glm::i32vec3 block_coord, Voxel::Block block){
	using namespace Component;
	
	auto view = engine.view<Position, VoxelCollision>();

	for (auto e : view) {
		const auto pos = view.get<Position>(e);
		const auto coll = view.get<VoxelCollision>(e);

		const auto min = pos + coll.aabb.getMin();
		const auto max = pos + coll.aabb.getMax();

		const auto min_block = world_to_block_coord(min);
		const auto max_block = world_to_block_coord(max);

		if (min_block.x <= block_coord.x && block_coord.x <= max_block.x 
			&& min_block.y <= block_coord.y && block_coord.y <= max_block.y
			&& min_block.z <= block_coord.z && block_coord.z <= max_block.z)
			return false;
	}

	auto &manager = engine.ctx<Voxel::ChunkManager>();
	manager.setBlock(block_coord, block);
	engine.getBlockPlaceBuffer().push_back(std::pair(block_coord, block));

	return true;
}