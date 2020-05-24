#include "VoxelCollision.h"
#include <optional>
#include <algorithm>
#include "IntersectionTests.h"
#include "gtx/component_wise.hpp"


using namespace Physics;

//Type used for block coord
typedef Component::VoxelCollisionSample::VoxelSample VoxelSample;

//optional types for all collision strategies
typedef std::optional<std::pair<float, Voxel::Block>> FaceOptional;

constexpr int X = 0;
constexpr int Y = 1;
constexpr int Z = 2;

int max_component(const glm::vec3 &vec){
	return vec.y > vec.x ? (vec.z > vec.y ? 2 : 1) : (vec.z > vec.x ? 2 : 0);
}

Physics::AABB get_swept_broadphase_box(const Physics::AABB &aabb, const glm::vec3 &vel) {
	const auto new_min = glm::min(aabb.min, aabb.min + vel);
	const auto new_max = glm::max(aabb.max, aabb.max + vel);

	return Physics::AABB{ new_min, new_max };
}

std::vector<std::pair<BlockCoord, Voxel::Block>> Physics::broadphase(
	const Physics::AABB &worldspace_aabb,
	const glm::vec3 &vel_delta,
	GetBlockFunc &getBlock
) {

	//fine packing because same cache line
	std::vector<std::pair<BlockCoord, Voxel::Block>> potential_collisions;

	const auto broadphase_box = get_swept_broadphase_box(worldspace_aabb, vel_delta);

	const BlockCoord block_min = glm::floor(broadphase_box.min);
	const BlockCoord block_max = glm::floor(broadphase_box.max);
	for (int x = block_min.x; x <= block_max.x; x++) {
		for (int z = block_min.z; z <= block_max.z; z++) {
			for (int y = block_min.y; y <= block_max.y; y++) {
				const BlockCoord block_coord(x, y, z);
				const auto block = getBlock(block_coord);
				if (block.getMeshType() == Voxel::MeshType::MESH_TYPE_BLOCK) {
					potential_collisions.emplace_back(block_coord, block);
				}
			}
		}
	}


	return potential_collisions;
}

float swept_aabb(
	const glm::vec3 &vel_delta,
	const Physics::AABB &moving_aabb,
	const Physics::AABB &static_aabb,
	glm::vec3 &normal
) {

	glm::vec3 inv_entry;
	glm::vec3 inv_exit;

	if (vel_delta.x > 0) {
		inv_entry.x = static_aabb.min.x - moving_aabb.max.x;
		inv_exit.x = static_aabb.max.x - moving_aabb.min.x;
	}
	else {
		inv_entry.x = static_aabb.max.x - moving_aabb.min.x;
		inv_exit.x = static_aabb.min.x - moving_aabb.max.x;
	}

	if (vel_delta.y > 0) {
		inv_entry.y = static_aabb.min.y - moving_aabb.max.y;
		inv_exit.y = static_aabb.max.y - moving_aabb.min.y;
	}
	else {
		inv_entry.y = static_aabb.max.y - moving_aabb.min.y;
		inv_exit.y = static_aabb.min.y - moving_aabb.max.y;
	}

	if (vel_delta.z > 0) {
		inv_entry.z = static_aabb.min.z - moving_aabb.max.z;
		inv_exit.z = static_aabb.max.z - moving_aabb.min.z;
	}
	else {
		inv_entry.z = static_aabb.max.z - moving_aabb.min.z;
		inv_exit.z = static_aabb.min.z - moving_aabb.max.z;
	}

	glm::vec3 entry;
	glm::vec3 exit;

	if (vel_delta.x == 0) {
		entry.x = -std::numeric_limits<float>::infinity();
		exit.x = std::numeric_limits<float>::infinity();
	}
	else {
		entry.x = inv_entry.x / vel_delta.x;
		exit.x = inv_exit.x / vel_delta.x;
	}

	if (vel_delta.y == 0) {
		entry.y = -std::numeric_limits<float>::infinity();
		exit.y = std::numeric_limits<float>::infinity();
	}
	else {
		entry.y = inv_entry.y / vel_delta.y;
		exit.y = inv_exit.y / vel_delta.y;
	}

	if (vel_delta.z == 0) {
		entry.z = -std::numeric_limits<float>::infinity();
		exit.z = std::numeric_limits<float>::infinity();
	}
	else {
		entry.z = inv_entry.z / vel_delta.z;
		exit.z = inv_exit.z / vel_delta.z;
	}

	const float entry_time = glm::compMax(entry);
	const float exit_time = glm::compMin(exit);
	
	const auto all_negative = glm::all(glm::lessThan(entry, glm::vec3(0)));
	const auto any_above_one = glm::any(glm::greaterThan(entry, glm::vec3(1)));
	if (entry_time > exit_time || all_negative || any_above_one) {
		//no collision
		normal = glm::vec3(0);
		return 1.0f;
	}
	else {
		int comp = max_component(entry);
		normal = glm::vec3(0);
		normal[comp] = (inv_entry[comp] <= 0 ? 1 : -1);
	}
	return entry_time;
}

glm::vec3 Physics::sample_terrain_collision(
	const glm::vec3 &pos,
	const glm::vec3 &vel,
	const Physics::AABB &aabb,
	float delta,
	const std::vector<std::pair<BlockCoord, Voxel::Block>> &broadphase,
	Component::VoxelCollisionSample &sample

) {	
	
	const auto vel_delta = vel * delta;
	auto new_pos = pos;
	auto new_vel_delta = vel_delta;



	for (const auto pair : broadphase) {
		if (glm::all(glm::equal(new_vel_delta, glm::vec3(0)))) {
			break;
		}
		
		const auto worldspace_aabb = Physics::translate_aabb(aabb, new_pos);

		const auto block_coord = pair.first;
		const auto block = pair.second;

		const auto block_aabb = Physics::AABB(block_coord, glm::vec3(block_coord) + glm::vec3(1));
		
		
		glm::vec3 normal;
		const float collision_time = swept_aabb(new_vel_delta, worldspace_aabb, block_aabb, normal) * 0.99999f;
		new_pos += new_vel_delta * collision_time;

		if (collision_time < 1.0f) {

			const float remaining_time = 1 - collision_time;
			const float dot = glm::dot(new_vel_delta, normal) * remaining_time;

			//new_vel_delta = glm::vec3();
			if(normal.y != 0)
				new_vel_delta.y = 0;
			//new_vel_delta = glm::vec3();
		}
	}

	new_pos += new_vel_delta;
	if (broadphase.size()) {
		return (new_pos - pos) / delta;
	}
	else {
		return vel;
	}
}
