#pragma once

#include "glm.hpp"
#include "gtx/quaternion.hpp"
#include "AABB.h"
#include "Block.h"
#include <optional>

namespace Component {
	struct Player {
		bool can_jump;

		Player() : can_jump(false) {}
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
		std::optional<Voxel::Block> px;
		std::optional<Voxel::Block> nx;
		std::optional<Voxel::Block> py;
		std::optional<Voxel::Block> ny;
		std::optional<Voxel::Block> pz;
		std::optional<Voxel::Block> nz;

		void setX(float sign, std::optional<Voxel::Block> block) {
			if (sign > 0) {
				px = block;
			}
			else{
				nx = block;
			}
		}

		void setY(float sign, std::optional<Voxel::Block> block) {
			if (sign > 0) {
				py = block;
			}
			else {
				ny = block;
			}
		}

		void setZ(float sign, std::optional<Voxel::Block> block) {
			if (sign > 0) {
				pz = block;
			}
			else {
				nz = block;
			}
		}
	};

	struct VoxelCollision {
		Physics::AABB aabb;
		VoxelCollisionSample sample;

		VoxelCollision(Physics::AABB aabb) : aabb(aabb) {}

	};


}