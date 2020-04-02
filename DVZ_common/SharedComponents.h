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

		std::optional<std::pair<float, Voxel::Block>> px;
		std::optional<std::pair<float, Voxel::Block>> nx;
		std::optional<std::pair<float, Voxel::Block>> py;
		std::optional<std::pair<float, Voxel::Block>> ny;
		std::optional<std::pair<float, Voxel::Block>> pz;
		std::optional<std::pair<float, Voxel::Block>> nz;

		VoxelCollision(Physics::AABB aabb) : aabb(aabb) {}

	};


}