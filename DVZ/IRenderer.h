#pragma once
#include "IResizable.h"
#include "Scene.h"

namespace Graphics {
	class IRenderer  : public IResizable{
	protected:
		Scene *scene;

	public:
		virtual ~IRenderer() {}

		virtual void init(Scene *scene) = 0;
		virtual void prerender() = 0;
		virtual void render() = 0;
	};
}