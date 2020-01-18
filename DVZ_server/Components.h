#pragma once

/*SERVER COMPONENTS*/

#include "glm.hpp"
#include "gtx/quaternion.hpp"
#include "RakNetTypes.h"
#include "ChunkModStamp.h"
#include <unordered_map>

namespace Component {
	struct Position : glm::vec3 {
		Position(glm::vec3 vec) : glm::vec3(vec) {}
	};

	struct Rotation : glm::quat {
		Rotation(glm::quat quat) : glm::quat(quat) {}
	};

	struct Velocity : glm::vec3 {
		Velocity(glm::vec3 vec) : glm::vec3(vec) {}
	};

	struct Dir {
		glm::vec3 forward;
		glm::vec3 up;
		glm::vec3 right;
	};

	struct Input {
		//	glm::vec2 mousePos[2];		//[0] previous frame position [1] current frame position
		bool up;
		bool down;
		bool left;
		bool right;
		bool space;
		bool shift;
		bool ctrl;

		Input() :
			up(false),
			down(false),
			left(false),
			right(false),
			space(false),
			shift(false),
			ctrl(false) {}

		bool operator==(const Input &other) {
			return up == other.up
				&& down == other.down
				&& left == other.left
				&& right == other.right
				&& space == other.space
				&& shift == other.shift
				&& ctrl == other.ctrl;
		}

		bool operator!=(const Input &other) {
			return !operator==(other);
		}
	};

	struct ChunkBoundry : glm::i32vec3 {
		ChunkBoundry(glm::i32vec3 vec) : glm::i32vec3(vec) {}
	};

	struct ClientChunkSnapshot {
		static constexpr int VIEW_RADIUS = 9;
		static constexpr int VIEW_DIST = 2 * VIEW_RADIUS + 1;

		static constexpr int SYNC_RADIUS = 7;
		static constexpr int SYNC_DIST = 2 * SYNC_RADIUS + 1;

		//Voxel::ChunkModStamp snapshot[VIEW_DIST][VIEW_DIST];
		std::unordered_map<int, int> chunkSnapshots;
	};

	struct NetClient {
		SLNet::RakNetGUID guid;
	};

	struct AFK {
		float timer;
		Input lastInput;
	};
}


/*SERVER COMPONENTS*/
