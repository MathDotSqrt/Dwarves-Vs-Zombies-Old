#pragma once
#include <glm.hpp>
#include <gtx/quaternion.hpp>

struct PositionComponent {
	glm::vec3 pos;
};

struct RotationComponent {
	glm::quat rot;
};

struct ScaleComponent {
	glm::vec3 scale;
};

struct VelocityComponent {
	glm::vec3 vel;
};

struct RotationalVelocityComponent {
	glm::vec3 eular;
};

struct DirComponent {
	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;
};

struct InputComponent {};

struct RenderInstanceComponent {
	unsigned int instanceID;
};

struct CameraInstanceComponent {
	unsigned int cameraID;
};

struct NetworkComponent {
	unsigned int netID;
};

