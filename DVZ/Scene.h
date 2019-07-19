#pragma once
#include <glm.hpp>
#include "Geometry.h"
#include "PackedFreeList.h"

namespace Graphics  {

struct Material {
	float color[3];
};

struct Mesh {
	Geometry model;
	unsigned int materialID;
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
	Util::PackedFreeList<Material> materialCache;
	Util::PackedFreeList<Mesh> meshCache;
	Util::PackedFreeList<Transformation> transformationCache;
	Util::PackedFreeList<Instance> instanceCache;
	Util::PackedFreeList<Camera> cameraCache;
	

	Scene();
	~Scene();

	unsigned int createBasicMesh(Geometry model, float r, float g, float b);
	unsigned int createRenderInstance(unsigned int meshID, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	unsigned int createCameraInstance(Camera camera);

	void setMainCamera(unsigned int cameraID);
	unsigned int getMainCameraID();
};
}
