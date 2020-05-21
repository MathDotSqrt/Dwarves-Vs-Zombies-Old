#include "VoxelCollision.h"
#include <optional>
#include "IntersectionTests.h"

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

glm::vec3 sample_cylinder(
	const Physics::AABB &worldspace_aabb,
	const glm::vec3 &vel_delta,
	GetBlockFunc &func
);

glm::vec3 adjust_vel_for_collision(
	const glm::vec3 &vel,
	const Physics::AABB &worldspace_aabb,
	const Physics::AABB &problem,
	const Voxel::Block block,
	glm::vec<3, FaceOptional> &samples
);

Physics::AABB testing_aabb(
	int comp, 
	float vel, 
	const Physics::AABB &aabb
);

Physics::AABB vel_aabb(
	const glm::vec3 &vel,
	const Physics::AABB &aabb
);

glm::vec3 get_current_pos(glm::vec3 vel, const Physics::AABB &aabb);

void set_sample(
	int comp, 
	float sign, 
	FaceOptional face, 
	Component::VoxelCollisionSample &sample
);
float handle_collision(
	float vel,
	float target_pos,
	float current_pos,
	float delta
);

glm::vec3 handle_collision(
	const Physics::AABB &aabb,
	const Physics::AABB &problem
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
	
	delta = 1 / 60.0f;
	const auto vel_delta = vel * delta;

	const Physics::AABB worldspace_aabb(aabb.min + pos, aabb.max + pos);
	const auto current_pos = get_current_pos(vel, worldspace_aabb);

	auto new_vel = vel;
	Component::VoxelCollisionSample new_sample;
	{
		const auto face_y = sample_collision_y(worldspace_aabb, new_vel * delta, getBlock);
		if (face_y.has_value()) {
			set_sample(Y, vel_delta.y, face_y, new_sample);
			const auto target_pos = face_y->first;
			new_vel.y = handle_collision(vel.y, target_pos, current_pos.y, delta);
			//printf("Y target %f current %f \n", target_pos, current_pos.y);
		}
	}

	{
		const auto face_x = sample_collision_x(worldspace_aabb, new_vel * delta, getBlock);
		if (face_x.has_value()) {
			set_sample(X, vel_delta.x, face_x, new_sample);
			const auto target_pos = face_x->first;
			new_vel.x = handle_collision(vel.x, target_pos, current_pos.x, delta);
			//printf("X target %f current %f \n", target_pos, current_pos.x);

		}
	}

	{
		const auto face_z = sample_collision_z(worldspace_aabb, new_vel * delta, getBlock);
		if (face_z.has_value()) {
			set_sample(Z, vel_delta.z, face_z, new_sample);
			const auto target_pos = face_z->first;
			new_vel.z = handle_collision(vel.z, target_pos, current_pos.z, delta);
			//printf("Z target %f current %f \n", target_pos, current_pos.z);
		}
	}

	{
		new_vel = sample_cylinder(worldspace_aabb, new_vel * delta, getBlock) / delta;
		
		//if (sample.x.has_value()) {
		//	set_sample(X, vel_delta.x, sample.x, new_sample);
		//	const auto target_pos = sample.x->first;
		//	new_vel.x = handle_collision(vel.x, target_pos, current_pos.x, delta);
		//}
		//if (sample.y.has_value()) {
		//	set_sample(Y, vel_delta.y, sample.y, new_sample);
		//	const auto target_pos = sample.y->first;
		//	new_vel.y = handle_collision(vel.y, target_pos, current_pos.y, delta);
		//}
		//if (sample.z.has_value()) {
		//	set_sample(Z, vel_delta.z, sample.z, new_sample);
		//	const auto target_pos = sample.z->first;
		//	new_vel.z = handle_collision(vel.z, target_pos, current_pos.z, delta);
		//}
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
				if (!face.has_value() || face->first < block_aabb.max.y) {
					face = std::make_pair(block_aabb.max.y, block);
				}
			}
			else { 
				if (!face.has_value() || face->first > block_aabb.min.y) {
					face = std::make_pair(block_aabb.min.y, block);
				}
			}
		}

	};

	iterate_block_volume(blockMin, blockMax, sample_lambda);

	return face;
}

FaceOptional sample_collision_x(
	const Physics::AABB &worldspace_aabb,
	const glm::vec3 &vel_delta,
	GetBlockFunc &getBlock
) {

	FaceOptional face;

	if (vel_delta.x == 0) {
		return face;
	}

	const auto sample_box = testing_aabb(X, vel_delta.x, worldspace_aabb);

	const BlockCoord blockMin = glm::floor(sample_box.min);
	const BlockCoord blockMax = glm::floor(sample_box.max);


	auto sample_lambda = [&vel_delta, &sample_box, &face, &getBlock](BlockCoord coord) {
		const auto block = getBlock(coord);

		if (block.getMeshType() != Voxel::MeshType::MESH_TYPE_BLOCK) {
			return;
		}

		const auto block_aabb = Physics::AABB(coord, glm::vec3(coord) + glm::vec3(1));

		if (Physics::intersect(block_aabb, sample_box)) {
			if (vel_delta.x < 0) {
				if (!face.has_value() || face->first < block_aabb.max.x) {
					face = std::make_pair(block_aabb.max.x, block);
				}
			}
			else {
				if (!face.has_value() || face->first > block_aabb.min.x) {
					face = std::make_pair(block_aabb.min.x, block);
				}
			}
		}

	};

	iterate_block_volume(blockMin, blockMax, sample_lambda);

	return face;
}

FaceOptional sample_collision_z(
	const Physics::AABB &worldspace_aabb,
	const glm::vec3 &vel_delta,
	GetBlockFunc &getBlock
) {

	FaceOptional face;

	if (vel_delta.z == 0) {
		return face;
	}

	const auto sample_box = testing_aabb(Z, vel_delta.z, worldspace_aabb);

	const BlockCoord blockMin = glm::floor(sample_box.min);
	const BlockCoord blockMax = glm::floor(sample_box.max);


	auto sample_lambda = [&vel_delta, &sample_box, &face, &getBlock](BlockCoord coord) {
		const auto block = getBlock(coord);

		if (block.getMeshType() != Voxel::MeshType::MESH_TYPE_BLOCK) {
			return;
		}

		const auto block_aabb = Physics::AABB(coord, glm::vec3(coord) + glm::vec3(1));

		if (Physics::intersect(block_aabb, sample_box)) {
			if (vel_delta.z < 0) {
				if (!face.has_value() || face->first < block_aabb.max.z) {
					face = std::make_pair(block_aabb.max.z, block);
				}
			}
			else {
				if (!face.has_value() || face->first > block_aabb.min.z) {
					face = std::make_pair(block_aabb.min.z, block);
				}
			}
		}

	};

	iterate_block_volume(blockMin, blockMax, sample_lambda);

	return face;
}

glm::vec3 sample_cylinder(
	const Physics::AABB &worldspace_aabb,
	const glm::vec3 &vel_delta,
	GetBlockFunc &getBlock
) {
	glm::vec<3, FaceOptional> samples{std::nullopt, std::nullopt, std::nullopt};
	
	auto new_vel_delta = vel_delta;
	auto sample_box = vel_aabb(new_vel_delta, worldspace_aabb);
	auto radius = glm::distance(sample_box.max, sample_box.min);
	auto sample_cylinder = Physics::BoundingCylinder(sample_box.min, sample_box.max, radius);

	const BlockCoord minBlock = glm::floor(sample_box.min);
	const BlockCoord maxBlock = glm::floor(sample_box.max);
	auto sample_lambda = [&](BlockCoord coord) {
		const auto block = getBlock(coord);
		if (block.getMeshType() != Voxel::MeshType::MESH_TYPE_BLOCK) {
			return;
		}

		const auto block_aabb = Physics::AABB(coord, glm::vec3(coord) + glm::vec3(1));

		if (Physics::intersect(sample_cylinder, block_aabb)) {
			if (Physics::intersect(sample_box, block_aabb)) {
				printf("TEST\n");
				new_vel_delta = adjust_vel_for_collision(new_vel_delta, worldspace_aabb, block_aabb, block, samples);
				sample_box = vel_aabb(new_vel_delta, worldspace_aabb);
				radius = glm::distance(sample_box.max, sample_box.min);
				sample_cylinder = Physics::BoundingCylinder(sample_box.min, sample_box.max, radius);
			}
		}
	};

	

	iterate_block_volume(minBlock, maxBlock, sample_lambda);

	if (samples.x.has_value() || samples.y.has_value() || samples.z.has_value()) {
		printf("CYLINDER\n");
	}

	return new_vel_delta;
}

glm::vec3 adjust_vel_for_collision(
	const glm::vec3 &vel,
	const Physics::AABB &worldspace_aabb,
	const Physics::AABB &problem,
	const Voxel::Block block,
	glm::vec<3, FaceOptional> &samples
) {

	auto new_vel = vel;
	if (vel.x < 0) {
		new_vel.x = worldspace_aabb.min.x - problem.max.x;
		samples.x = std::make_pair(problem.max.x, block);
	} 
	if (vel.x > 0) {
		new_vel.x = worldspace_aabb.max.x - problem.min.x;
		samples.x = std::make_pair(problem.min.x, block);
	}
	if (vel.y < 0) {
		new_vel.y = worldspace_aabb.min.y - problem.max.y;
		samples.y = std::make_pair(problem.max.y, block);
	}
	if (vel.y > 0) {
		new_vel.y = worldspace_aabb.max.y - problem.min.y;
		samples.y = std::make_pair(problem.min.y, block);
	}
	if (vel.z < 0) {
		new_vel.z = worldspace_aabb.min.z - problem.max.z;
		samples.z = std::make_pair(problem.max.z, block);
	}
	if (vel.z > 0) {
		const auto delta = worldspace_aabb.max.z - problem.min.z;
		new_vel.z = delta < vel.z ? delta : vel.z;
		samples.z = std::make_pair(problem.min.z, block);
	}

	return new_vel;
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

Physics::AABB vel_aabb(
	const glm::vec3 &vel_delta,
	const Physics::AABB &aabb
) {
	const auto new_min = glm::min(aabb.min + vel_delta, aabb.min);
	const auto new_max = glm::max(aabb.max + vel_delta, aabb.max);

	return Physics::AABB{ new_min, new_max };
}

glm::vec3 get_current_pos(glm::vec3 vel, const Physics::AABB &aabb) {
	const auto &min = aabb.min;
	const auto &max = aabb.max;

	return glm::vec3(vel.x < 0 ? min.x : max.x, vel.y < 0 ? min.y : max.y, vel.z < 0 ? min.z : max.z);
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
		(sign > 0 ? sample.px : sample.nx) = face; break;
	case Y:
		(sign > 0 ? sample.py : sample.ny) = face; break;
	case Z:
		(sign > 0 ? sample.pz : sample.nz) = face; break;
	}
}

float handle_collision(
	float vel,
	float target_pos, 
	float current_pos, 
	float delta
) {

	float new_vel = (target_pos - current_pos) / delta;

	//if (glm::sign(vel) == glm::sign(target_pos - current_pos)) {
	//	return new_vel - glm::sign(vel) * 0.01f;
	//}
	return new_vel;

}