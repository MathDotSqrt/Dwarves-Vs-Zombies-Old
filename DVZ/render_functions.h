#pragma once
#include "glm.hpp"
#include <gtx/transform.hpp>
#include <gtx/quaternion.hpp>
#include "RenderStateKey.h"
#include <vector>

namespace Voxel {
	class ChunkRenderDataManager;
}

namespace Graphics {
	
	class Scene;
	class DepthFBO;

	namespace ShaderVariables {
		constexpr glm::mat4 identity = glm::identity<glm::mat4>();
		
		extern float shader_time;

		extern glm::vec3 camera_pos;
		
		extern glm::mat4 p;
		extern glm::mat4 v;
		extern glm::mat4 vp;

		extern glm::mat4 sunVP;
	}

	typedef std::vector<RenderStateKey>::const_iterator iterator;

	iterator render_basic(Scene *scene, iterator start, iterator end);
	iterator render_normal(Scene *scene, iterator start, iterator end);
	iterator render_basic_lit(Scene *scene, iterator start, iterator end);
	iterator render_chunks(Voxel::ChunkRenderDataManager *manager, Scene *scene, DepthFBO *fbo, iterator start, iterator end);
	iterator render_chunks_shadow(Voxel::ChunkRenderDataManager *manager, Scene *scene, iterator start, iterator end);
	


}