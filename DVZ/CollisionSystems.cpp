#include "CollisionSystems.h"
#include "Components.h"
#include "Engine.h"
#include "ChunkManager.h"
#include "VoxelCollision.h"


using namespace System;

void System::voxel_collision_system(Engine &engine, float delta) {
	using namespace Component;

	const auto &manager = engine.ctx<Voxel::ChunkManager>();
	Physics::GetBlockFunc getBlockFunc = [&manager](const glm::i32vec3 &coord) {
		return manager.getBlockIfMapped(coord);
	};


	auto view = engine.view<Position, Velocity, VoxelCollision>();
	view.each([&getBlockFunc, delta](auto &pos, auto &vel, auto &collision) {
		
		vel = face_collision_handling(pos, vel, collision.aabb, delta, getBlockFunc);
		vel = edge_collision_handling(pos, vel, collision.aabb, delta, getBlockFunc);
		vel = corner_collision_handling(pos, vel, collision.aabb, delta, getBlockFunc);

	});
}
