#pragma once

#include "glm.hpp"
#include "gtx/quaternion.hpp"
#include "AABB.h"
#include "Block.h"
#include <optional>

namespace Component {
	struct Player {
		bool is_sprinting;
		bool is_grounded;

		Player() : is_sprinting(false), is_grounded(false) {}
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

	struct VoxelCollisionSample {
		typedef std::pair<float, Voxel::Block> Sample;
		std::optional<Sample> px;
		std::optional<Sample> nx;
		std::optional<Sample> py;
		std::optional<Sample> ny;
		std::optional<Sample> pz;
		std::optional<Sample> nz;
	};

	struct VoxelCollision {
		Physics::AABB aabb;
		VoxelCollisionSample sample;

		VoxelCollision(Physics::AABB aabb) : aabb(aabb) {}

	};


}