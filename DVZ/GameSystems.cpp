#include "GameSystems.h"
#include "Engine.h"
#include "Components.h"
#include "macrologger.h"

#include "Window.h"


#include "glm.hpp"
#include "VAO.h"
#include "VBO.h"
#include "ShaderSet.h"
#include "Scene.h"
#include <gtc/quaternion.hpp>

#include "ChunkManager.h"
#include "ChunkRenderDataManager.h"


using namespace GameSystem;


void GameSystem::input_system(Engine &engine, float delta) {
	engine.view<InputComponent>().each([](auto &input) {
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

	engine.group<InputComponent>(entt::get<DirComponent, RotationComponent, VelocityComponent>)
		.each([delta](auto &input, auto &dir, auto &rot, auto &vel) {

		const float SPEED = 9.0f;
		const float TURN_SPEED = .5f;
		const float FAST_SPEED = 88.0f;

		float forward = (float)input.up - (float)input.down;
		float right = (float)input.right - (float)input.left;
		float up = (float)input.space - (float)input.shift;
		glm::vec2 mouseDelta = input.mousePos[1] - input.mousePos[0];

		glm::vec3 userForward = dir.forward * forward * (input.ctrl ? FAST_SPEED : SPEED);
		glm::vec3 userRight = dir.right * right * (input.ctrl ? FAST_SPEED : SPEED);

		glm::vec3 newForward = rot * userForward;
		glm::vec3 newRight = rot * userRight;

		vel = newForward + newRight;
		vel.y += up * (input.ctrl ? FAST_SPEED : SPEED);

		//todo see if i need to put this above newForward and newRight code
		glm::quat qYaw = glm::angleAxis((float)-mouseDelta.x * TURN_SPEED / 100.0f, (glm::vec3)dir.up);
		glm::quat qPitch = glm::angleAxis((float)-mouseDelta.y * TURN_SPEED / 100.0f, (glm::vec3)dir.right);
		rot = (qYaw * (rot)) * qPitch;

	});

	entt::entity player = engine.getPlayer();
	if (Window::isPressed('T')) {
		//engine->replace<PositionComponent>(player, glm::vec3(0, 4, 0));
		PositionComponent &comp = engine.get<PositionComponent>(player);
		comp.x = 0;
		comp.z = 0;
	}
}

void GameSystem::movement_system(Engine &engine, float delta) {
	engine.group<PositionComponent, VelocityComponent>().each([delta](auto &pos, auto &vel) {
		pos += vel * delta;
	});
}

void GameSystem::voxel_system(Engine &engine, float delta) {
	Voxel::ChunkManager &manager = engine.ctx<Voxel::ChunkManager>();
	PositionComponent p = engine.get<PositionComponent>(engine.getPlayer());
	RotationComponent r = engine.get<RotationComponent>(engine.getPlayer());
	glm::vec3 playerPos = p;
	glm::vec3 playerRot = glm::eulerAngles(r);

	manager.update(playerPos.x, playerPos.y, playerPos.z);

	static bool badCode = true;

	bool left = Window::isMousePressed(Window::LEFT_CLICK);
	bool right = Window::isMousePressed(Window::RIGHT_CLICK);
	if (badCode && (left || right)) {
		DirComponent dir = engine.get<DirComponent>(engine.getPlayer());
		RotationComponent rot = engine.get<RotationComponent>(engine.getPlayer());

		glm::vec3 ray = glm::rotate(rot, dir.forward);
		Voxel::BlockRayCast cast = manager.castRay(playerPos, ray, 10);

		if (left && cast.block != Voxel::Block()) {
			manager.setBlock(cast.nx, cast.ny, cast.nz, Voxel::Block(Voxel::BlockType::BLOCK_TYPE_PURPLE));

		}
		else if (right) {
			manager.setBlock(cast.x, cast.y, cast.z, Voxel::Block());
		}

		badCode = false;
	}
	else {
		badCode = !(left || right);
	}

	auto &renderDataManager = engine.ctx<Voxel::ChunkRenderDataManager>();
	renderDataManager.update(playerPos, playerRot, manager);
}

void GameSystem::render_system(Engine &engine, float delta) {
	auto &scene = engine.ctx<Graphics::Scene>();

	engine.group<RenderInstanceComponent>(entt::get<PositionComponent, RotationComponent, ScaleComponent>)
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

	engine.group<PointLightComponent>(entt::get<PositionComponent>)
		.each([&scene](auto &pointLightComponent, auto &positionComponent)
	{
		Graphics::Scene::PointLight &pointLight = scene.pointLightCache[pointLightComponent.lightInstanceID];

		pointLight.position = positionComponent;
		pointLight.color[0] = pointLightComponent.color.x;
		pointLight.color[1] = pointLightComponent.color.y;
		pointLight.color[2] = pointLightComponent.color.z;
		pointLight.intensity = pointLightComponent.intensity;
	});

	engine.group<CameraInstanceComponent>(entt::get<PositionComponent, RotationComponent, DirComponent>)
		.each([&scene](auto &cameraComponent, auto &positionComponent, auto &rotationComponent, auto &dirComponent)
	{
		auto &camera = scene.cameraCache[cameraComponent.cameraID];

		glm::quat orientation = rotationComponent;
		camera.eye = positionComponent;
		camera.target = orientation * dirComponent.forward;
		camera.up = orientation * dirComponent.up;
	});
}

void GameSystem::shader_update_system(Engine &engine, float delta) {
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

