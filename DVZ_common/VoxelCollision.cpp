#include "VoxelCollision.h"
#include <optional>

using namespace Physics;

//Type used for block coord
typedef glm::i32vec3 BlockCoord;

//optional types for all collision strategies
typedef std::optional<float> FaceOptional;
typedef std::optional<std::pair<float, float>> EdgeOptional;
typedef std::optional<glm::vec3> CornerOptional;

glm::i32vec2 convert(float sign, int min, int max);
FaceOptional x_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);
FaceOptional y_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);
FaceOptional z_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);

EdgeOptional xy_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);
EdgeOptional yz_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);
EdgeOptional zx_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);

CornerOptional corner_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, GetBlockFunc &func);




void Physics::face_block_sample(glm::vec3 next_pos, Component::VoxelCollision &collision, GetBlockFunc &getBlock) {
	const auto min = next_pos + collision.aabb.getMin();
	const auto max = next_pos + collision.aabb.getMax();
	const BlockCoord blockMin(glm::floor(min));
	const BlockCoord blockMax(glm::floor(max));
}



glm::vec3 Physics::face_collision_handling(glm::vec3 pos, glm::vec3 vel, Physics::AABB aabb, float delta, GetBlockFunc &getBlock) {
	
	const auto next_pos = pos + vel * delta;
	const auto min = next_pos + aabb.getMin();
	const auto max = next_pos + aabb.getMax();
	const BlockCoord blockMin(glm::floor(min));
	const BlockCoord blockMax(glm::floor(max));

	const auto face_x = x_axis_voxel_intersection(vel, blockMin, blockMax, getBlock);		//returns closest block face in x axis
	const auto face_y = y_axis_voxel_intersection(vel, blockMin, blockMax, getBlock);		//returns closest block face in y axis
	const auto face_z = z_axis_voxel_intersection(vel, blockMin, blockMax, getBlock);		//returns closest block face in z axis

	if (face_x.has_value()) {
		vel.x = 0;
	}

	if (face_y.has_value()) {
		vel.y = 0;
	}

	if (face_z.has_value()) {
		vel.z = 0;
	}

	return vel;
}

glm::vec3 Physics::edge_collision_handling(glm::vec3 pos, glm::vec3 vel, Physics::AABB aabb, float delta, GetBlockFunc &getBlock) {
	const auto next_pos = pos + vel * delta;
	const auto min = next_pos + aabb.getMin();
	const auto max = next_pos + aabb.getMax();
	const BlockCoord blockMin(glm::floor(min));
	const BlockCoord blockMax(glm::floor(max));

	const auto edge_xy = xy_edge_intersection(vel, blockMin, blockMax, getBlock);			//returns closest edge block faces in xy direction
	const auto edge_yz = yz_edge_intersection(vel, blockMin, blockMax, getBlock);			//returns closest edge block faces in yz direction
	const auto edge_zx = zx_edge_intersection(vel, blockMin, blockMax, getBlock);			//returns closest edge block faces in zx direction

	if (edge_xy.has_value()) {
		//if edge was found return closest face from x and y pos
		const auto edge_x_pos = edge_xy->first;
		const auto edge_y_pos = edge_xy->second;

		//return x and y position of aabb face
		const auto current_x_pos = vel.x > 0 ? max.x : min.x;
		const auto current_y_pos = vel.y > 0 ? max.y : min.y;

		const auto delta_x = glm::abs(edge_x_pos - current_x_pos);
		const auto delta_y = glm::abs(edge_y_pos - current_y_pos);

		//cant push in axis direction if there exists other edges in the way
		const auto can_push_x = true;// !edge_yz.has_value();
		const auto can_push_y = true;// !edge_zx.has_value();

		//the push occurs only on the axis with the smallest intersection for edge xy
		if (delta_x < delta_y && can_push_x) {
			vel.x = 0;
		}
		else if (can_push_y) {
			vel.y = 0;
		}
	}

	if (edge_yz.has_value()) {
		const auto edge_y_pos = edge_yz->first;
		const auto edge_z_pos = edge_yz->second;

		const auto current_y_pos = vel.y > 0 ? max.y : min.y;
		const auto current_z_pos = vel.z > 0 ? max.z : min.z;

		const auto delta_y = glm::abs(edge_y_pos - current_y_pos);
		const auto delta_z = glm::abs(edge_z_pos - current_z_pos);

		const auto can_push_y = true;// !edge_zx.has_value();
		const auto can_push_z = true;// !edge_xy.has_value();

		if (delta_y < delta_z && can_push_y) {
			vel.y = 0;
		}
		else if (can_push_z) {
			vel.z = 0;
		}
	}

	if (edge_zx.has_value()) {
		const auto edge_z_pos = edge_zx->first;
		const auto edge_x_pos = edge_zx->second;

		const auto current_z_pos = vel.z > 0 ? max.z : min.z;
		const auto current_x_pos = vel.x > 0 ? max.x : min.x;

		const auto delta_z = glm::abs(edge_z_pos - current_z_pos);
		const auto delta_x = glm::abs(edge_x_pos - current_x_pos);

		const auto can_push_z = true;// !edge_xy.has_value();
		const auto can_push_x = true;// !edge_yz.has_value();

		if (delta_z < delta_x && can_push_z) {
			vel.z = 0;
		}
		else if (can_push_x) {
			vel.x = 0;
		}
	}

	return vel;
}

glm::vec3 Physics::corner_collision_handling(glm::vec3 pos, glm::vec3 vel, Physics::AABB aabb, float delta, GetBlockFunc &getBlock) {
	const auto next_pos = pos + vel * delta;
	const auto min = next_pos + aabb.getMin();
	const auto max = next_pos + aabb.getMax();
	const BlockCoord blockMin(glm::floor(min));
	const BlockCoord blockMax(glm::floor(max));

	const auto corner = corner_intersection(vel, min, max, getBlock);

	if (corner.has_value()) {
		const glm::vec3 current_pos(vel.x > 0 ? max.x : min.x, vel.y > 0 ? max.y : min.y, vel.z > 0 ? max.z : min.z);
		const auto delta = glm::abs(*corner - current_pos);

		//push in the axis with smallest intersection
		if (delta.x < delta.y && delta.x < delta.z) {
			vel.x = 0;
		}
		else if (delta.y < delta.z) {
			vel.y = 0;
		}
		else {
			vel.z = 0;
		}

	}

	return vel;
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
				block_face_x = vel.x > 0 ? (float)x_coord : (float)x_coord + 1.0f;
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
				block_face_y = vel.y > 0 ? (float)y_coord : (float)y_coord + 1.0f;
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
				block_face_z = vel.z > 0 ? (float)z_coord : (float)z_coord + 1.0f;
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

			corner_block = std::make_pair(x_pos, y_pos);
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

			corner_block = std::make_pair(y_pos, z_pos);
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

			corner_block = std::make_pair(z_pos, x_pos);
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

		corner = glm::vec3(x_pos, y_pos, z_pos);
	}

	return corner;
}