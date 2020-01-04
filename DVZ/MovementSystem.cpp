#include "MovementSystem.h"
#include "macrologger.h"

#include "Engine.h"
#include "Components.h"
#include "glm.hpp"

MovementSystem::MovementSystem(int priority) : System(priority) {

}

MovementSystem::~MovementSystem() {

}

void MovementSystem::addedToEngine(Engine * engine) {
	LOG_SYSTEM("Added to engine");

	engine->group<PositionComponent, VelocityComponent>();
	engine->group<RotationComponent, RotationalVelocityComponent>();
}

void MovementSystem::removedFromEngine(Engine * engine) {
	LOG_SYSTEM("Removed from engine");
}

void MovementSystem::update(Engine * engine, float delta) {
	engine->group<PositionComponent, VelocityComponent>().each([delta](auto &pos, auto &vel) {
		pos.pos += vel.vel * delta;
	});
}
