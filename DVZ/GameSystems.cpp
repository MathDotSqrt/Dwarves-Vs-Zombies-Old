#include "GameSystems.h"
#include "Engine.h"
#include "Components.h"
#include "macrologger.h"

#include "GameUtil.h"
#include "MathUtil.h"

#include "Window.h"

#include "glm.hpp"
#include "VAO.h"
#include "VBO.h"
#include "ShaderSet.h"
#include "Scene.h"
#include "gtc/quaternion.hpp"

#include "ChunkManager.h"
#include "ChunkRenderDataManager.h"
#include "CoordIterator.h"

#include <optional>

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

void System::player_state_system(Engine &engine, float delta) {
	using namespace Component;
	engine.view<Player, VoxelCollision>().each([](auto &player, auto &collision) {
		player.is_grounded = collision.sample.ny.has_value();
	});

	engine.view<Player, Velocity>().each([](auto &player, auto &vel) {
		constexpr float SPRINT_THRESH = 30.0f;

		const float speed_2 = glm::length2(static_cast<glm::vec3>(vel));
		if (player.is_sprinting || player.is_grounded) {
			player.is_sprinting = Window::isPressed('w')
				&& Window::isPressed(Window::LCTRL)
				&& (speed_2 > SPRINT_THRESH);
		}
		else {
			player.is_sprinting = false;
		}
	});
}

void System::sprint_system(Engine &engine, float delta) {
	using namespace Component;

	auto &scene = engine.ctx<Graphics::Scene>();
	const auto &camera = scene.cameraCache[scene.getMainCameraID()];
	auto view = engine.view<Player, CameraInstance>();
	view.each([&scene, delta](auto &player, auto &cameraComponent) {
		constexpr float SPRINT_FOV = glm::radians(90.0f);
		constexpr float WALK_FOV = glm::radians(80.0f);
		constexpr float SPRINT_FOV_SPEED = 20.0f;

		auto &camera = scene.cameraCache[cameraComponent.cameraID];
		const float current_fov = camera.fov;
		if (player.is_sprinting) {
			camera.fov = Math::lerp(current_fov, SPRINT_FOV, delta * SPRINT_FOV_SPEED);
		}
		else {
			camera.fov = Math::lerp(current_fov, WALK_FOV, delta * SPRINT_FOV_SPEED);
		}

	});
}

void System::collision_test_system(Engine &engine, float delta) {
	using namespace Component;
	
	static bool test = false;

	auto view = engine.view<Player, Position, Rotation, Velocity, Input>();
	assert(view.begin() != view.end());

	auto &entity = *view.begin();

	if (Window::isPressed('r') && test == false) {
		test = !test;

		auto &chunk_manager = engine.ctx<Voxel::ChunkManager>();
		const Voxel::Block AIR(Voxel::BlockType::BLOCK_TYPE_DEFAULT);
		const Voxel::Block GLASS(Voxel::BlockType::BLOCK_TYPE_GLASS);
		chunk_manager.setBlock(26, 1, 26, AIR);
		chunk_manager.setBlock(28, 2, 25, GLASS);
		engine.get<Position>(entity) = glm::vec3(26.67, 2 + .5f, 26.3);
		engine.get<Velocity>(entity) = glm::vec3(0);


		engine.get<Rotation>(entity) = glm::quat(glm::vec3(0, -glm::pi<float>() / 2.0f + .05f, 0));
	}
	else if (Window::isPressed('r') == false && test == true) {
		test = false;
	}

	if (test) {
		
		auto &input = engine.get<Input>(entity);
		input.up = true;
		input.space = true;
	}
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

void System::voxel_system(Engine &engine, float delta) {
	using namespace Component;
	
	Voxel::ChunkManager &manager = engine.ctx<Voxel::ChunkManager>();

	const auto playerID = engine.getPlayer();

	const Position pos = engine.get<Position>(playerID);
	const Rotation rot = engine.get<Rotation>(playerID);
	manager.update(pos.x, pos.y, pos.z);

	static bool badCode = true;
	const bool left = Window::isMousePressed(Window::LEFT_CLICK);
	const bool right = Window::isMousePressed(Window::RIGHT_CLICK);
	if (badCode && (left || right)) {
		const Dir dir = engine.get<Dir>(engine.getPlayer());

		glm::vec3 ray = glm::rotate(rot, dir.forward);
		Voxel::BlockRayCast cast = manager.castRay(pos, ray, 10);

		if (right && cast.block != Voxel::Block()) {
			const glm::i32vec3 block_coord(cast.nx, cast.ny, cast.nz);
			GameUtil::attempt_place_block_net(engine, block_coord, Voxel::Block(Voxel::BlockType::BLOCK_TYPE_PURPLE));
		}
		else if (left) {
			const glm::i32vec3 block_coord(cast.x, cast.y, cast.z);
			GameUtil::attempt_place_block_net(engine, block_coord, Voxel::BlockType::BLOCK_TYPE_DEFAULT);
		}

		badCode = false;
	}
	else {
		badCode = !(left || right);
	}

	auto &renderDataManager = engine.ctx<Voxel::ChunkRenderDataManager>();
	renderDataManager.update(pos, glm::eulerAngles(rot), manager);
}

void System::render_system(Engine &engine, float delta) {
	using namespace Component;
	auto &scene = engine.ctx<Graphics::Scene>();

	engine.group<RenderInstance>(entt::get<Position, Rotation, Scale>)
		.each([&scene](auto &instance, auto &pos, auto &rot, auto &scale)
	{
		auto transformationID = scene.instanceCache[instance.instanceID].transformationID;
		auto &transformation = scene.transformationCache[transformationID];

		glm::vec3 a = glm::eulerAngles(rot);
		//LOG_INFO("%f %f %f", a.x, a.y, a.z);

		transformation.position = pos + instance.offset;
		transformation.rotation = glm::eulerAngles(rot);
		transformation.scale = scale * instance.scale;
	});

	engine.group<PointLight>(entt::get<Position>)
		.each([&scene](auto &pointLightComponent, auto &positionComponent)
	{
		Graphics::Scene::PointLight &pointLight = scene.pointLightCache[pointLightComponent.lightInstanceID];

		pointLight.position = positionComponent;
		pointLight.color[0] = pointLightComponent.color.x;
		pointLight.color[1] = pointLightComponent.color.y;
		pointLight.color[2] = pointLightComponent.color.z;
		pointLight.intensity = pointLightComponent.intensity;
	});

	engine.group<CameraInstance>(entt::get<Position, Rotation, Dir>)
		.each([&scene](auto &cameraComponent, auto &positionComponent, auto &rotationComponent, auto &dirComponent)
	{
		auto &camera = scene.cameraCache[cameraComponent.cameraID];
		glm::quat orientation = rotationComponent;
		camera.eye = positionComponent;
		camera.target = orientation * dirComponent.forward;
		camera.up = orientation * dirComponent.up;
	});
}

void System::shader_update_system(Engine &engine, float delta) {
	Graphics::Shader::ShaderIterator iterator = Graphics::Shader::begin();

	while (iterator != Graphics::Shader::end()) {
		Graphics::Shader::GLSLProgram *program = iterator->second;
		bool shaderNeedsUpdate = false;

		for (auto& fileData : program->getFiledata()) {
			HANDLE fileHandle = (HANDLE)fileData.first;
			FILETIME lastWriteTime = { fileData.second.low, fileData.second.high };
			if (fileHandle != INVALID_HANDLE_VALUE) {
				FILETIME currentWriteTime = { 0 };
				GetFileTime(fileHandle, NULL, NULL, &currentWriteTime);

				bool fileUpdated = false;

				if (currentWriteTime.dwHighDateTime > lastWriteTime.dwHighDateTime) {
					fileUpdated |= true;
				}
				else if (currentWriteTime.dwHighDateTime == lastWriteTime.dwHighDateTime
					&& currentWriteTime.dwLowDateTime > lastWriteTime.dwLowDateTime) {
					fileUpdated |= true;
				}

				if (fileUpdated) {
					fileData.second.low = currentWriteTime.dwLowDateTime;
					fileData.second.high = currentWriteTime.dwHighDateTime;
					shaderNeedsUpdate = true;
				}

			}
		}
		if (shaderNeedsUpdate) {
			LOG_SYSTEM("Updating Shader...");
			Graphics::Shader::reloadShader(&program);
		}

		iterator++;
	}
}

