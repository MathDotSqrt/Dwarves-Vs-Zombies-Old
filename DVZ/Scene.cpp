#include "Scene.h"



Scene::Scene() {
	this->materialCache = PackedFreeList<Material>(2000);
	this->meshCache = PackedFreeList<Mesh>(2000);
	this->transformationCache = PackedFreeList<Transformation>(2000);
	this->instanceCache = PackedFreeList<Instance>(2000);
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

unsigned int Scene::createInstance(unsigned int meshID, glm::vec3 pos, glm::vec3 scale) {
	Transformation newTransformation = { pos , scale};
	unsigned int transformationID = this->transformationCache.insert(newTransformation);

	Instance newInstance = { meshID, transformationID };
	unsigned int instanceID = this->instanceCache.insert(newInstance);
	return instanceID;
}
