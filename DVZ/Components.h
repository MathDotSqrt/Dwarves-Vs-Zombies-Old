#pragma once
#include <glm.hpp>
#include "entt.hpp"

using namespace entt;
using namespace glm;

struct PositionComponent {
	vec3 pos;
};

struct RotationComponent {
	vec3 rot;
};

struct ScaleComponent {
	vec3 scale;
};

struct TransformationComponent {
	mat4 mat;
};

struct ParentComponent {
	mat4 parent_transform;
	entity parent;
	
};