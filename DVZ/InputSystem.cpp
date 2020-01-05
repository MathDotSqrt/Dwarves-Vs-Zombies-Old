#include "InputSystem.h"
#include "Engine.h"
#include "Components.h"
#include "Window.h"
#include "macrologger.h"
InputSystem::InputSystem(int priority) : System(priority) {
}


InputSystem::~InputSystem() {
}

void InputSystem::addedToEngine(Engine * engine) {
	LOG_SYSTEM("Added to engine");

	engine->group<InputComponent>(entt::get<DirComponent, RotationComponent, VelocityComponent>);
}

void InputSystem::removedFromEngine(Engine * engine) {
	LOG_SYSTEM("Removed from engine");
}

void InputSystem::update(Engine * engine, float deltaTime) {

	
}
