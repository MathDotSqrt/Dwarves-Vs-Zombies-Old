#include "VoxelCollision.h"
#include <optional>

using namespace Physics;

//Type used for block coord
typedef glm::i32vec3 BlockCoord;

//optional types for all collision strategies
typedef std::optional<std::pair<float, Voxel::Block>> FaceOptional;
typedef std::optional<std::pair<glm::vec2, Voxel::Block>> EdgeOptional;
typedef std::optional<std::pair<glm::vec3, Voxel::Block>> CornerOptional;


constexpr float EPSILON = 0.01f;


glm::i32vec2 convert(float sign, int min, int max);
FaceOptional x_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);
FaceOptional y_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);
FaceOptional z_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);

EdgeOptional xy_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);
EdgeOptional yz_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);
EdgeOptional zx_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);

CornerOptional corner_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);

float calc_target_vel(float target_pos, float current_pos, float delta_time) {
	return (target_pos - current_pos) / delta_time * (1 - EPSILON);
}

template<typename OPT, typename FUNC>
OPT 
intersection(
	const glm::vec3 &pos, 
	const glm::vec3 &vel_delta, 
	const Physics::AABB &aabb,
	GetBlockFunc &getBlock,
	FUNC voxel_intersection,
	float sample
) {

	//vel distance to sample
	float INV_VEL_MAX_SAMPLE = (1 / sample);

	//calculating number of subdivisions for vel_delta given max sample length
	const int NUM_ITER = (int)glm::ceil(glm::length(vel_delta) * INV_VEL_MAX_SAMPLE);
	const float INV_NUM_ITER = 1.0f / NUM_ITER;

	//optional stores the optional returned from the intersection test
	OPT optional;
	for (int i = 1; i <= NUM_ITER; i++) {
		//frac is the percentage of the vel we want to sample
		const float FRAC = i * INV_NUM_ITER;
		const auto next_pos = pos + (vel_delta * FRAC);
		const auto next_min = next_pos + aabb.getMin();
		const auto next_max = next_pos + aabb.getMax();
		const BlockCoord blockMin(glm::floor(next_min));
		const BlockCoord blockMax(glm::floor(next_max));

		optional = voxel_intersection(vel_delta, blockMin, blockMax, getBlock);
		//if we intersect early stop
		if (optional.has_value()) {
			break;
		}
	}

	return optional;
}


std::pair<glm::vec3, Component::VoxelCollisionSample> 
Physics::face_collision_handling(glm::vec3 pos, glm::vec3 vel, const Component::VoxelCollision &collision, float delta_time, GetBlockFunc &getBlock) {
	constexpr float SAMPLE = 1.1f;
	const auto &aabb = collision.aabb;
	const auto vel_delta = vel * delta_time;

	const auto face_x = 
		intersection<FaceOptional>( pos, vel_delta, aabb, getBlock, &x_axis_voxel_intersection, SAMPLE);
	const auto face_y = 
		intersection<FaceOptional>(pos, vel_delta, aabb, getBlock, &y_axis_voxel_intersection, SAMPLE);
	const auto face_z = 
		intersection<FaceOptional>(pos, vel_delta, aabb, getBlock, &z_axis_voxel_intersection, SAMPLE);

	Component::VoxelCollisionSample sample;	//default nulls for all the samples
	auto handle_collision = [&] 
	(int component, FaceOptional face, float current_pos) {
		if (face.has_value()) {
			const auto face_pos = face->first;
			const auto face_block = face->second;
			sample.set(component, glm::sign(vel[component]), face_block);
			vel[component] = calc_target_vel(face_pos, current_pos, delta_time);
		}
	};
	
	const auto min = pos + aabb.getMin();
	const auto max = pos + aabb.getMax();

	//modifies the state of vel and sample
	handle_collision(0, face_x, vel.x > 0 ? max.x : min.x);
	handle_collision(1, face_y, vel.y > 0 ? max.y : min.y);
	handle_collision(2, face_z, vel.z > 0 ? max.z : min.z);
	return std::make_pair(vel, sample);
}

std::pair<glm::vec3, Component::VoxelCollisionSample> 
Physics::edge_collision_handling(glm::vec3 pos, glm::vec3 vel, const Component::VoxelCollision &collision, float delta_time, GetBlockFunc &getBlock) {
	constexpr float SAMPLE = 1.5f;
	const auto vel_delta = vel * delta_time;
	const auto &aabb = collision.aabb;
	auto sample = collision.sample;

	const auto edge_xy 
		= intersection<EdgeOptional>(pos, vel_delta, aabb, getBlock, xy_edge_intersection, SAMPLE);			
	const auto edge_yz 
		= intersection<EdgeOptional>(pos, vel_delta, aabb, getBlock, yz_edge_intersection, SAMPLE);
	const auto edge_zx 
		= intersection<EdgeOptional>(pos, vel_delta, aabb, getBlock, zx_edge_intersection, SAMPLE);


	const auto current_min = pos + aabb.getMin();
	const auto current_max = pos + aabb.getMax();
	const auto next_pos = pos + vel_delta;
	const auto next_min = next_pos + aabb.getMin();
	const auto next_max = next_pos + aabb.getMax();

	auto handle_collision 
		= [&](int comp0, int comp1, const EdgeOptional &edge) {
		if (edge.has_value()) {
			const auto edge_pos = edge->first;
			const auto edge_block = edge->second;

			const auto current_pos_0 = vel[comp0] > 0 ? current_max[comp0] : current_min[comp0];
			const auto current_pos_1 = vel[comp1] > 0 ? current_max[comp1] : current_min[comp1];
			const auto next_pos_0 = vel[comp0] > 0 ? next_max[comp0] : next_min[comp0];
			const auto next_pos_1 = vel[comp1] > 0 ? next_max[comp1] : next_min[comp1];

			const auto delta_0 = glm::abs(edge_pos[0] - next_pos_0);
			const auto delta_1 = glm::abs(edge_pos[1] - next_pos_1);

			if (delta_0 < delta_1) {
				sample.set(comp0, glm::sign(vel[comp0]), edge_block);
				vel[comp0] = calc_target_vel(edge_pos[0], current_pos_0, delta_time);
			}
			else {
				sample.set(comp1, glm::sign(vel[comp1]), edge_block);
				vel[comp1] = calc_target_vel(edge_pos[1], current_pos_1, delta_time);
			}
		}
	};

	//modifies state of vel and sample
	handle_collision(0, 1, edge_xy);
	handle_collision(1, 2, edge_yz);
	handle_collision(2, 0, edge_zx);

	return std::make_pair(vel, sample);
}

std::pair<glm::vec3, Component::VoxelCollisionSample>
Physics::corner_collision_handling(glm::vec3 pos, glm::vec3 vel, const Component::VoxelCollision &collision, float delta_time , GetBlockFunc &getBlock) {
	constexpr float SAMPLE = 1.5f;

	const auto &aabb = collision.aabb;
	auto sample = collision.sample;

	const auto vel_delta = vel * delta_time;
	const auto next_pos = pos + vel_delta;
	const auto current_min = pos + aabb.getMin();
	const auto current_max = pos + aabb.getMax();
	const auto next_min = next_pos + aabb.getMin();
	const auto next_max = next_pos + aabb.getMax();

	const auto corner
		= intersection<CornerOptional>(pos, vel_delta, aabb, getBlock, &corner_intersection, SAMPLE);
	if (corner.has_value()) {
		const glm::vec3 current_aabb_pos{
			vel.x > 0 ? current_max.x : current_min.x,
			vel.y > 0 ? current_max.y : current_min.y,
			vel.z > 0 ? current_max.z : current_min.z
		};

		const glm::vec3 next_aabb_pos { 
			vel.x > 0 ? next_max.x : next_min.x, 
			vel.y > 0 ? next_max.y : next_min.y, 
			vel.z > 0 ? next_max.z : next_min.z
		};

		const auto corner_pos = corner->first;
		const auto corner_block = corner->second;

		const auto delta = glm::abs(corner_pos - next_aabb_pos);
		const auto sign_vel = glm::sign(vel);

		//push in the axis with smallest intersection
		if (delta.x < delta.y && delta.x < delta.z) {
			sample.setX(sign_vel.x, corner_block);
			vel.x = calc_target_vel(corner_pos.x, current_aabb_pos.x, delta_time);
		}
		else if (delta.y < delta.z) {
			sample.setY(sign_vel.y, corner_block);
			vel.y = calc_target_vel(corner_pos.y, current_aabb_pos.y, delta_time);
		}
		else {
			sample.setZ(sign_vel.z, corner_block);
			vel.z = calc_target_vel(corner_pos.z, current_aabb_pos.z, delta_time);
		}

	}

	return std::make_pair(vel, sample);
}

glm::i32vec2 convert(float sign, int min, int max) {
	//this is the function that takes in the aabb min and max block coord 
	//and the relevent sign component of the vel vector and returns the inclusive bounds
	//for the block samples for face/edge collision handling


	//if min==max, the bounds returned will be the same
	if (min == max) {
		return glm::i32vec2(min, max);
	}


	//if max > min, the bounds returned will be the bound relevent for face collision
	//detection. The edge will be ommited from bounds if sign is non zero in the direction
	//vel is. Edge collision handling is handeled differently
	//if sign is 0, no edge collision handling would occur, so return original bound
	return glm::i32vec2(sign >= 0 ? min : min + 1, sign > 0 ? max - 1 : max);
}

FaceOptional x_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &getBlock) {
	if (vel.x == 0) {
		return std::nullopt;	//nothing to return
	}

	FaceOptional block_face_x;
	const int x_coord = vel.x > 0 ? max.x : min.x;

	const auto y_bound = convert(vel.y, min.y, max.y);
	const auto z_bound = convert(vel.z, min.z, max.z);

	const int min_y = y_bound[0];
	const int max_y = y_bound[1];
	const int min_z = z_bound[0];
	const int max_z = z_bound[1];

	for (int y = min_y; y <= max_y; y++) {
		for (int z = min_z; z <= max_z; z++) {
			const BlockCoord coord(x_coord, y, z);
			const Voxel::Block block = getBlock(coord);

			if (block.getMeshType() == Voxel::MeshType::MESH_TYPE_BLOCK) {
				const auto x_pos = vel.x > 0 ? (float)x_coord : x_coord + 1.0f;
				block_face_x = std::make_pair(x_pos, block);
			}
		}
	}

	return block_face_x;
}

FaceOptional y_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &getBlock) {
	if (vel.y == 0) {
		return std::nullopt;	//nothing to return
	}

	FaceOptional block_face_y;
	const int y_coord = vel.y > 0 ? max.y : min.y;


	const auto x_bound = convert(vel.x, min.x, max.x);
	const auto z_bound = convert(vel.z, min.z, max.z);

	const int min_x = x_bound[0];
	const int max_x = x_bound[1];
	const int min_z = z_bound[0];
	const int max_z = z_bound[1];

	for (int x = min_x; x <= max_x; x++) {
		for (int z = min_z; z <= max_z; z++) {
			const BlockCoord coord(x, y_coord, z);
			const Voxel::Block block = getBlock(coord);

			if (block.getMeshType() == Voxel::MeshType::MESH_TYPE_BLOCK) {
				const auto y_pos = vel.y >= 0 ? (float)y_coord : y_coord + 1.0f;
				block_face_y = std::make_pair(y_pos, block);
			}
		}
	}

	return block_face_y;
}

FaceOptional z_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &getBlock) {
	if (vel.z == 0) {
		return std::nullopt;	//nothing to return
	}


	FaceOptional block_face_z;
	const int z_coord = vel.z > 0 ? max.z : min.z;

	const auto x_bound = convert(vel.x, min.x, max.x);
	const auto y_bound = convert(vel.y, min.y, max.y);

	const int min_x = x_bound[0];
	const int max_x = x_bound[1];
	const int min_y = y_bound[0];
	const int max_y = y_bound[1];

	for (int x = min_x; x <= max_x; x++) {
		for (int y = min_y; y <= max_y; y++) {
			const BlockCoord coord(x, y, z_coord);
			const Voxel::Block block = getBlock(coord);

			if (block.getMeshType() == Voxel::MeshType::MESH_TYPE_BLOCK) {
				const auto z_pos = vel.z > 0 ? (float)z_coord : z_coord + 1.0f;
				block_face_z = std::make_pair(z_pos, block);
			}
		}
	}

	return block_face_z;
}

EdgeOptional xy_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &getBlock) {

	if (vel.x == 0 || vel.y == 0) {
		return std::nullopt;
	}

	EdgeOptional corner_block;

	const auto x_coord = vel.x > 0 ? max.x : min.x;
	const auto y_coord = vel.y > 0 ? max.y : min.y;

	const auto z_bound = convert(vel.z, min.z, max.z);
	const auto min_z = z_bound[0];
	const auto max_z = z_bound[1];

	for (int z = min_z; z <= max_z; z++) {
		const BlockCoord coord(x_coord, y_coord, z);
		const Voxel::Block block = getBlock(coord);

		if (block.getMeshType() == Voxel::MeshType::MESH_TYPE_BLOCK) {
			const float x_pos = vel.x > 0 ? coord.x : coord.x + 1.0f;
			const float y_pos = vel.y > 0 ? coord.y : coord.y + 1.0f;

			corner_block = std::make_pair(glm::vec2(x_pos, y_pos), block);
		}
	}


	return corner_block;
}

EdgeOptional yz_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &getBlock) {

	if (vel.y == 0 || vel.z == 0) {
		return std::nullopt;
	}

	EdgeOptional corner_block;

	const auto y_coord = vel.y > 0 ? max.y : min.y;
	const auto z_coord = vel.z > 0 ? max.z : min.z;

	const auto x_bound = convert(vel.x, min.x, max.x);
	const auto min_x = x_bound[0];
	const auto max_x = x_bound[1];

	for (int x = min_x; x <= max_x; x++) {
		const BlockCoord coord(x, y_coord, z_coord);
		const Voxel::Block block = getBlock(coord);

		if (block.getMeshType() == Voxel::MeshType::MESH_TYPE_BLOCK) {
			const float y_pos = vel.y > 0 ? coord.y : coord.y + 1.0f;
			const float z_pos = vel.z > 0 ? coord.z : coord.z + 1.0f;

			corner_block = std::make_pair(glm::vec2(y_pos, z_pos), block);
		}
	}


	return corner_block;
}

EdgeOptional zx_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &getBlock) {

	if (vel.z == 0 || vel.x == 0) {
		return std::nullopt;
	}

	EdgeOptional corner_block;

	const auto z_coord = vel.z > 0 ? max.z : min.z;
	const auto x_coord = vel.x > 0 ? max.x : min.x;

	const auto y_bound = convert(vel.y, min.y, max.y);
	const auto min_y = y_bound[0];
	const auto max_y = y_bound[1];

	for (int y = min_y; y <= max_y; y++) {
		const BlockCoord coord(x_coord, y, z_coord);
		const Voxel::Block block = getBlock(coord);

		if (block.getMeshType() == Voxel::MeshType::MESH_TYPE_BLOCK) {
			const float z_pos = vel.z > 0 ? coord.z : coord.z + 1.0f;
			const float x_pos = vel.x > 0 ? coord.x : coord.x + 1.0f;

			corner_block = std::make_pair(glm::vec2(z_pos, x_pos), block);
		}
	}


	return corner_block;
}

CornerOptional corner_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &getBlock) {
	if (vel.x == 0 || vel.y == 0 || vel.z == 0) {
		return std::nullopt;
	}

	CornerOptional corner;

	const auto x_coord = vel.x > 0 ? max.x : min.x;
	const auto y_coord = vel.y > 0 ? max.y : min.y;
	const auto z_coord = vel.z > 0 ? max.z : min.z;

	const BlockCoord coord(x_coord, y_coord, z_coord);
	const Voxel::Block block = getBlock(coord);

	if (block.getMeshType() == Voxel::MeshType::MESH_TYPE_BLOCK) {
		const float x_pos = vel.x > 0 ? coord.x : coord.x + 1.0f;
		const float y_pos = vel.y > 0 ? coord.y : coord.y + 1.0f;
		const float z_pos = vel.z > 0 ? coord.z : coord.z + 1.0f;

		corner = std::make_pair(glm::vec3(x_pos, y_pos, z_pos), block);
	}

	return corner;
}