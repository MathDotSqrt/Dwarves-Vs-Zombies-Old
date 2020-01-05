#pragma once
#include "entt.hpp"
#include "TEX.h"
#include "ModelGeometry.h"

namespace ResourceManager {
	struct TexLoader : entt::resource_loader<TexLoader, Graphics::TEX> {
		std::shared_ptr<Graphics::TEX> load(Graphics::TEX&& tex) const {
			return std::make_shared<Graphics::TEX>(std::move(tex));
		}
	};

	struct GeometryLoader : entt::resource_loader<GeometryLoader, Graphics::ModelGeometry> {
		std::shared_ptr<Graphics::ModelGeometry> load(std::string str) const {
			return std::make_shared<Graphics::ModelGeometry>(std::move(Graphics::CreateModel(str)));
		}
	};


	using TexCache = entt::resource_cache<Graphics::TEX>;
	using GeometryCache = entt::resource_cache<Graphics::ModelGeometry>;
}
