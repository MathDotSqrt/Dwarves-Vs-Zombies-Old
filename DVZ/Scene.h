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
	TEXTURE_MATERIAL_ID
} MaterialID;

struct ColorMaterial {
	static MaterialID type;
	float color[3];
};

struct NormalMaterial {
	static MaterialID type;
};

struct TextureMaterial {
	static MaterialID type;
	GLuint texID;
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
	Util::PackedFreeList<ColorMaterial> colorMaterialCache;
	Util::PackedFreeList<NormalMaterial> normalMaterialCache;
	Util::PackedFreeList<TextureMaterial> textureMaterialCache;
	
	Util::PackedFreeList<Transformation> transformationCache;

	Util::PackedFreeList<Mesh> meshCache;
	Util::PackedFreeList<Instance> instanceCache;


	Util::PackedFreeList<Camera> cameraCache;
	

	Scene();
	~Scene();

	unsigned int createBasicMesh(Geometry &model, float r, float g, float b);
	unsigned int createNormalMesh(Geometry &model);
	unsigned int createRenderInstance(unsigned int meshID, Transformation t);
	unsigned int createRenderInstance(unsigned int meshID, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	unsigned int createRenderInstance(unsigned int meshID);
	unsigned int createCameraInstance(Camera camera);

	void setMainCamera(unsigned int cameraID);
	unsigned int getMainCameraID();
};
}
