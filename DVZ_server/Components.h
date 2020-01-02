#pragma once
#include "glm.hpp"
#include "gtx/quaternion.hpp"


struct PositionComponent : glm::vec3 { };

struct RotationComponent : glm::quat { };

struct VelocityComponent : glm::vec3 { };

struct DirComponent {
	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;
};