#pragma once
#include <glm.hpp>
#include "Geometry.h"
#include "PackedFreeList.h"
#include "Material.h"
#include "VAO.h"
#include "VBO.h"

namespace Voxel {
	class Chunk;
}

namespace Graphics  {

/*
0000 0000   0000 0000   0000 0000   0000 0000

layer, transparancy type, cmd, material id, texture id
000 00 0 00000 00000
*/

class Scene {
private:
	unsigned int mainCameraID;

	//todo add reference count to mesh to delete it when no transformations are applied to it
	struct VertexBuffer {
		VBO vbo;
		unsigned int reference_count;

		VertexBuffer() : vbo(GL_ARRAY_BUFFER) {
			this->reference_count = 0;
		}
	};
public:

//annoying bug where RakPeerInterface is redefining these vaiable names
#undef near
#undef far
	struct Camera {
		glm::vec3 eye;
		glm::vec3 target;
		glm::vec3 up;

		float fov;
		float aspect;
		float near;
		float far;
	};

	struct PointLight {
		glm::vec3 position;
		float color[3];
		float intensity;
	};

	struct Mesh {
		VAO vao;
		VBO ebo;
		GLsizei indexCount;

		unsigned int vboID;

		MaterialID typeID;
		unsigned int materialInstanceID;

		Mesh() : ebo(GL_ELEMENT_ARRAY_BUFFER) {
			this->indexCount = 0;
			this->vboID = 0;
			this->typeID = MaterialID::NONE_MATERIAL_ID;
			this->materialInstanceID = 0;
		}
	};

	struct Transformation {
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
	};

	struct Instance {
		unsigned int meshID;
		unsigned int transformationID;
	};

	Util::PackedFreeList<Camera> cameraCache;
	Util::PackedFreeList<Instance> instanceCache;
	Util::PackedFreeList<Transformation> transformationCache;
	Util::PackedFreeList<Mesh> meshCache;
	Util::PackedFreeList<VertexBuffer> vertexBufferCache;

	Util::PackedFreeList<ColorMaterial> colorMaterialCache;
	Util::PackedFreeList<BasicLitMaterial> basicLitMaterialCache;
	Util::PackedFreeList<TextureMaterial> textureMaterialCache;
	Util::PackedFreeList<BlockMaterial> blockMaterialCache;

	Util::PackedFreeList<PointLight> pointLightCache;

	Scene();
	~Scene();

	template<typename VERTEX>
	unsigned int createVertexBuffer(std::vector<VERTEX> &verticies) {
		VertexBuffer vb;
		vb.vbo.bind();
		vb.vbo.bufferData(verticies, GL_STATIC_DRAW);
		vb.vbo.unbind();
		return this->vertexBufferCache.insert(std::move(vb));
	}

	template<typename VERTEX, typename MATERIAL, typename ...T>
	unsigned int createMesh(Geometry<VERTEX, T...> &model, MATERIAL &material) {
		unsigned int materialInstanceID = this->createMaterialInstance(material);
		unsigned int bufferInstanceID = this->createVertexBuffer(model.getVerticies());
		
		VertexBuffer &vb = this->vertexBufferCache[bufferInstanceID];

		Mesh newMesh;
		newMesh.typeID = MATERIAL::type;

		newMesh.vao.bind();
		newMesh.vao.bufferInterleavedData(vb.vbo, model.attribs);
		newMesh.ebo.bind();
		newMesh.ebo.bufferData(model.getIndices(), GL_STATIC_DRAW);
		newMesh.vao.unbind();
		newMesh.ebo.unbind();
		newMesh.indexCount = (GLsizei)model.getIndexCount();

		return this->meshCache.insert(std::move(newMesh));
	}


	template<typename MATERIAL>
	unsigned int createMaterialInstance(MATERIAL &material) {
		return 0;
	}

	unsigned int createMaterialInstance(ColorMaterial &material);
	unsigned int createMaterialInstance(BasicLitMaterial &material);
	unsigned int createMaterialInstance(TextureMaterial &material);
	unsigned int createMaterialInstance(BlockMaterial &material);

	unsigned int createRenderInstance(unsigned int meshID, Transformation t);
	unsigned int createRenderInstance(unsigned int meshID, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	unsigned int createRenderInstance(unsigned int meshID);
	
	unsigned int createCameraInstance(Camera camera);

	unsigned int createPointLightInstance(PointLight &p);
	unsigned int createPointLightInstance();

	void removeMaterialInstance(MaterialID materialType, unsigned int materialID);
	void removeMesh(unsigned int meshID);
	void removeTransformation(unsigned int transformationID);
	void removeRenderInstance(unsigned int instanceID);

	void setMainCamera(unsigned int cameraID);
	unsigned int getMainCameraID();
};
}
