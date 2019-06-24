#include "BasicRenderSystem.h"
#include "VAO.h"
#include "VBO.h"
#include "Shader.h"
#include "Components.h"
#include "macrologger.h"
#include "glm.hpp"
#include <gtx/transform.hpp>
#include <gtx/quaternion.hpp>

BasicRenderSystem::BasicRenderSystem(Scene *scene, int priority) : System(priority) {
	this->scene = scene;
}


BasicRenderSystem::~BasicRenderSystem()
{
}

void BasicRenderSystem::addedToEngine(entt::registry &engine) {
	engine.group<RenderInstanceComponent>(entt::get<PositionComponent, RotationComponent, ScaleComponent>);
}

void BasicRenderSystem::removedFromEngine(entt::registry &engine) {
}

void BasicRenderSystem::update(entt::registry &engine, float delta) {
	engine.group<RenderInstanceComponent>(entt::get<PositionComponent, RotationComponent, ScaleComponent>)
		.each([this](auto &instanceComponent, auto &positionComponent, auto &rotationComponent, auto &scaleComponent)
	{
		auto transformationID = this->scene->instanceCache[instanceComponent.instanceID].transformationID;
		auto *transformation = &this->scene->transformationCache[transformationID];
		

		glm::mat4 matrix = glm::identity<glm::mat4>();

		transformation->position = positionComponent.pos;
		transformation->rotation = rotationComponent.rot;
		transformation->scale = scaleComponent.scale;
	});
	
}
