#pragma once
#include <glm.hpp>
#include <gtx/quaternion.hpp>
#include "PrototypeID.h"

struct PlayerComponent {

};

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
	uint32 instanceID;
};

struct PointLightComponent {
	uint32 lightInstanceID;
	glm::vec3 color;
	float intensity;
};

struct CameraInstanceComponent {
	uint32 cameraID;
};

struct NetworkComponent {
	uint32 netID;
};

struct PrototypeComponent {
	PrototypeID ID;
};
