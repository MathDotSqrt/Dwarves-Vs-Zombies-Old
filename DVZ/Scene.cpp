#include "Scene.h"

using namespace Graphics;

Scene::Scene() : 
	cameraCache(10),
	instanceCache(5000),
	transformationCache(5000),
	meshCache(5000),
	colorMaterialCache(5000),
	basicLitMaterialCache(5000),
	blockMaterialCache(500),
	pointLightCache(4)
	{


}


Scene::~Scene() {

}

unsigned int Scene::createMaterialInstance(ColorMaterial &material) {
	return this->colorMaterialCache.insert(material);
}

unsigned int Scene::createMaterialInstance(BasicLitMaterial &material) {
	return this->basicLitMaterialCache.insert(material);
}

unsigned int Scene::createMaterialInstance(TextureMaterial &material) {
	return this->textureMaterialCache.insert(material);
}

unsigned int Scene::createMaterialInstance(BlockMaterial &material) {
	return this->blockMaterialCache.insert(material);
}

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

unsigned int Scene::createPointLightInstance(PointLight &p) {
	return this->pointLightCache.insert(p);
}

unsigned int Scene::createPointLightInstance() {
	PointLight p;
	return this->pointLightCache.insert(p);
}

void Scene::setMainCamera(unsigned int cameraID) {
	this->mainCameraID = cameraID;
}

unsigned int Scene::getMainCameraID() {
	return this->mainCameraID;
}