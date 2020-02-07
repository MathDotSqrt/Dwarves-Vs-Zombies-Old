#pragma once

#include "glm.hpp"
#include "gtx/quaternion.hpp"
#include "AABB.h"
#include "Block.h"
#include <optional>

namespace Component {
	struct Player {

	};

	struct Position : glm::vec3 {
		Position(glm::vec3 vec) : glm::vec3(vec) {}
	};

	struct Rotation : glm::quat {
		Rotation(glm::quat quat) : glm::quat(quat) {}
	};

	struct Scale : glm::vec3 {
		Scale(glm::vec3 vec) : glm::vec3(vec) {}
	};

	struct Velocity : glm::vec3 {
		Velocity(glm::vec3 vec) : glm::vec3(vec) {}
	};

	struct RotationalVelocity : glm::vec3 {
		RotationalVelocity(glm::vec3 vec) : glm::vec3(vec) {}
	};

	struct Acceleration : glm::vec3 {
		Acceleration(glm::vec3 vec) : glm::vec3(vec) {}
	};

	struct Dir {
		glm::vec3 forward;
		glm::vec3 up;
		glm::vec3 right;
	};



	struct VoxelCollision {
		Physics::AABB aabb;

		std::optional<Voxel::Block> px = std::nullopt;
		std::optional<Voxel::Block> nx = std::nullopt;
		std::optional<Voxel::Block> py = std::nullopt;
		std::optional<Voxel::Block> ny = std::nullopt;
		std::optional<Voxel::Block> pz = std::nullopt;
		std::optional<Voxel::Block> nz = std::nullopt;

		VoxelCollision(Physics::AABB aabb) : aabb(aabb) {}
	};
}