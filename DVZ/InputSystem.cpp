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

	engine->group<InputComponent>(entt::get<DirComponent, RotationComponent, VelocityComponent, RotationalVelocityComponent>);
}

void InputSystem::removedFromEngine(Engine * engine) {
	LOG_SYSTEM("Removed from engine");
}

void InputSystem::update(Engine * engine, float deltaTime) {

	engine->view<InputComponent>().each([](auto &input) {
		input.mousePos[0] = input.mousePos[1];

		double mx = 0;
		double my = 0;
		Window::getMousePos(mx, my);
		input.mousePos[1] = glm::vec2(mx, my);
		
		input.up = Window::isPressed('W');
		input.down = Window::isPressed('S');
		input.left = Window::isPressed('A');
		input.right = Window::isPressed('D');
		input.space = Window::isPressed(Window::SPACE);
		input.shift = Window::isPressed(Window::LSHIFT);
		input.ctrl = Window::isPressed(Window::LCTRL);
		
	});

	engine->group<InputComponent>(entt::get<DirComponent, RotationComponent, VelocityComponent, RotationalVelocityComponent>)
		.each([deltaTime] (auto &input, auto &dir, auto &rot, auto &vel, auto &rotVel){
		
		const float SPEED = 9.0f;
		const float TURN_SPEED = .5f;
		const float FAST_SPEED = 88.0f;

		float forward = (float)input.up - (float)input.down;
		float right = (float)input.right - (float)input.left;
		float up = (float)input.space - (float)input.shift;
		glm::vec2 mouseDelta = input.mousePos[1] - input.mousePos[0];


		glm::vec3 userForward = dir.forward * forward * (input.ctrl ? FAST_SPEED : SPEED);
		glm::vec3 userRight = dir.right * right * (input.ctrl ? FAST_SPEED : SPEED);

		glm::vec3 newForward = rot.rot * userForward;
		glm::vec3 newRight = rot.rot * userRight;

		vel.vel = newForward + newRight;
		vel.vel.y += up * (input.ctrl ? FAST_SPEED : SPEED);

		glm::quat qYaw = glm::angleAxis((float)-mouseDelta.x * TURN_SPEED / 100.0f, (glm::vec3)dir.up);
		glm::quat qPitch = glm::angleAxis((float)-mouseDelta.y * TURN_SPEED / 100.0f, (glm::vec3)dir.right);
		rot.rot = (qYaw * (rot.rot)) * qPitch;

	});

	entt::entity player =  engine->getPlayer();
	if (Window::isPressed('T')) {
		//engine->replace<PositionComponent>(player, glm::vec3(0, 4, 0));
		PositionComponent &comp = engine->get<PositionComponent>(player);
		comp.pos.x = 0;
		comp.pos.z = 0;
	}
}
