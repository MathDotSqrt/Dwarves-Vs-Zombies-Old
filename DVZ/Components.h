#pragma once
#include "SharedComponents.h"
#include "PrototypeID.h"
#include "Block.h"
#include "AABB.h"

namespace Component {

	struct Input {
		glm::vec2 mousePos[2];		//[0] previous frame position [1] current frame position

		bool up;
		bool down;
		bool left;
		bool right;
		bool space;
		bool shift;
		bool ctrl;
	};

	struct RenderInstance {
		uint32 instanceID;
	};

	struct PointLight {
		uint32 lightInstanceID;
		glm::vec3 color;
		float intensity;
	};

	struct CameraInstance {
		uint32 cameraID;
	};

	struct Network {
		uint32 netID;
	};

	struct Prototype {
		PrototypeID ID;
	};

}

