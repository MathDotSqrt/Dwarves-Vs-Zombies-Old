#include "MovementSystem.h"
#include "Components.h"
#include "glm.hpp"

MovementSystem::MovementSystem(int priority) : System(priority) {

}

MovementSystem::~MovementSystem() {

}

void MovementSystem::addedToEngine(entt::registry &engine) {
	engine.group<PositionComponent, RotationComponent, VelocityComponent>();
}

void MovementSystem::removedFromEngine(entt::registry &engine) {
}

void MovementSystem::update(entt::registry &engine, float delta) {
	engine.group<PositionComponent, RotationComponent, VelocityComponent>().each([delta](auto &pos, auto &rot, auto &vel) {
		pos.pos += vel.vel * delta;

		rot.rot += vel.vel * delta;
	});
}
