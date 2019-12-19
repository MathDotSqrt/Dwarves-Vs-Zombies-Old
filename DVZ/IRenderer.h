#pragma once

//todo delete this pointless class
#include "IResizable.h"
#include "Scene.h"	//todo remove this include

namespace Voxel {
	class ChunkRenderDataManager;
}

namespace Graphics {
	class IRenderer  : public IResizable{
	protected:
		Scene *scene;

	public:
		virtual ~IRenderer() {}

		virtual void init(Scene *scene) = 0;
		virtual void prerender() = 0;
		virtual void render(Voxel::ChunkRenderDataManager *manager) = 0;
		virtual void postrender() = 0;
		inline Scene* getScene() {
			return this->scene;
		}
	};
}