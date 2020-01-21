#pragma once
#include <glm.hpp>
#include <gtx/quaternion.hpp>
#include "PrototypeID.h"
#include "Block.h"
#include "AABB.h"

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

	struct Input {
		glm::vec2 mousePos[2];		//[0] previous frame position [1] current frame position

		bool up;
		bool down;
		bool left;
		bool right;
		bool space;
		bool shift;
		bool ctrl;
	};

	struct Bullet {
		float timeAlive;
		float lifeTime;
	};

	struct RenderInstance {
		uint32 instanceID;
	};

	struct PointLight {
		uint32 lightInstanceID;
		glm::vec3 color;
		float intensity;
	};

	struct CameraInstance {
		uint32 cameraID;
	};

	struct Network {
		uint32 netID;
	};

	struct Prototype {
		PrototypeID ID;
	};

}

