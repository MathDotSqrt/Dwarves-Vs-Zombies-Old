#pragma once
#include <glm.hpp>
#include "Geometry.h"
#include "PackedFreeList.h"

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

struct Camera {
	glm::vec3 position;
	glm::vec3 rotation;
	unsigned int viewMatrixID;
};

class Scene {

public:
	PackedFreeList<Material> materialCache;
	PackedFreeList<Mesh> meshCache;
	PackedFreeList<Transformation> transformationCache;
	PackedFreeList<Instance> instanceCache;

	Camera mainCamera;

	Scene();
	~Scene();



	unsigned int createBasicMesh(Geometry model, float r, float g, float b);
	unsigned int createInstance(unsigned int meshID, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
};

