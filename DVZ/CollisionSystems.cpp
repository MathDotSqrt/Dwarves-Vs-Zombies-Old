#include "CollisionSystems.h"
#include "Components.h"
#include "Engine.h"
#include "ChunkManager.h"

using namespace System;

void entity_voxel_collision(Component::Position &pos, Component::Velocity &vel, const Physics::AABB &aabb) {

}

void System::voxel_collision_system(Engine &engine, float delta) {
	using namespace Component;

	auto view = engine.view<Position, Velocity, const Physics::AABB>();
	view.each(entity_voxel_collision);
}
