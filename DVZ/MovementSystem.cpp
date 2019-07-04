#include "MovementSystem.h"
#include "Engine.h"
#include "Components.h"
#include "glm.hpp"

MovementSystem::MovementSystem(int priority) : System(priority) {

}

MovementSystem::~MovementSystem() {

}

void MovementSystem::addedToEngine(Engine * engine) {
	engine->group<PositionComponent, VelocityComponent>();
	engine->group<RotationComponent, RotationalVelocityComponent>();
}

void MovementSystem::removedFromEngine(Engine * engine) {
}

void MovementSystem::update(Engine * engine, float delta) {
	engine->group<PositionComponent, VelocityComponent>().each([delta](auto &pos, auto &vel) {
		pos.pos += vel.vel * delta;
	});

	engine->group<RotationComponent, RotationalVelocityComponent>().each([delta] (auto &rot, auto &rotVel){
		rot.rot = glm::quat(rotVel.eular * delta) * rot.rot;
	});
}
