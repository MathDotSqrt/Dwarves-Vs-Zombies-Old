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
	bool flag;
};
//registry.on_replace<position>().disconnect<&my_free_function>();
struct ParentComponent {
	mat4 local_transform;
	entity parent;
	bool needsTransformUpdate;
	//some listener pointer
};