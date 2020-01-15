#pragma once

/*SERVER COMPONENTS*/

#include "glm.hpp"
#include "gtx/quaternion.hpp"
#include "RakNetTypes.h"
#include "ChunkModStamp.h"

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
//	glm::vec2 mousePos[2];		//[0] previous frame position [1] current frame position
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

	bool operator==(const InputComponent &other) {
		return up == other.up
			&& down == other.down
			&& left == other.left
			&& right == other.right
			&& space == other.space
			&& shift == other.shift
			&& ctrl == other.ctrl;
	}

	bool operator!=(const InputComponent &other) {
		return !operator==(other);
	}
};

struct ChunkBoundryComponent : glm::i32vec3 {
	ChunkBoundryComponent(glm::i32vec3 vec) : glm::i32vec3(vec) {}
};

struct ClientChunkSnapshotComponent {
	static constexpr int VIEW_RADIUS = 3;
	static constexpr int VIEW_DIST = 2 * VIEW_RADIUS + 1;
	
	bool has_origin = false;	//right now simple bool
	Voxel::ChunkModStamp snapshot[VIEW_DIST][VIEW_DIST];
};

struct NetClientComponent {
	SLNet::RakNetGUID guid;
};

struct AFKComponent {
	float timer;
	InputComponent lastInput;
};
/*SERVER COMPONENTS*/
