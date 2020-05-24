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
		normal[comp] = -glm::sign(inv_entry[comp]);
	}
	return entry_time;
}

std::pair<float, glm::vec3> 
calc_nearest_swept_aabb(
	const glm::vec3 &vel_delta,
	const Physics::AABB &moving_aabb,
	std::vector<std::pair<BlockCoord, Voxel::Block>> &broadphase
) {

	int index = 0;
	std::pair<float, glm::vec3> collision = std::make_pair(0, glm::vec3());

	for (int i = 0; i < broadphase.size(); i++) {
		const auto &element = broadphase[i];
		const auto &block_coord = element.first;
		const auto block_aabb = Physics::AABB(block_coord, glm::vec3(block_coord) + glm::vec3(1));

		
		glm::vec3 normal;
		const float collision_time = swept_aabb(vel_delta, moving_aabb, block_aabb, normal);

		if (i == 0 || collision.first > collision_time) {
			index = i;
			collision.first = collision_time;
			collision.second = normal;
		}
	}

	if (broadphase.size() > 0) {
		broadphase.erase(broadphase.begin() + index);
	}

	return collision;
}

glm::vec3 Physics::sample_terrain_collision(
	const glm::vec3 &pos,
	const glm::vec3 &vel,
	const Physics::AABB &aabb,
	float delta,
	std::vector<std::pair<BlockCoord, Voxel::Block>> &broadphase,
	Component::VoxelCollisionSample &sample

) {	
	

	const auto vel_delta = vel * delta;
	
	auto new_pos = pos;
	auto new_vel_delta = vel_delta;
	bool has_collided = 0;
	while (broadphase.size()) {
		const auto worldspace_aabb = Physics::translate_aabb(aabb, new_pos);
		const auto collision = calc_nearest_swept_aabb(vel_delta, worldspace_aabb, broadphase);
		const float collision_time = collision.first;
		new_pos += new_vel_delta * collision_time - (glm::sign(new_vel_delta)) * 0.001f;

		
		if (collision_time < 1) {
			const float remaining_time = 1 - collision_time;
			const auto &normal = collision.second;
			if(glm::sign(normal.y) != 0 && glm::sign(normal.y) != glm::sign(new_vel_delta.y))
				new_vel_delta.y = 0;

			if (glm::sign(normal.x) != 0 && glm::sign(normal.x) != glm::sign(new_vel_delta.x))
				new_vel_delta.x = 0;

			if (glm::sign(normal.z) != 0 && glm::sign(normal.z) != glm::sign(new_vel_delta.z))
				new_vel_delta.z = 0;
		}
		else {
			new_vel_delta = glm::vec3();
			break;
		}
	}
	new_pos += new_vel_delta;
	return (new_pos - pos) / delta;
}
