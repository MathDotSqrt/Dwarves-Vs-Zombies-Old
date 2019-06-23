#include "MovementSystem.h"
#include "Components.h"
#include "glm.hpp"

MovementSystem::MovementSystem(int priority) : System(priority) {

}

MovementSystem::~MovementSystem() {

}

void MovementSystem::addedToEngine(entt::registry &engine) {
	engine.group<PositionComponent, VelocityComponent>();
}

void MovementSystem::removedFromEngine(entt::registry &engine) {
}

void MovementSystem::update(entt::registry &engine, float delta) {
	engine.group<PositionComponent, VelocityComponent>().each([delta](auto &pos, auto &vel) {
		pos.pos += vel.vel * delta;
	});
}
