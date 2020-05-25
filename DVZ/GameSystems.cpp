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

