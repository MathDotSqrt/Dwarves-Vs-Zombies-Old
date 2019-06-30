
#pragma once
#include <glm.hpp>
#include <gtx/quaternion.hpp>
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
	quat rot;
};

struct ScaleComponent {
	vec3 scale;
};

struct VelocityComponent {
	vec3 vel;
};

struct RotationalVelocityComponent {
	vec3 eular;
};

struct DirComponent {
	vec3 forward;
	vec3 up;
	vec3 right;
};

struct InputComponent {};

struct RenderInstanceComponent {
	unsigned int instanceID;
};

struct CameraInstanceComponent {
	unsigned int cameraID;
};

