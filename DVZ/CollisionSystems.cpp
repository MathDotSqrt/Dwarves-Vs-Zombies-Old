#include "GameSystems.h"
#include "Components.h"
#include "Engine.h"
#include "ChunkManager.h"
#include "VoxelCollision.h"
#include "HandleVoxelCollision.h"

#include "Timer.h"

using namespace System;

void System::voxel_collision_system(Engine &engine, float delta) {
	using namespace Component;
	using namespace Physics;

	const auto &manager = engine.ctx<Voxel::ChunkManager>();
	Physics::GetBlockFunc getBlockFunc = [&manager](const glm::i32vec3 &coord) {
		return manager.getBlockIfMapped(coord);
	};


	Util::Performance::Timer timer("Collision");
	auto view = engine.view<Position, Velocity, VoxelCollision>();
	view.each([&getBlockFunc, delta](auto &pos, auto &vel, auto &collision) {
		const auto worldspace_aabb = Physics::translate_aabb(collision.aabb, pos);
		auto broadphase = Physics::broadphase(worldspace_aabb, vel * delta, getBlockFunc);
	
		vel = Physics::sample_terrain_collision(pos, vel, collision.aabb, delta, broadphase, collision.sample);
	});
}
