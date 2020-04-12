#include "CollisionSystems.h"
#include "Components.h"
#include "Engine.h"
#include "ChunkManager.h"
#include "VoxelCollision.h"

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
		

		collision.sample = face_collision_handling(pos, vel, collision, delta, getBlockFunc);
		collision.sample = edge_collision_handling(pos, vel, collision, delta, getBlockFunc);
		collision.sample = corner_collision_handling(pos, vel, collision, delta, getBlockFunc);

		//{
		//	const auto vel_sample = face_collision_handling(pos, vel, collision, delta, getBlockFunc);
		//	vel = vel_sample.first;
		//	collision.sample = vel_sample.second;
		//}
		//{
		//	const auto vel_sample = edge_collision_handling(pos, vel, collision, delta, getBlockFunc);
		//	vel = vel_sample.first;
		//	collision.sample = vel_sample.second;
		//}
		//{
		//	const auto vel_sample = corner_collision_handling(pos, vel, collision, delta, getBlockFunc);
		//	vel = vel_sample.first;
		//	collision.sample = vel_sample.second;
		//}

		//printf(" px %d | py %d | pz %d\n", collision.sample.px.has_value(), collision.sample.py.has_value(), collision.sample.pz.has_value());
		//printf(" nx %d | ny %d | nz %d\n", collision.sample.nx.has_value(), collision.sample.ny.has_value(), collision.sample.nz.has_value());
		//printf("----------------------\n");
	});
}
