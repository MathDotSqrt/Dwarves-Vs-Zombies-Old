#include "BasicRenderSystem.h"
#include "Engine.h"
#include "VAO.h"
#include "VBO.h"
#include "Shader.h"
#include "Components.h"
#include "macrologger.h"
#include "glm.hpp"
#include <gtx/transform.hpp>
#include <gtx/quaternion.hpp>

using namespace glm;

BasicRenderSystem::BasicRenderSystem(int priority) : System(priority) {
}


BasicRenderSystem::~BasicRenderSystem()
{
}

void BasicRenderSystem::addedToEngine(Engine * engine) {
	LOG_SYSTEM("Added to engine");
	engine->group<RenderInstanceComponent>(entt::get<PositionComponent, RotationComponent, ScaleComponent>);
	engine->group<CameraInstanceComponent>(entt::get<PositionComponent, RotationComponent, DirComponent>);
}

void BasicRenderSystem::removedFromEngine(Engine * engine) {
	LOG_SYSTEM("Removed from engine");
}

void BasicRenderSystem::update(Engine * engine, float delta) {
	engine->group<RenderInstanceComponent>(entt::get<PositionComponent, RotationComponent, ScaleComponent>)
		.each([engine](auto &instanceComponent, auto &positionComponent, auto &rotationComponent, auto &scaleComponent)
	{
		Graphics::Scene *scene = engine->getScene();
		auto transformationID = scene->instanceCache[instanceComponent.instanceID].transformationID;
		auto *transformation = &scene->transformationCache[transformationID];

		vec3 a = glm::eulerAngles(rotationComponent.rot);
		//LOG_INFO("%f %f %f", a.x, a.y, a.z);

		transformation->position = positionComponent.pos;
		transformation->rotation = glm::eulerAngles(rotationComponent.rot);
		transformation->scale = scaleComponent.scale;
	});

	engine->group<CameraInstanceComponent>(entt::get<PositionComponent, RotationComponent, DirComponent>)
		.each([engine](auto &cameraComponent, auto &positionComponent, auto &rotationComponent, auto &dirComponent) 
	{
		Graphics::Scene *scene = engine->getScene();
		auto *camera = &scene->cameraCache[cameraComponent.cameraID];

		glm::quat orientation = rotationComponent.rot;

		camera->eye = positionComponent.pos;
		camera->target = orientation * dirComponent.forward;
		camera->up = orientation * dirComponent.up;
	});
	
}
