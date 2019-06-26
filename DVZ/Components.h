#pragma once
#include <glm.hpp>
#include "entt.hpp"
#include "VAO.h"
#include "VBO.h"
#include "Shader.h"

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

struct VelocityComponent {
	vec3 vel;
};

struct RotationalVelocityComponent {
	vec3 vel;
};

struct RenderInstanceComponent {
	unsigned int instanceID;
};

struct CameraInstanceComponent {
	
};

