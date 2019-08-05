#pragma once
#include <glm.hpp>
#include "Geometry.h"
#include "PackedFreeList.h"

namespace Graphics  {

/*
0000 0000   0000 0000   0000 0000   0000 0000

layer, transparancy type, cmd, material id, texture id
000 00 0 00000 00000
*/


typedef enum _MaterialID: unsigned char {
	NONE_MATERIAL_ID = 0,
	COLOR_MATERIAL_ID,
	NORMAL_MAERIAL_ID,
	BASIC_LIT_MATERIAL_ID,
	TEXTURE_MATERIAL_ID,
	BLOCK_MATERIAL_ID
} MaterialID;

struct ColorMaterial {
	static MaterialID type;
	float color[3];
};

struct NormalMaterial {
	static MaterialID type;
};

struct BasicLitMaterial {
	static MaterialID type;

	float diffuseColor[3];
	float specularColor[3];
	float shinyness;
};

struct TextureMaterial {
	static MaterialID type;
	GLuint texID;
};

struct BlockMaterial {
	static MaterialID type;
	float specularColor[3];
	float shinyness;
};

struct Mesh {
	Geometry model;
	MaterialID typeID;
	unsigned int materialInstanceID;
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

struct PointLight {
	glm::vec3 position;
	float color[3];
	float intensity;
};

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




class Scene {
private:
	unsigned int mainCameraID;

public:
	Util::PackedFreeList<Camera> cameraCache;
	Util::PackedFreeList<Instance> instanceCache;
	Util::PackedFreeList<Transformation> transformationCache;
	Util::PackedFreeList<Mesh> meshCache;
	
	Util::PackedFreeList<ColorMaterial> colorMaterialCache;
	Util::PackedFreeList<BasicLitMaterial> basicLitMaterialCache;
	Util::PackedFreeList<TextureMaterial> textureMaterialCache;
	Util::PackedFreeList<BlockMaterial> blockMaterialCache;


	Util::PackedFreeList<PointLight> pointLightCache;

	Scene();
	~Scene();

	template<typename MATERIAL>
	unsigned int createMesh(Geometry &model, MATERIAL &material) {
		unsigned int materialInstanceID = this->createMaterialInstance(material);
		Mesh newMesh = { model, MATERIAL::type, materialInstanceID };
		unsigned int newMeshID = this->meshCache.insert(newMesh);
		return newMeshID;
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

	void setMainCamera(unsigned int cameraID);
	unsigned int getMainCameraID();


};
}
