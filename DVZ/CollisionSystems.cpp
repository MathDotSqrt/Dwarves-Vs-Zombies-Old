#include "CollisionSystems.h"
#include "Components.h"
#include "Engine.h"
#include "ChunkManager.h"
#include "CoordIterator.h"
#include <optional>


using namespace System;

typedef glm::i32vec3 BlockCoord;

std::optional<float> x_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager);
std::optional<float> y_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager);
std::optional<float> z_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager);

std::optional<std::pair<float, float>> xy_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager);
std::optional<std::pair<float, float>> yz_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager);
std::optional<std::pair<float, float>> zx_edge_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager);

void System::voxel_collision_system(Engine &engine, float delta) {
	using namespace Component;

	const auto &manager = engine.ctx<Voxel::ChunkManager>();

	auto view = engine.view<Position, Velocity, const Physics::AABB>();
	view.each([&manager, delta](auto &pos, auto &vel, const auto &aabb) {
		const auto next_pos = pos + vel * delta;
		const BlockCoord min(glm::floor(next_pos + aabb.getMin()));
		const BlockCoord max(glm::floor(next_pos + aabb.getMax()));

		const auto intersect_x = x_axis_voxel_intersection(vel, min, max, manager);
		const auto intersect_y = y_axis_voxel_intersection(vel, min, max, manager);
		const auto intersect_z = z_axis_voxel_intersection(vel, min, max, manager);

		if (intersect_x.has_value()) {
			pos.x = *intersect_x + (vel.x > 0 ? aabb.getMin().x : aabb.getMax().x);

			vel.x = 0;
		}

		if (intersect_y.has_value()) {
			vel.y = 0;
		}

		if (intersect_z.has_value()) {
			vel.z = 0;
		}
		//const auto intersect_y = y_axis_voxel_intersection(vel, min, max, manager);
		//const auto intersect_z = z_axis_voxel_intersection(vel, min, max, manager);
	});
}

std::optional<float> x_axis_voxel_intersection(const glm::vec3 vel, BlockCoord min, BlockCoord max, const Voxel::ChunkManager &manager) {
	if (vel.x == 0) {
		return std::nullopt;	//nothing to return
	}

	std::optional<float> block_face_x;
	const int x_coord = vel.x > 0 ? max.x : min.x;

	//if vel.y or vel.z is nonzero we do not sample those edges 
	//those edges are handled in a separate edge collision handling

	//if vel.y or vel.z is zero there will be no edge collision handling
	//we can sample all edges for face collision handling

	const int min_y = vel.y >= 0 ? min.y : min.y + 1;
	const int max_y = vel.y > 0 ? max.y - 1 : max.y;
	const int min_z = vel.z >= 0 ? min.z : min.z + 1;
	const int max_z = vel.z > 0 ? max.z - 1 : max.z;

	for (int y = min_y; y <= max_y; y++) {
		for (int z = min_z;  z <= max_z; z++) {
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

	//if vel.x or vel.z is nonzero we do not sample those edges 
	//those edges are handled in a separate edge collision handling

	//if vel.x or vel.z is zero there will be no edge collision handling
	//we can sample all edges for face collision handling

	std::optional<float> block_face_y;
	const int y_coord = vel.y > 0 ? max.y : min.y;
	const int min_x = vel.x >= 0 ? min.x : min.x + 1;
	const int max_x = vel.x > 0 ? max.x - 1 : max.x;
	const int min_z = vel.z >= 0 ? min.z : min.z + 1;
	const int max_z = vel.z > 0 ? max.z - 1 : max.z;

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

	//if vel.x or vel.y is nonzero we do not sample those edges 
	//those edges are handled in a separate edge collision handling

	//if vel.x or vel.y is zero there will be no edge collision handling
	//we can sample all edges for face collision handling

	std::optional<float> block_face_z;
	const int z_coord = vel.z > 0 ? max.z : min.z;
	const int min_x = vel.x >= 0 ? min.x : min.x + 1;
	const int max_x = vel.x > 0 ? max.x - 1 : max.x;
	const int min_y = vel.y >= 0 ? min.y : min.y + 1;
	const int max_y = vel.y > 0 ? max.y - 1 : max.y;

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

	for (int z = min.z; z <= max.z; z++) {
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

	for (int x = min.x; x <= max.x; x++) {
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

	const auto z_coord = vel.x > 0 ? max.x : min.x;
	const auto x_coord = vel.y > 0 ? max.y : min.y;

	for (int y = min.y; y <= max.y; y++) {
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