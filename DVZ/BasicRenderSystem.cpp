#include "BasicRenderSystem.h"
#include "VAO.h"
#include "VBO.h"
#include "Shader.h"
#include "Components.h"
#include "macrologger.h"

BasicRenderSystem::BasicRenderSystem(Scene *scene, int priority) : System(priority) {
	this->scene = scene;
}


BasicRenderSystem::~BasicRenderSystem()
{
}

void BasicRenderSystem::addedToEngine(entt::registry &engine) {
	engine.group<RenderInstanceComponent>(entt::get<PositionComponent, ScaleComponent>);
}

void BasicRenderSystem::removedFromEngine(entt::registry &engine) {
}

void BasicRenderSystem::update(entt::registry &engine, float delta) {
	engine.group<RenderInstanceComponent>(entt::get<PositionComponent, ScaleComponent>)
		.each([this](auto &instanceComponent, auto &positionComponent, auto &scaleComponent)
	{
		auto transformationID = this->scene->instanceCache[instanceComponent.instanceID].transformationID;
		auto *transformation = &this->scene->transformationCache[transformationID];
		transformation->position = positionComponent.pos;
		transformation->scale = scaleComponent.scale;
	});
	
}
