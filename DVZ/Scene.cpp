#include "Scene.h"

Scene::Scene() {
	this->materialCache = PackedFreeList<Material>(500);
	this->meshCache = PackedFreeList<Mesh>(500);
	this->transformationCache = PackedFreeList<Transformation>(500);
	this->instanceCache = PackedFreeList<Instance>(500);
}


Scene::~Scene() {
}

unsigned int Scene::createBasicMesh(Geometry model, float r, float g, float b) {
	Material newMaterial;
	newMaterial.color[0] = r;
	newMaterial.color[1] = g;
	newMaterial.color[2] = b;

	unsigned int newMaterialID = this->materialCache.insert(newMaterial);

	Mesh newMesh = { model, newMaterialID };

	unsigned int newMeshID = this->meshCache.insert(newMesh);

	return newMeshID;
}

unsigned int Scene::createInstance(unsigned int meshID, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
	Transformation newTransformation = { position, rotation, scale };
	unsigned int transformationID = this->transformationCache.insert(newTransformation);

	Instance newInstance = { meshID, transformationID };
	unsigned int instanceID = this->instanceCache.insert(newInstance);
	return instanceID;
}

