#include "Scene.h"

using namespace Graphics;

//MaterialID ColorMaterial::type;
//MaterialID NormalMaterial::type;
//MaterialID TextureMaterial::type;

Scene::Scene() {
	this->colorMaterialCache = Util::PackedFreeList<ColorMaterial>(5000);
	this->meshCache = Util::PackedFreeList<Mesh>(5000);
	this->transformationCache = Util::PackedFreeList<Transformation>(5000);
	this->instanceCache = Util::PackedFreeList<Instance>(5000);
	this->cameraCache = Util::PackedFreeList<Camera>(10);
}


Scene::~Scene() {

}

unsigned int Scene::createBasicMesh(Geometry &model, float r, float g, float b) {
	ColorMaterial c;
	c.color[0] = r;
	c.color[1] = g;
	c.color[2] = b;

	unsigned int materialInstanceID = this->colorMaterialCache.insert(c);

	Mesh newMesh = { model, ColorMaterial::type, materialInstanceID };
	unsigned int newMeshID = this->meshCache.insert(newMesh);
	return newMeshID;
}

unsigned int Scene::createNormalMesh(Geometry &model) {
	Mesh newMesh = { model, NormalMaterial::type, 0 };
	unsigned int newMeshID = this->meshCache.insert(newMesh);
	return newMeshID;
};

unsigned int Scene::createRenderInstance(unsigned int meshID, Transformation t) {
	unsigned int transformationID = this->transformationCache.insert(t);
	Instance newInstance = {meshID, transformationID };
	unsigned int instanceID = this->instanceCache.insert(newInstance);
	return instanceID;
}

unsigned int Scene::createRenderInstance(unsigned int meshID, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
	Transformation newTransformation = { position, rotation, scale };
	return this->createRenderInstance(meshID, newTransformation);
}

unsigned int Scene::createRenderInstance(unsigned int meshID) {
	glm::vec3 pos(0, 0, 0);
	glm::vec3 rot(0, 0, 0);
	glm::vec3 scale(1, 1, 1);
	return this->createRenderInstance(meshID, pos, rot, scale);
}

unsigned int Scene::createCameraInstance(Camera camera) {
	return this->cameraCache.insert(camera);
}

void Scene::setMainCamera(unsigned int cameraID) {
	this->mainCameraID = cameraID;
}

unsigned int Scene::getMainCameraID() {
	return this->mainCameraID;
}