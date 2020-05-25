#include "GameSystems.h"
#include "Engine.h"
#include "Components.h"
#include "Window.h"
using namespace System;

void System::movement_system(Engine &engine, float delta) {
	using namespace Component;

	engine.view<Velocity, Acceleration>().each([delta](auto &vel, auto &accel) {
		accel += glm::vec3(0, -35.0f * 4, 0);
		vel += accel * delta * .5f;
	});

	System::voxel_collision_system(engine, delta);

	engine.group<Position, Velocity>().each([delta](auto &pos, auto &vel) {
		pos += delta * vel;
	});

	engine.view<Velocity, Acceleration>().each([delta](glm::vec3 &vel, auto &accel) {
		accel = glm::vec3();
		vel += accel * delta * .5f;

		/*constexpr float MAX_SPEED = 100.0f;
		const auto current_speed = glm::length(vel);
		const auto clamped_speed = glm::min(current_speed, MAX_SPEED);
		vel = glm::normalize(vel) * clamped_speed;*/
	});
}

void System::friction_system(Engine &engine, float delta) {
	using namespace Component;

	auto view = engine.view<Player, Velocity, Acceleration>();
	view.each([delta](auto &player, glm::vec3 &vel, auto &accel) {
		constexpr glm::vec3 FRIC(.6f, 1, .6f);
		constexpr glm::vec3 AIR_FRIC(0);

		//accel -= (player.is_grounded ? FRIC : AIR_FRIC) * vel;
		vel *= FRIC;
	});
}


void System::input_system(Engine &engine, float delta) {
	using namespace Component;



	engine.view<Input>().each([](auto &input) {
		input.mousePos[0] = input.mousePos[1];

		//todo remove
		if (Window::isFocused() && Window::isPressed('r') == false) {
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
		}

	});
	engine.group<Input>(entt::get<Dir, Rotation, Velocity, Acceleration, Player>)
		.each([delta](auto &input, auto &dir, auto &rot, auto &vel, auto &accel, auto &player) {

		/*CONSTANTS*/
		//const float SPEED = 8.0f * 60;
		//const float RIGHT_SPEED = 6.0f * 60;
		//const float FAST_SPEED = 13.0f * 60;

		//const float JUMP_FORCE = 10.0f * 60;
		//const float TURN_SPEED = .5f;
		//const float LOOK_DOWN_CONSTANT = .01f;
		const float SPEED = 8.0f * 30 * 2;
		const float RIGHT_SPEED = 5.0f * 30 * 2;
		const float FAST_SPEED = 13.0f * 30 * 2;
		const float AIR_SPEED = .8f;

		const float JUMP_FORCE = 35 * 10 * 5.5;
		const float TURN_SPEED = .5f;
		const float LOOK_DOWN_CONSTANT = .01f;
		/*CONSTANTS*/

		auto remove_pitch_rot = [](const glm::quat &rot) {
			const float yaw = glm::yaw(rot);
			const float roll = glm::roll(rot);
			return glm::quat(glm::vec3(roll, yaw, roll));
		};


		/*Movement scalars*/
		const float forward = (float)input.up - (float)input.down;
		const float right = (float)input.right - (float)input.left;
		const float up = (float)input.space - (float)input.shift;
		const glm::vec2 mouse_delta = input.mousePos[1] - input.mousePos[0];
		/*Movement scalars*/

		/*Movement input vectors/quaternions*/
		const glm::vec3 user_forward = dir.forward * forward;
		const glm::vec3 user_right = dir.right * right;

		const glm::quat q_yaw = glm::angleAxis((float)-mouse_delta.x * TURN_SPEED / 100.0f, (glm::vec3)dir.up);
		const glm::quat q_pitch = glm::angleAxis((float)-mouse_delta.y * TURN_SPEED / 100.0f, (glm::vec3)dir.right);
		/*Movement input vectors/quaternions*/

		/*Camera rotation code*/
		glm::quat new_rot = (q_yaw * (rot)) * q_pitch;
		glm::quat move_dir = remove_pitch_rot(new_rot);		//rotation quaternion without pitch
		if (glm::dot(new_rot * dir.forward, move_dir * dir.forward) < LOOK_DOWN_CONSTANT) {
			new_rot = q_yaw * rot;
			move_dir = remove_pitch_rot(new_rot);
		}

		rot = new_rot;
		/*Camera rotation code*/



		/*Actual velocity vectors*/
		const glm::vec3 move_forward = move_dir * user_forward * (player.is_sprinting ? FAST_SPEED : SPEED) * (player.is_grounded ? 1.0f : AIR_SPEED);
		const glm::vec3 move_right = move_dir * user_right * RIGHT_SPEED * (player.is_grounded ? 1.0f : AIR_SPEED);
		const glm::vec3 move_up = glm::vec3(0, player.is_grounded ? (up * JUMP_FORCE) : 0, 0);

		const glm::vec3 input_accel = (move_forward + move_right + move_up);

		//vel += input_accel;
		if (player.is_grounded && up != 0) {
			vel.y = 0;
		}
		accel += input_accel;

		/*Actual velocity vectors*/

		/*if(up > 0 && player.is_grounded)
			vel = input_vel;
		else
			vel = glm::vec3(input_vel.x, vel.y, input_vel.z);*/


	});

	entt::entity player = engine.getPlayer();
	if (Window::isPressed('T')) {
		//engine->replace<PositionComponent>(player, glm::vec3(0, 4, 0));
		Position &comp = engine.get<Position>(player);
		comp.x = 0;
		comp.z = 0;
	}


}