#include "GameSystems.h"
#include "Engine.h"
#include "Components.h"
#include "macrologger.h"

#include "GameUtil.h"

#include "Window.h"

#include "glm.hpp"
#include "VAO.h"
#include "VBO.h"
#include "ShaderSet.h"
#include "Scene.h"
#include <gtc/quaternion.hpp>

#include "ChunkManager.h"
#include "ChunkRenderDataManager.h"
#include "CoordIterator.h"

#include <optional>

using namespace System;

void System::gravity_system(Engine &engine, float delta) {
	using namespace Component;
	engine.view<Velocity, Acceleration>().each([delta](auto &vel, auto &accel) {
		accel = glm::vec3(0, -35.0f, 0);
		vel += accel * delta;
	});
}

void System::movement_system(Engine &engine, float delta) {
	using namespace Component;

	engine.group<Position, Velocity>().each([delta](auto &pos, auto &vel) {
		pos += vel * delta;
	});
}

void System::player_state_system(Engine &engine, float delta) {
	using namespace Component;
	engine.view<Player, VoxelCollision>().each([](auto &player, auto &collision) {
		player.can_jump = collision.sample.ny.has_value();
	});
}

void System::input_system(Engine &engine, float delta) {
	using namespace Component;
	
	engine.view<Input>().each([](auto &input) {
		input.mousePos[0] = input.mousePos[1];

		if (Window::isFocused()) {
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

	engine.group<Input>(entt::get<Dir, Rotation, Velocity, Player>)
		.each([delta](auto &input, auto &dir, auto &rot, auto &vel, auto &player) {

		/*CONSTANTS*/
		const float SPEED = 8.0f;
		const float JUMP_FORCE = 11.0f;
		const float TURN_SPEED = .5f;
		const float FAST_SPEED = 13.0f;
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
		const glm::vec3 user_forward = dir.forward * forward * (input.ctrl ? FAST_SPEED : SPEED);
		const glm::vec3 user_right = dir.right * right * (input.ctrl ? FAST_SPEED : SPEED);

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
		/*Camera rotation code*/

		/*Actual velocity vectors*/
		const glm::vec3 move_forward = move_dir * user_forward;
		const glm::vec3 move_right = move_dir * user_right;
		const glm::vec3 move_up = glm::vec3(0, up * JUMP_FORCE, 0);

		const glm::vec3 input_vel = move_forward + move_right + move_up;
		/*Actual velocity vectors*/

		if(up > 0 && player.can_jump)
			vel = input_vel;
		else
			vel = glm::vec3(input_vel.x, vel.y, input_vel.z);

		rot = new_rot;

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
		.each([&scene](auto &instanceComponent, auto &positionComponent, auto &rotationComponent, auto &scaleComponent)
	{
		auto transformationID = scene.instanceCache[instanceComponent.instanceID].transformationID;
		auto &transformation = scene.transformationCache[transformationID];

		glm::vec3 a = glm::eulerAngles(rotationComponent);
		//LOG_INFO("%f %f %f", a.x, a.y, a.z);

		transformation.position = positionComponent;
		transformation.rotation = glm::eulerAngles(rotationComponent);
		transformation.scale = scaleComponent;
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

