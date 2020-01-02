#pragma once
#include "glm.hpp"
#include "gtx/quaternion.hpp"


struct PositionComponent : glm::vec3 { };

struct RotationComponent : glm::quat { };