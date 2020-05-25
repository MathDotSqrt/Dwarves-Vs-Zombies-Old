#pragma once
#include <glm.hpp>
#include "Geometry.h"
#include "PackedFreeList.h"
#include "Material.h"
#include "VAO.h"
#include "VBO.h"
#include "Mesh.h"

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
	unsigned int sunCameraID;

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

	struct Transformation {
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

		Transformation() : position(0), rotation(0), scale(1) {}

		Transformation(glm::vec3 p, glm::vec3 r, glm::vec3 s) :
			position(p),
			rotation(r),
			scale(s) {}

	};

	struct Instance {
		MaterialID materialType;
		unsigned int materialID;
		unsigned int transformationID;
		entt::resource_handle<Mesh> meshHandle;
	};

	Util::PackedFreeList<Camera> cameraCache;
	Util::PackedFreeList<Instance> instanceCache;
	Util::PackedFreeList<Transformation> transformationCache;

	Util::PackedFreeList<ColorMaterial> colorMaterialCache;
	Util::PackedFreeList<BasicLitMaterial> basicLitMaterialCache;
	Util::PackedFreeList<TextureMaterial> textureMaterialCache;
	Util::PackedFreeList<BlockMaterial> blockMaterialCache;

	Util::PackedFreeList<PointLight> pointLightCache;

	Scene();
	~Scene();

	template<typename MATERIAL>
	unsigned int createRenderInstance(const entt::resource_handle<Mesh> handle, MATERIAL material, Transformation t = Transformation()) {
		assert(handle);
		
		auto transformationID = transformationCache.insert(t);
		auto materialID = createMaterialInstance(material);

		Instance newInstance = { MATERIAL::type, materialID, transformationID, handle };
		return instanceCache.insert(newInstance);
	}

	template<typename MATERIAL>
	unsigned int createMaterialInstance(MATERIAL &material) {
		return 0;
	}

	unsigned int createMaterialInstance(ColorMaterial &material);
	unsigned int createMaterialInstance(BasicLitMaterial &material);
	unsigned int createMaterialInstance(TextureMaterial &material);
	unsigned int createMaterialInstance(BlockMaterial &material);

	unsigned int createCameraInstance(Camera camera);

	unsigned int createPointLightInstance(const PointLight &p);
	unsigned int createPointLightInstance();

	void removeMaterialInstance(MaterialID materialType, unsigned int materialID);
	void removeTransformation(unsigned int transformationID);
	void removeRenderInstance(unsigned int instanceID);

	void setMainCamera(unsigned int cameraID);
	unsigned int getMainCameraID();

	void setSunCameraID(unsigned int cameraID);
	unsigned int getSunCameraID();
};
}
