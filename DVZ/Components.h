#pragma once
#include <glm.hpp>
#include <gtx/quaternion.hpp>
#include "PrototypeID.h"

struct PlayerComponent {

};

struct PositionComponent : glm::vec3 {
	PositionComponent(glm::vec3 vec) : glm::vec3(vec) {}
};

struct RotationComponent : glm::quat {
	RotationComponent(glm::quat quat) : glm::quat(quat) {}
};

struct ScaleComponent : glm::vec3 {
	ScaleComponent(glm::vec3 vec) : glm::vec3(vec) {}
};

struct VelocityComponent : glm::vec3 {
	VelocityComponent(glm::vec3 vec) : glm::vec3(vec) {}
};

struct RotationalVelocityComponent : glm::vec3{
	RotationalVelocityComponent(glm::vec3 vec) : glm::vec3(vec) {}
};

struct DirComponent {
	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;
};

struct InputComponent {
	glm::vec2 mousePos[2];		//[0] previous frame position [1] current frame position

	bool up;
	bool down;
	bool left;
	bool right;
	bool space;
	bool shift;
	bool ctrl;
};

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
