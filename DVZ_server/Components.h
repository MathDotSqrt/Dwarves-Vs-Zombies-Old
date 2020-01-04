#pragma once
#include "glm.hpp"
#include "gtx/quaternion.hpp"

struct PositionComponent : glm::vec3 { 
	PositionComponent(glm::vec3 vec) : glm::vec3(vec) {}
};

struct RotationComponent : glm::quat { 
	RotationComponent(glm::quat quat) : glm::quat(quat){}
};

struct VelocityComponent : glm::vec3 { 
	VelocityComponent(glm::vec3 vec) : glm::vec3(vec) {}
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

	InputComponent() : 
		up(false), 
		down(false), 
		left(false), 
		right(false), 
		space(false),
		shift(false),
		ctrl(false) {}
};
