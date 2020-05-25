#include "GameLogic.h"
#include "Components.h"
#include "SingletonComponents.h"
#include "NetUtil.h"
#include "ChunkManager.h"
#include "RLEncoder.h"
#include "VoxelCollision.h"
#include "HandleVoxelCollision.h"


using namespace GameLogic;

void GameLogic::input_system(EntityAdmin &admin, float delta) {
	using namespace Component;
	
	entt::registry &registry = admin.registry;

	registry.group<Input>(entt::get<Dir, Rotation, Velocity>)
		.each([delta](auto &input, auto &dir, auto &rot, auto &vel) {
		
		/*CONSTANTS*/
		const float SPEED = 9.0f;
		const float TURN_SPEED = .5f;
		const float FAST_SPEED = 88.0f;
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
		/*Movement scalars*/

		/*Movement input vectors/quaternions*/
		const glm::vec3 user_forward = dir.forward * forward * (input.ctrl ? FAST_SPEED : SPEED);
		const glm::vec3 user_right = dir.right * right * (input.ctrl ? FAST_SPEED : SPEED);
		/*Movement input vectors/quaternions*/

		/*Actual velocity vectors*/
		const glm::vec3 move_forward = rot * user_forward;
		const glm::vec3 move_right = rot * user_right;
		const glm::vec3 move_up = glm::vec3(0, up * (input.ctrl ? FAST_SPEED : SPEED), 0);

		const glm::vec3 input_vel = move_forward + move_right + move_up;
		/*Actual velocity vectors*/

		if (up > 0)
			vel = input_vel;
		else
			vel = glm::vec3(input_vel.x, vel.y, input_vel.z);

	});
}

void GameLogic::gravity_system(EntityAdmin &admin, float delta) {
	using namespace Component;

	
}

void GameLogic::movement_system(EntityAdmin &admin, float delta) {
	using namespace Component;
	
	entt::registry &registry = admin.registry;
	//registry.view<Velocity, Acceleration>().each([delta](auto &vel, auto &accel) {
	//	accel += glm::vec3(0, -35.0f, 0);
	//	vel += accel * delta * .05f;
	//});

	registry.group<Position, Velocity>().each([delta](auto &pos, auto &vel) {
		pos += delta * vel;
	});

	//registry.view<Velocity, Acceleration>().each([delta](auto &vel, auto &accel) {
	//	vel += accel * delta * .05f;
	//});

}

void GameLogic::afk_system(EntityAdmin &admin, float delta) {
	using namespace Component;
	constexpr float AFK_TIMEOUT = 60 * 20;		//20 minutes
	
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
				break;
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

void GameLogic::voxel_collision_system(EntityAdmin &admin, float delta) {
	using namespace Component;
	using namespace Physics;

	auto &registry = admin.registry;

	const auto &manager = registry.ctx<Voxel::ChunkManager>();

	Physics::GetBlockFunc getBlockFunc = [&manager](const glm::i32vec3 &coord) {
		return manager.getBlock(coord);
	};

	auto view = registry.view<Position, Velocity, VoxelCollision>();
	view.each([&getBlockFunc, delta](auto &pos, auto &vel, auto &collision) {
		const auto worldspace_aabb = Physics::translate_aabb(collision.aabb, pos);
		auto broadphase = Physics::broadphase(worldspace_aabb, vel * delta, getBlockFunc);
		
		const auto new_vel = Physics::sample_terrain_collision(
			pos,
			vel,
			collision.aabb,
			delta,
			broadphase,
			collision.sample
		);

		vel = new_vel;
	});

}