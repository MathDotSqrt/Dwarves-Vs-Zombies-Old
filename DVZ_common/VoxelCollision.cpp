#include "VoxelCollision.h"
#include <optional>

using namespace Physics;

//Type used for block coord
typedef glm::i32vec3 BlockCoord;

//optional types for all collision strategies
typedef std::optional<std::pair<float, Voxel::Block>> FaceOptional;
typedef std::optional<std::pair<glm::vec2, Voxel::Block>> EdgeOptional;
typedef std::optional<std::pair<glm::vec3, Voxel::Block>> CornerOptional;


constexpr float EPSILON = 0.001f;


glm::i32vec2 convert(float sign, int min, int max);
FaceOptional x_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);
FaceOptional y_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);
FaceOptional z_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);

EdgeOptional xy_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);
EdgeOptional yz_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);
EdgeOptional zx_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);

CornerOptional corner_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);

std::pair<glm::vec3, Component::VoxelCollisionSample> 
Physics::face_collision_handling(glm::vec3 pos, glm::vec3 vel, const Component::VoxelCollision &collision, float delta_time, GetBlockFunc &getBlock) {
	const auto &aabb = collision.aabb;

	const auto min = pos + aabb.getMin();
	const auto max = pos + aabb.getMax();

	const auto next_pos = pos + vel * delta_time;
	const auto next_min = next_pos + aabb.getMin();
	const auto next_max = next_pos + aabb.getMax();
	const BlockCoord blockMin(glm::floor(next_min));
	const BlockCoord blockMax(glm::floor(next_max));

	const auto face_x = x_axis_voxel_intersection(vel, blockMin, blockMax, getBlock);		//returns closest block face in x axis
	const auto face_y = y_axis_voxel_intersection(vel, blockMin, blockMax, getBlock);		//returns closest block face in y axis
	const auto face_z = z_axis_voxel_intersection(vel, blockMin, blockMax, getBlock);		//returns closest block face in z axis

	auto handle_collision = [delta_time, vel] (int component, FaceOptional face, float current_pos) {
		std::optional<Voxel::Block> block;
		
		if (face.has_value()) {
			const auto face_pos = face->first;
			const auto face_block = face->second;
			const auto new_vel = (face_pos - current_pos) / delta_time;

			block = face_block;
			return std::make_pair((float)0, block);
		}

		return std::make_pair(vel[component], block);
	};
	
	const auto vel_block_x = handle_collision(0, face_x, vel.x > 0 ? max.x : min.x);
	const auto vel_block_y = handle_collision(1, face_y, vel.y > 0 ? max.y : min.y);
	const auto vel_block_z = handle_collision(2, face_z, vel.z > 0 ? max.z : min.z);

	Component::VoxelCollisionSample sample;	//default nulls for all the samples
	sample.setX(glm::sign(vel.x), vel_block_x.second);
	sample.setY(glm::sign(vel.y), vel_block_y.second);
	sample.setZ(glm::sign(vel.z), vel_block_z.second);

	glm::vec3 new_vel { vel_block_x.first, vel_block_y.first, vel_block_z.first };
	return std::make_pair(new_vel, sample);
}

std::pair<glm::vec3, Component::VoxelCollisionSample> 
Physics::edge_collision_handling(glm::vec3 pos, glm::vec3 vel, const Component::VoxelCollision &collision, float delta, GetBlockFunc &getBlock) {
	const auto &aabb = collision.aabb;
	auto sample = collision.sample;

	const auto next_pos = pos + vel * delta;
	const auto min = next_pos + aabb.getMin();
	const auto max = next_pos + aabb.getMax();
	const BlockCoord blockMin(glm::floor(min));
	const BlockCoord blockMax(glm::floor(max));

	const auto edge_xy = xy_edge_intersection(vel, blockMin, blockMax, getBlock);			//returns closest edge block faces in xy direction
	const auto edge_yz = yz_edge_intersection(vel, blockMin, blockMax, getBlock);			//returns closest edge block faces in yz direction
	const auto edge_zx = zx_edge_intersection(vel, blockMin, blockMax, getBlock);			//returns closest edge block faces in zx direction

	if (edge_xy.has_value()) {
		const auto edge_pos = edge_xy->first;
		const auto edge_block = edge_xy->second;

		//if edge was found return closest face from x and y pos
		const auto edge_x_pos = edge_pos[0];
		const auto edge_y_pos = edge_pos[1];

		//return x and y position of aabb face
		const auto current_x_pos = vel.x > 0 ? max.x : min.x;
		const auto current_y_pos = vel.y > 0 ? max.y : min.y;

		const auto delta_x = glm::abs(edge_x_pos - current_x_pos);
		const auto delta_y = glm::abs(edge_y_pos - current_y_pos);

		//the push occurs only on the axis with the smallest intersection for edge xy
		if (delta_x < delta_y) {
			sample.setX(glm::sign(vel.x), edge_block);
			vel.x = 0;
		}
		else {
			sample.setY(glm::sign(vel.y), edge_block);

			vel.y = 0;
		}
	}

	if (edge_yz.has_value()) {
		const auto edge_pos = edge_yz->first;
		const auto edge_block = edge_yz->second;

		const auto edge_y_pos = edge_pos[0];
		const auto edge_z_pos = edge_pos[1];

		const auto current_y_pos = vel.y > 0 ? max.y : min.y;
		const auto current_z_pos = vel.z > 0 ? max.z : min.z;

		const auto delta_y = glm::abs(edge_y_pos - current_y_pos);
		const auto delta_z = glm::abs(edge_z_pos - current_z_pos);

		if (delta_y < delta_z) {
			sample.setY(glm::sign(vel.y), edge_block);
			vel.y = 0;
		}
		else {
			sample.setZ(glm::sign(vel.z), edge_block);
			vel.z = 0;
		}
	}

	if (edge_zx.has_value()) {
		const auto edge_pos = edge_zx->first;
		const auto edge_block = edge_zx->second;

		const auto edge_z_pos = edge_pos[0];
		const auto edge_x_pos = edge_pos[1];

		const auto current_z_pos = vel.z > 0 ? max.z : min.z;
		const auto current_x_pos = vel.x > 0 ? max.x : min.x;

		const auto delta_z = glm::abs(edge_z_pos - current_z_pos);
		const auto delta_x = glm::abs(edge_x_pos - current_x_pos);

		if (delta_z < delta_x) {
			sample.setZ(glm::sign(vel.z), edge_block);
			vel.z = 0;
		}
		else {
			sample.setX(glm::sign(vel.x), edge_block);
			vel.x = 0;
		}
	}

	return std::make_pair(vel, sample);
}

std::pair<glm::vec3, Component::VoxelCollisionSample> 
Physics::corner_collision_handling(glm::vec3 pos, glm::vec3 vel, const Component::VoxelCollision &collision, float delta, GetBlockFunc &getBlock) {
	const auto &aabb = collision.aabb;
	auto sample = collision.sample;

	const auto next_pos = pos + vel * delta;
	const auto min = next_pos + aabb.getMin();
	const auto max = next_pos + aabb.getMax();
	const BlockCoord blockMin(glm::floor(min));
	const BlockCoord blockMax(glm::floor(max));

	const auto corner = corner_intersection(vel, min, max, getBlock);

	if (corner.has_value()) {
		const glm::vec3 current_pos(vel.x > 0 ? max.x : min.x, vel.y > 0 ? max.y : min.y, vel.z > 0 ? max.z : min.z);
		const auto corner_pos = corner->first;
		const auto corner_block = corner->second;

		const auto delta = glm::abs(corner_pos - current_pos);
		const auto sign_vel = glm::sign(vel);
		//push in the axis with smallest intersection
		if (delta.x < delta.y && delta.x < delta.z) {
			sample.setX(sign_vel.x, corner_block);
			vel.x = 0;
		}
		else if (delta.y < delta.z) {
			sample.setY(sign_vel.y, corner_block);
			vel.y = 0;
		}
		else {
			sample.setZ(sign_vel.z, corner_block);
			vel.z = 0;
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
				const auto y_pos = vel.y > 0 ? (float)y_coord : y_coord + 1.0f;
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