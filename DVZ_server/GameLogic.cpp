#include "GameLogic.h"
#include "Components.h"
#include "SingletonComponents.h"
#include "NetUtil.h"
#include "ChunkManager.h"
#include "RLEncoder.h"

using namespace GameLogic;

void GameLogic::input_system(EntityAdmin &admin, float delta) {
	using namespace Component;
	
	entt::registry &registry = admin.registry;

	registry.group<Input>(entt::get<Dir, Rotation, Velocity>)
		.each([delta](auto &input, auto &dir, auto &rot, auto &vel) {
		
		const float SPEED = 9.0f;
		const float TURN_SPEED = .5f;
		const float FAST_SPEED = 88.0f;

		float forward = (float)input.up - (float)input.down;
		float right = (float)input.right - (float)input.left;
		float up = (float)input.space - (float)input.shift;
		//glm::vec2 mouseDelta = input.mousePos[1] - input.mousePos[0];

		glm::vec3 userForward = dir.forward * forward * (input.ctrl ? FAST_SPEED : SPEED);
		glm::vec3 userRight = dir.right * right * (input.ctrl ? FAST_SPEED : SPEED);

		glm::vec3 newForward = rot * userForward;
		glm::vec3 newRight = rot * userRight;

		vel = newForward + newRight;
		vel.y += up * (input.ctrl ? FAST_SPEED : SPEED);

		//todo see if i need to put this above newForward and newRight code
		//glm::quat qYaw = glm::angleAxis((float)-mouseDelta.x * TURN_SPEED / 100.0f, (glm::vec3)dir.up);
		//glm::quat qPitch = glm::angleAxis((float)-mouseDelta.y * TURN_SPEED / 100.0f, (glm::vec3)dir.right);
		//rot = (qYaw * (rot)) * qPitch;

	});
}

void GameLogic::movement_system(EntityAdmin &admin, float delta) {
	using namespace Component;
	
	entt::registry &registry = admin.registry;
	registry.group<Position, Velocity>().each([delta](auto &pos, auto &vel) {
		pos += delta * vel;
	});

}

void GameLogic::afk_system(EntityAdmin &admin, float delta) {
	using namespace Component;
	constexpr float AFK_TIMEOUT = 60 * 5;		//five minutes
	
	entt::registry &registry = admin.registry;
	auto view = registry.view<Input, AFK, NetClient>();

	for (entt::entity e : view) {
		auto &input = view.get<Input>(e);
		auto &afk = view.get<AFK>(e);
		auto &client = view.get<NetClient>(e);

		if (afk.lastInput == input) {
			afk.timer += delta;
			if (afk.timer >= AFK_TIMEOUT) {
				printf("ENTITY AFK TOO LONG\n");
				registry.destroy(e);
				closeClientConnection(admin, client.guid);
			}
		}
		else {
			afk.timer = 0;
			afk.lastInput = input;
		}

	}
}

void GameLogic::voxel_system(EntityAdmin &admin, float delta) {
	using namespace Component;
	auto &manager = admin.getChunkManager();
	manager.update(admin, delta);

	admin.registry.view<Position, ChunkBoundry>().each([](auto &pos, auto &bound) {
		bound.x = (int)(pos.x / Voxel::CHUNK_RENDER_WIDTH_X);
		bound.y = (int)(pos.y / Voxel::CHUNK_RENDER_WIDTH_Y);
		bound.z = (int)(pos.z / Voxel::CHUNK_RENDER_WIDTH_Z);
	});

}