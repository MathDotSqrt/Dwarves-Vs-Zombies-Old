#pragma once
#include "glm.hpp"

namespace Voxel {
	class ChunkRenderDataManager;
}

namespace Graphics {
	
	class Scene;

	namespace ShaderVariables {
		glm::vec3 camera_pos;
		
		glm::mat4 p;
		glm::mat4 v;
		glm::mat4 vp;
	}

	int renderBasic(int startIndex);
	int renderNormal(int startIndex);
	int renderBasicLit(int startIndex);
	int renderChunks(int startIndex);


}