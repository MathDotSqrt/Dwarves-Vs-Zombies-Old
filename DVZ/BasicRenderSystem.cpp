#include "BasicRenderSystem.h"
#include "VAO.h"
#include "VBO.h"
#include "Shader.h"
#include "Components.h"
#include "macrologger.h"
#include "glm.hpp"
#include <gtx/transform.hpp>
#include <gtx/quaternion.hpp>

using namespace glm;

BasicRenderSystem::BasicRenderSystem(Graphics::Scene *scene, int priority) : System(priority) {
	this->scene = scene;
}


BasicRenderSystem::~BasicRenderSystem()
{
}

void BasicRenderSystem::addedToEngine(entt::registry &engine) {
	engine.group<RenderInstanceComponent>(entt::get<PositionComponent, RotationComponent, ScaleComponent>);
	engine.group<CameraInstanceComponent>(entt::get<PositionComponent, RotationComponent, DirComponent>);
}

void BasicRenderSystem::removedFromEngine(entt::registry &engine) {
}

void BasicRenderSystem::update(entt::registry &engine, float delta) {
	engine.group<RenderInstanceComponent>(entt::get<PositionComponent, RotationComponent, ScaleComponent>)
		.each([this](auto &instanceComponent, auto &positionComponent, auto &rotationComponent, auto &scaleComponent)
	{
		auto transformationID = this->scene->instanceCache[instanceComponent.instanceID].transformationID;
		auto *transformation = &this->scene->transformationCache[transformationID];

		vec3 a = glm::eulerAngles(rotationComponent.rot);
		//LOG_INFO("%f %f %f", a.x, a.y, a.z);

		transformation->position = positionComponent.pos;
		transformation->rotation = glm::eulerAngles(rotationComponent.rot);
		transformation->scale = scaleComponent.scale;
	});

	engine.group<CameraInstanceComponent>(entt::get<PositionComponent, RotationComponent, DirComponent>)
		.each([this](auto &cameraComponent, auto &positionComponent, auto &rotationComponent, auto &dirComponent) 
	{
		auto *camera = &this->scene->cameraCache[cameraComponent.cameraID];

		glm::quat orientation = rotationComponent.rot;

		camera->eye = positionComponent.pos;
		camera->target = orientation * dirComponent.forward;
		camera->up = orientation * dirComponent.up;
	});
	
}
