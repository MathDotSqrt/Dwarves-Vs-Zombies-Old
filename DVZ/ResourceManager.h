#pragma once
#include "entt.hpp"
#include "TEX.h"
#include "ModelGeometry.h"

namespace ResourceManager {
	struct TexLoader : entt::resource_loader<TexLoader, Graphics::TEX> {
		std::shared_ptr<Graphics::TEX> load(Graphics::TEX&& tex) {
			return std::make_shared<Graphics::TEX>(std::move(tex));
		}
	};

	using namespace Graphics;
	struct GeometryLoader : entt::resource_loader<GeometryLoader, Graphics::ModelGeometry> {
		std::shared_ptr<Graphics::ModelGeometry> load(std::string str) {
			return std::make_shared<Graphics::ModelGeometry>(Graphics::CreateModel(str));
		}
	};


	using TexCache = entt::resource_cache<Graphics::TEX>;
	using GeometryCache = entt::resource_cache<Graphics::ModelGeometry>;
}
