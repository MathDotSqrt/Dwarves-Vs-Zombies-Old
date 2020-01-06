#include "Scene.h"

using namespace Graphics;

Scene::Scene() :
	cameraCache(10),
	instanceCache(5000),
	transformationCache(5000),
	colorMaterialCache(5000),
	basicLitMaterialCache(5000),
	blockMaterialCache(5000),
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

void Scene::removeMaterialInstance(MaterialID typeID, unsigned int materialInstanceID) {
	switch (typeID) {
	case MaterialID::COLOR_MATERIAL_ID:
		this->colorMaterialCache.erase(materialInstanceID);
		break;
	case MaterialID::BASIC_LIT_MATERIAL_ID:
		this->basicLitMaterialCache.erase(materialInstanceID);
		break;
	case MaterialID::TEXTURE_MATERIAL_ID:
		this->textureMaterialCache.erase(materialInstanceID);
		break;
	case MaterialID::BLOCK_MATERIAL_ID:
		this->blockMaterialCache.erase(materialInstanceID);
		break;
	default:
		break;
	}
}


void Scene::removeTransformation(unsigned int transformationID) {
	this->transformationCache.erase(transformationID);
}

void Scene::removeRenderInstance(unsigned int instanceID) {
	Instance &i = this->instanceCache[instanceID];
	
	removeTransformation(i.transformationID);
	removeMaterialInstance(i.materialType, i.materialID);
	instanceCache.erase(instanceID);
}

void Scene::setMainCamera(unsigned int cameraID) {
	this->mainCameraID = cameraID;
}

unsigned int Scene::getMainCameraID() {
	return this->mainCameraID;
}

void Scene::setSunCameraID(unsigned int cameraID) {
	sunCameraID = cameraID;
}

unsigned int Scene::getSunCameraID() {
	return sunCameraID;
}