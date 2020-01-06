#pragma once
#include "entt.hpp"
#include "Geometry.h"
#include "ModelGeometry.h"
#include "VAO.h"
#include "VBO.h"

namespace Graphics {
	struct Mesh {
		VAO vao;
		VBO vbo;
		VBO ebo;
		GLsizei index_count;

		template<typename ...T>
		Mesh(const Graphics::Geometry<T...> &geometry) : vbo(GL_ARRAY_BUFFER), ebo(GL_ELEMENT_ARRAY_BUFFER){
			vao.bind();
			vbo.bind();
			vbo.bufferData(geometry.getVerticies(), GL_STATIC_DRAW);
			vao.bufferInterleavedData(geometry.attribs);
			vbo.unbind();
			ebo.bind();
			ebo.bufferData(geometry.getIndices(), GL_STATIC_DRAW);
			vao.unbind();
			ebo.unbind();

			index_count = (GLsizei)geometry.getIndexCount();
		}
	};
}

namespace ResourceManager {
	struct MeshLoader : entt::resource_loader<MeshLoader, Graphics::Mesh> {
		template<typename ...T>
		std::shared_ptr<Graphics::Mesh> load(const Graphics::Geometry<T...> &geometry) const {
			return std::make_shared<Graphics::Mesh>(geometry);
		}

		std::shared_ptr<Graphics::Mesh> load(std::string filename) const {
			return std::make_shared<Graphics::Mesh>(Graphics::CreateModel(filename));
		}
	};

	using MeshCache = entt::resource_cache<Graphics::Mesh>;
}