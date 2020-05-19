#include "VoxelCollision.h"
#include <optional>

using namespace Physics;

//Type used for block coord
typedef glm::i32vec3 BlockCoord;
typedef Component::VoxelCollisionSample::VoxelSample VoxelSample;

//optional types for all collision strategies
typedef std::optional<std::pair<float, Voxel::Block>> FaceOptional;
typedef std::optional<std::pair<glm::vec2, Voxel::Block>> EdgeOptional;
typedef std::optional<std::pair<glm::vec3, Voxel::Block>> CornerOptional;

constexpr int X = 0;
constexpr int Y = 1;
constexpr int Z = 2;

FaceOptional sample_collision_y( 
	const Physics::AABB &worldspace_aabb, 
	const glm::vec3 &vel_delta, 
	GetBlockFunc &getBlock 
);

FaceOptional sample_collision_x(
	const Physics::AABB &worldspace_aabb,
	const glm::vec3 &vel_delta,
	GetBlockFunc &getBlock
);

FaceOptional sample_collision_z(
	const Physics::AABB &worldspace_aabb,
	const glm::vec3 &vel_delta,
	GetBlockFunc &getBlock
);

Physics::AABB testing_aabb(
	int comp, 
	float vel, 
	const Physics::AABB &aabb
);
void set_sample(
	int comp, 
	float sign, 
	FaceOptional face, 
	Component::VoxelCollisionSample &sample
);
float handle_collision(
	float target_pos,
	float current_pos,
	float delta
);

template<typename FUNC>
void iterate_block_volume(BlockCoord min, BlockCoord max, FUNC lambda) {
	for (int x = min.x; x <= max.x; x++) {
		for (int z = min.z; z <= max.z; z++) {
			for (int y = min.y; y <= max.y; y++) {
				BlockCoord block_coord(x, y, z);
				lambda(block_coord);
			}
		}
	}
}

glm::vec3 Physics::sample_terrain_collision(
	const glm::vec3 &pos,
	const glm::vec3 &vel,
	const Physics::AABB &aabb,
	Component::VoxelCollisionSample &sample,
	float delta,
	GetBlockFunc &getBlock
) {	
	const auto vel_delta = vel * delta;
	const Physics::AABB worldspace_aabb(aabb.min + pos, aabb.max + pos);


	auto new_vel = vel;
	Component::VoxelCollisionSample new_sample;

	{
		const auto face_y = sample_collision_y(worldspace_aabb, vel_delta, getBlock);
		if (face_y.has_value()) {
			set_sample(Y, vel_delta.y, face_y, new_sample);
			const auto current_pos = vel.y < 0 ? worldspace_aabb.min.y : worldspace_aabb.max.y;
			const auto target_pos = face_y->first;
			new_vel.y = handle_collision(target_pos, current_pos, delta);
			printf("target_pos %f current_pos %f\n", target_pos, current_pos);
		}
	}

	sample = new_sample;

	return new_vel;
}

FaceOptional sample_collision_y(
	const Physics::AABB &worldspace_aabb,
	const glm::vec3 &vel_delta,
	GetBlockFunc &getBlock
) {

	FaceOptional face;

	if (vel_delta.y == 0) {
		return face;
	}

	const auto sample_box = testing_aabb(Y, vel_delta.y, worldspace_aabb);

	const BlockCoord blockMin = glm::floor(sample_box.min);
	const BlockCoord blockMax = glm::floor(sample_box.max);


	auto sample_lambda = [&vel_delta, &sample_box, &face, &getBlock](BlockCoord coord) {
		const auto block = getBlock(coord);

		if (block.getMeshType() != Voxel::MeshType::MESH_TYPE_BLOCK) {
			return;
		}

		const auto block_aabb = Physics::AABB(coord, glm::vec3(coord) + glm::vec3(1));

		if (Physics::intersect(block_aabb, sample_box)) {
			if (vel_delta.y < 0) {
				if (!face.has_value() || face->first < block_aabb.min.y) {
					face = std::make_pair(block_aabb.max.y, block);
				}
			}
			else { 
				if (!face.has_value() || face->first > block_aabb.max.y) {
					face = std::make_pair(block_aabb.min.y, block);
				}
			}
		}

	};

	iterate_block_volume(blockMin, blockMax, sample_lambda);

	return face;
}



Physics::AABB testing_aabb(
	int comp, 
	float vel, 
	const Physics::AABB &aabb
) {
	assert(vel != 0);

	auto new_min = aabb.min;
	auto new_max = aabb.max;

	if (vel < 0) {
		new_min[comp] += vel;
	}
	else {
		new_max[comp] += vel;
	}

	return Physics::AABB(new_min, new_max);
}


void set_sample(
	int comp, 
	float sign, 
	FaceOptional face, 
	Component::VoxelCollisionSample &sample
) {
	assert(comp == X || comp == Y || comp == Z);
	assert(sign != 0);

	switch (comp) {
	case X:
		(sign > 0 ? sample.px : sample.nx) = face;
	case Y:
		(sign > 0 ? sample.py : sample.ny) = face;
	case Z:
		(sign > 0 ? sample.pz : sample.nz) = face;
	}
}

float handle_collision(
	float target_pos, 
	float current_pos, 
	float delta
) {
	return (target_pos - current_pos) / delta;
}