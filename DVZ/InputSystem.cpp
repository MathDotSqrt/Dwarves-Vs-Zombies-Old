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
	const float SPEED = 9.0f;
	const float FLY = 3.0f;
	const float TURN_SPEED = .5f;
	const float FAST_SPEED = 88.0f;

	static double mX = 0;
	static double mY = 0;
	struct {
		float forward = 0;
		float right = 0;
		float turn = 0;
		float up = 0;
		float ctrl = 0;
		double dx = 0;
		double dy = 0;
	} userInput;

	double cX, cY;
	if (Window::getMousePos(cX, cY)) {
		userInput.dx = cX - mX;
		userInput.dy = cY - mY;
		mX = cX;
		mY = cY;
	}

	userInput.forward += Window::isPressed('W') ? 1 : 0;
	userInput.forward -= Window::isPressed('S') ? 1 : 0;
	userInput.right += Window::isPressed('D') ? 1 : 0;
	userInput.right -= Window::isPressed('A') ? 1 : 0;
	userInput.turn += Window::isPressed(Window::LEFT) ? 1 : 0;
	userInput.turn -= Window::isPressed(Window::RIGHT) ? 1 : 0;
	userInput.up += Window::isPressed(Window::SPACE) ? 1 : 0;
	userInput.up -= Window::isPressed(Window::LSHIFT) ? 1 : 0;
	userInput.ctrl += Window::isPressed(Window::LCTRL) ? 1 : 0;

	engine->group<InputComponent>(entt::get<DirComponent, RotationComponent, VelocityComponent, RotationalVelocityComponent>)
		.each([deltaTime, SPEED, TURN_SPEED, FAST_SPEED, userInput] (auto &input, auto &dir, auto &rot, auto &vel, auto &rotVel){
		
		glm::vec3 userForward = dir.forward * userInput.forward * (userInput.ctrl ? FAST_SPEED : SPEED);
		glm::vec3 userRight = dir.right * userInput.right * (userInput.ctrl ? FAST_SPEED : SPEED);

		glm::vec3 newForward = rot.rot * userForward;
		glm::vec3 newRight = rot.rot * userRight;

		vel.vel = newForward + newRight;
		vel.vel.y += userInput.up * (userInput.ctrl ? FAST_SPEED : SPEED);

		glm::quat qYaw = glm::angleAxis((float)-userInput.dx * TURN_SPEED / 100.0f, (glm::vec3)dir.up);
		glm::quat qPitch = glm::angleAxis((float)-userInput.dy * TURN_SPEED / 100.0f, (glm::vec3)dir.right);
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
