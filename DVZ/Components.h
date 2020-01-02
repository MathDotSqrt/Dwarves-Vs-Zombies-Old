#pragma once
#include <glm.hpp>
#include <gtx/quaternion.hpp>
#include "PrototypeID.h"

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

struct BulletComponent {
	float timeAlive;
	float lifeTime;
};

struct RenderInstanceComponent {
	unsigned int instanceID;
};

struct PointLightComponent {
	unsigned int lightInstanceID;
	glm::vec3 color;
	float intensity;
};

struct CameraInstanceComponent {
	unsigned int cameraID;
};

struct NetworkComponent {
	unsigned int netID;
	float delta;
};

struct PrototypeComponent {
	PrototypeID ID;
};
