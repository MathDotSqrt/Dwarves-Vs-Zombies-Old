#include "CollisionSystems.h"
#include "Components.h"
#include "Engine.h"
#include "ChunkManager.h"
#include "CoordIterator.h"
#include <optional>


using namespace System;

typedef glm::i32vec3 BlockCoord;

glm::i32vec2 convert(float sign, int min, int max);
std::optional<float> x_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager);
std::optional<float> y_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager);
std::optional<float> z_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager);

std::optional<std::pair<float, float>> xy_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager);
std::optional<std::pair<float, float>> yz_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager);
std::optional<std::pair<float, float>> zx_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager);

std::optional<glm::vec3> corner_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager);


void System::voxel_collision_system(Engine &engine, float delta) {
	using namespace Component;

	const auto &manager = engine.ctx<Voxel::ChunkManager>();

	auto view = engine.view<Position, Velocity, const Physics::AABB>();
	view.each([&manager, delta](auto &pos, auto &vel, const auto &aabb) {

		//Face collision handling
		const auto next_pos = pos + vel * delta;
		const auto min = next_pos + aabb.getMin();
		const auto max = next_pos + aabb.getMax();
		const BlockCoord blockMin(glm::floor(min));
		const BlockCoord blockMax(glm::floor(max));

		const auto face_x = x_axis_voxel_intersection(vel, blockMin, blockMax, manager);		//returns closest block face in x axis
		const auto face_y = y_axis_voxel_intersection(vel, blockMin, blockMax, manager);		//returns closest block face in y axis
		const auto face_z = z_axis_voxel_intersection(vel, blockMin, blockMax, manager);		//returns closest block face in z axis

		if (face_x.has_value()) {
			//printf("FACE X\n");
			vel.x = 0;
		}

		if (face_y.has_value()) {
			//printf("FACE Y\n");

			vel.y = 0;
		}

		if (face_z.has_value()) {
			//printf("FACE Z\n");

			vel.z = 0;
		}

		//Edge collision handling
		{
			//vel and pos have been modified by face collision handling. 
			//create new scope to reuse variable names
			const auto next_pos = pos + vel * delta;
			const auto min = next_pos + aabb.getMin();
			const auto max = next_pos + aabb.getMax();
			const BlockCoord blockMin(glm::floor(min));
			const BlockCoord blockMax(glm::floor(max));

			const auto edge_xy = xy_edge_intersection(vel, blockMin, blockMax, manager);			//returns closest edge block faces in xy direction
			const auto edge_yz = yz_edge_intersection(vel, blockMin, blockMax, manager);			//returns closest edge block faces in yz direction
			const auto edge_zx = zx_edge_intersection(vel, blockMin, blockMax, manager);			//returns closest edge block faces in zx direction

			if (edge_xy.has_value()) {
				//printf("EDGE XY\n");
				const auto edge_x_pos = edge_xy->first;
				const auto edge_y_pos = edge_xy->second;

				const auto current_x_pos = vel.x > 0 ? max.x : min.x;
				const auto current_y_pos = vel.y > 0 ? max.y : min.y;

				const auto delta_x = glm::abs(edge_x_pos - current_x_pos);
				const auto delta_y = glm::abs(edge_y_pos - current_y_pos);

				if (delta_x < delta_y && !edge_yz.has_value()) {
					vel.x = 0;
				}
				else if (!edge_zx.has_value()) {
					vel.y = 0;
				}
			}

			if (edge_yz.has_value()) {
				//printf("EDGE YZ\n");

				const auto edge_y_pos = edge_yz->first;
				const auto edge_z_pos = edge_yz->second;

				const auto current_y_pos = vel.y > 0 ? max.y : min.y;
				const auto current_z_pos = vel.z > 0 ? max.z : min.z;

				const auto delta_y = glm::abs(edge_y_pos - current_y_pos);
				const auto delta_z = glm::abs(edge_z_pos - current_z_pos);

				if (delta_y < delta_z && !edge_zx.has_value()) {
					vel.y = 0;
				}
				else if (!edge_xy.has_value()) {
					vel.z = 0;
				}
			}

			if (edge_zx.has_value()) {
				//printf("EDGE ZX\n");

				const auto edge_z_pos = edge_zx->first;
				const auto edge_x_pos = edge_zx->second;

				const auto current_z_pos = vel.z > 0 ? max.z : min.z;
				const auto current_x_pos = vel.x > 0 ? max.x : min.x;

				const auto delta_z = glm::abs(edge_z_pos - current_z_pos);
				const auto delta_x = glm::abs(edge_x_pos - current_x_pos);

				if (delta_z < delta_x && !edge_xy.has_value()) {
					vel.z = 0;
				}
				else if (!edge_yz.has_value()) {
					vel.x = 0;
				}
			}
		}


		//Corner Detection
		{
			const auto next_pos = pos + vel * delta;
			const auto min = next_pos + aabb.getMin();
			const auto max = next_pos + aabb.getMax();
			const BlockCoord blockMin(glm::floor(min));
			const BlockCoord blockMax(glm::floor(max));

			const auto corner = corner_intersection(vel, min, max, manager);

			if(corner.has_value()){
				printf("HAS CORNER\n");
			}
		}

	});
}

glm::i32vec2 convert(float sign, int min, int max) {
	//this is the function that takes in the aabb min and max block coord 
	//and the relevent sign component of the vel vector and returns the inclusive bounds
	//for the block samples for face collision handling


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

std::optional<float> x_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager) {
	if (vel.x == 0) {
		return std::nullopt;	//nothing to return
	}

	std::optional<float> block_face_x;
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
			const Voxel::Block block = manager.getBlockIfMapped(coord);

			if (block.getMeshType() == Voxel::MeshType::MESH_TYPE_BLOCK) {
				block_face_x = vel.x > 0 ? (float)x_coord : (float)x_coord + 1.0f;
			}
		}
	}

	return block_face_x;
}

std::optional<float> y_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager) {
	if (vel.y == 0) {
		return std::nullopt;	//nothing to return
	}

	std::optional<float> block_face_y;
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
			const Voxel::Block block = manager.getBlockIfMapped(coord);

			if (block.getMeshType() == Voxel::MeshType::MESH_TYPE_BLOCK) {
				block_face_y = vel.y > 0 ? (float)y_coord : (float)y_coord + 1.0f;
			}
		}
	}

	return block_face_y;
}

std::optional<float> z_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager) {
	if (vel.z == 0) {
		return std::nullopt;	//nothing to return
	}


	std::optional<float> block_face_z;
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
			const Voxel::Block block = manager.getBlockIfMapped(coord);

			if (block.getMeshType() == Voxel::MeshType::MESH_TYPE_BLOCK) {
				block_face_z = vel.z > 0 ? (float)z_coord : (float)z_coord + 1.0f;
			}
		}
	}

	return block_face_z;
}

std::optional<std::pair<float, float>> xy_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager) {

	if (vel.x == 0 || vel.y == 0) {
		return std::nullopt;
	}

	std::optional<std::pair<float, float>> corner_block;

	const auto x_coord = vel.x > 0 ? max.x : min.x;
	const auto y_coord = vel.y > 0 ? max.y : min.y;

	const auto z_bound = convert(vel.z, min.z, max.z);
	const auto min_z = z_bound[0];
	const auto max_z = z_bound[1];

	for (int z = min_z; z <= max_z; z++) {
		const BlockCoord coord(x_coord, y_coord, z);
		const Voxel::Block block = manager.getBlockIfMapped(coord);

		if (block.getMeshType() == Voxel::MeshType::MESH_TYPE_BLOCK) {
			const float x_pos = vel.x > 0 ? coord.x : coord.x + 1.0f;
			const float y_pos = vel.y > 0 ? coord.y : coord.y + 1.0f;

			corner_block = std::make_pair(x_pos, y_pos);
		}
	}


	return corner_block;
}

std::optional<std::pair<float, float>> yz_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager) {

	if (vel.y == 0 || vel.z == 0) {
		return std::nullopt;
	}

	std::optional<std::pair<float, float>> corner_block;

	const auto y_coord = vel.y > 0 ? max.y : min.y;
	const auto z_coord = vel.z > 0 ? max.z : min.z;

	const auto x_bound = convert(vel.x, min.x, max.x);
	const auto min_x = x_bound[0];
	const auto max_x = x_bound[1];

	for (int x = min_x; x <= max_x; x++) {
		const BlockCoord coord(x, y_coord, z_coord);
		const Voxel::Block block = manager.getBlockIfMapped(coord);

		if (block.getMeshType() == Voxel::MeshType::MESH_TYPE_BLOCK) {
			const float y_pos = vel.y > 0 ? coord.y : coord.y + 1.0f;
			const float z_pos = vel.z > 0 ? coord.z : coord.z + 1.0f;

			corner_block = std::make_pair(y_pos, z_pos);
		}
	}


	return corner_block;
}

std::optional<std::pair<float, float>> zx_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager) {

	if (vel.z == 0 || vel.x == 0) {
		return std::nullopt;
	}

	std::optional<std::pair<float, float>> corner_block;

	const auto z_coord = vel.z > 0 ? max.z : min.z;
	const auto x_coord = vel.x > 0 ? max.x : min.x;

	const auto y_bound = convert(vel.y, min.y, max.y);
	const auto min_y = y_bound[0];
	const auto max_y = y_bound[1];

	for (int y = min_y; y <= max_y; y++) {
		const BlockCoord coord(x_coord, y, z_coord);
		const Voxel::Block block = manager.getBlockIfMapped(coord);

		if (block.getMeshType() == Voxel::MeshType::MESH_TYPE_BLOCK) {
			const float z_pos = vel.z > 0 ? coord.z : coord.z + 1.0f;
			const float x_pos = vel.x > 0 ? coord.x : coord.x + 1.0f;

			corner_block = std::make_pair(z_pos, x_pos);
		}
	}


	return corner_block;
}

std::optional<glm::vec3> corner_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager) {
	if (vel.x == 0 || vel.y == 0 || vel.z == 0) {
		return std::nullopt;
	}

	std::optional<glm::vec3> corner;

	const auto x_coord = vel.x > 0 ? max.x : min.x;
	const auto y_coord = vel.y > 0 ? max.y : min.y;
	const auto z_coord = vel.z > 0 ? max.z : min.z;

	const BlockCoord coord(x_coord, y_coord, z_coord);
	const Voxel::Block block = manager.getBlockIfMapped(coord);

	if (block.getMeshType() == Voxel::MeshType::MESH_TYPE_BLOCK) {
		const float x_pos = vel.x > 0 ? coord.x : coord.x + 1.0f;
		const float y_pos = vel.y > 0 ? coord.y : coord.y + 1.0f;
		const float z_pos = vel.z > 0 ? coord.z : coord.z + 1.0f;

		corner = glm::vec3(x_pos, y_pos, z_pos);
	}

	return corner;
}