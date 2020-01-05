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

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "GamePacketID.h"
#include <unordered_map>

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

		glm::vec3 newForward = rot.rot * userForward;
		glm::vec3 newRight = rot.rot * userRight;

		vel.vel = newForward + newRight;
		vel.vel.y += up * (input.ctrl ? FAST_SPEED : SPEED);

		//todo see if i need to put this above newForward and newRight code
		glm::quat qYaw = glm::angleAxis((float)-mouseDelta.x * TURN_SPEED / 100.0f, (glm::vec3)dir.up);
		glm::quat qPitch = glm::angleAxis((float)-mouseDelta.y * TURN_SPEED / 100.0f, (glm::vec3)dir.right);
		rot.rot = (qYaw * (rot.rot)) * qPitch;

	});

	entt::entity player = engine.getPlayer();
	if (Window::isPressed('T')) {
		//engine->replace<PositionComponent>(player, glm::vec3(0, 4, 0));
		PositionComponent &comp = engine.get<PositionComponent>(player);
		comp.pos.x = 0;
		comp.pos.z = 0;
	}
}

void GameSystem::movement_system(Engine &engine, float delta) {
	engine.group<PositionComponent, VelocityComponent>().each([delta](auto &pos, auto &vel) {
		pos.pos += vel.vel * delta;
	});
}

void GameSystem::voxel_system(Engine &engine, float delta) {
	Voxel::ChunkManager *manager = engine.getChunkManager();
	PositionComponent p = engine.get<PositionComponent>(engine.getPlayer());
	RotationComponent r = engine.get<RotationComponent>(engine.getPlayer());
	glm::vec3 playerPos = p.pos;
	glm::vec3 playerRot = glm::eulerAngles(r.rot);

	manager->update(playerPos.x, playerPos.y, playerPos.z);

	static bool badCode = true;

	bool left = Window::isMousePressed(Window::LEFT_CLICK);
	bool right = Window::isMousePressed(Window::RIGHT_CLICK);
	if (badCode && (left || right)) {
		DirComponent dir = engine.get<DirComponent>(engine.getPlayer());
		RotationComponent rot = engine.get<RotationComponent>(engine.getPlayer());

		glm::vec3 ray = glm::rotate(rot.rot, dir.forward);
		Voxel::BlockRayCast cast = manager->castRay(playerPos, ray, 10);

		if (left && cast.block != Voxel::Block()) {
			manager->setBlock(cast.nx, cast.ny, cast.nz, Voxel::Block(Voxel::BlockType::BLOCK_TYPE_PURPLE));

		}
		else if (right) {
			manager->setBlock(cast.x, cast.y, cast.z, Voxel::Block());
		}

		badCode = false;
	}
	else {
		badCode = !(left || right);
	}

	engine.getChunkRenderDataManager()->update(playerPos, playerRot, *manager);
}

void GameSystem::render_system(Engine &engine, float delta) {
	Graphics::Scene *scene = engine.getScene();

	engine.group<RenderInstanceComponent>(entt::get<PositionComponent, RotationComponent, ScaleComponent>)
		.each([scene](auto &instanceComponent, auto &positionComponent, auto &rotationComponent, auto &scaleComponent)
	{
		auto transformationID = scene->instanceCache[instanceComponent.instanceID].transformationID;
		auto *transformation = &scene->transformationCache[transformationID];

		glm::vec3 a = glm::eulerAngles(rotationComponent.rot);
		//LOG_INFO("%f %f %f", a.x, a.y, a.z);

		transformation->position = positionComponent.pos;
		transformation->rotation = glm::eulerAngles(rotationComponent.rot);
		transformation->scale = scaleComponent.scale;
	});

	engine.group<PointLightComponent>(entt::get<PositionComponent>)
		.each([scene](auto &pointLightComponent, auto &positionComponent)
	{
		Graphics::Scene::PointLight &pointLight = scene->pointLightCache[pointLightComponent.lightInstanceID];

		pointLight.position = positionComponent.pos;
		pointLight.color[0] = pointLightComponent.color.x;
		pointLight.color[1] = pointLightComponent.color.y;
		pointLight.color[2] = pointLightComponent.color.z;
		pointLight.intensity = pointLightComponent.intensity;
	});

	engine.group<CameraInstanceComponent>(entt::get<PositionComponent, RotationComponent, DirComponent>)
		.each([scene](auto &cameraComponent, auto &positionComponent, auto &rotationComponent, auto &dirComponent)
	{
		auto *camera = &scene->cameraCache[cameraComponent.cameraID];

		glm::quat orientation = rotationComponent.rot;

		camera->eye = positionComponent.pos;
		camera->target = orientation * dirComponent.forward;
		camera->up = orientation * dirComponent.up;
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

void update_player(Engine &engine, SLNet::Packet *packet) {
	SLNet::BitStream read(packet->data, packet->length, false);

	read.IgnoreBytes(sizeof(SLNet::MessageID));

	entt::entity playerID = entt::null;
	entt::entity netID = entt::null;
	glm::vec3 pos;
	read.Read(netID);
	read.Read(pos);

	auto &map = engine.ctx<std::unordered_map<entt::entity, entt::entity>>();

	auto iter = map.find(netID);
	if (iter == map.end()) {
		playerID = engine.create();
		engine.assign<PositionComponent>(playerID, pos);
		engine.assign<RotationComponent>(playerID, glm::quat(glm::vec3(0, 0, 0)));
		engine.assign<ScaleComponent>(playerID, glm::vec3(1, 1, 1));

		map[netID] = playerID;
		LOG_NET("NEW ENT");
	}
	else {
		playerID = iter->second;
		auto &position = engine.get<PositionComponent>(playerID);
		position.pos = pos;
	}
}

void recieve_net_id(Engine &engine, SLNet::Packet *packet) {
	SLNet::BitStream stream(packet->data, packet->length, false);

	auto &map = engine.ctx<std::unordered_map<entt::entity, entt::entity>>();

	stream.IgnoreBytes(sizeof(SLNet::MessageID));
	entt::entity playerID = engine.getPlayer();
	entt::entity netID = entt::null;
	stream.Read(netID);
	engine.assign_or_replace<NetworkComponent>(playerID, netID);
	map.erase(netID);
	map[netID] = playerID;
	LOG_NET("CLIENT ID RECIEVED: %d", netID);
}

SLNet::MessageID get_packet_id(SLNet::Packet *packet) {
	SLNet::MessageID id = (SLNet::MessageID)packet->data[0];

	if (id == ID_TIMESTAMP) {
		return (SLNet::MessageID)packet->data[sizeof(SLNet::MessageID) + sizeof(SLNet::Time)];
	}
	else {
		return (SLNet::MessageID)packet->data[0];
	}
}

void GameSystem::netword_system(Engine &engine, float delta) {
	SLNet::RakPeerInterface *peer = engine.getPeer();

	entt::entity id = engine.getPlayer();

	SLNet::Packet *packet;
	for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()) {
		SLNet::MessageID id = get_packet_id(packet);


		switch (id) {
		case ID_CONNECTION_REQUEST_ACCEPTED:
			LOG_NET("CONNECTION ACCEPTED");
			break;
		case ID_PLAYER_MOVE:
			update_player(engine, packet);
			break;
		case ID_CLIENT_NET_ID:
			recieve_net_id(engine, packet);
			break;
		default:
			LOG_NET("ID CAUGHT: %d", id);
		}
	}
}

void GameSystem::send_packet_system(Engine &engine, float delta) {
	auto view = engine.view<PlayerComponent, NetworkComponent>();
	if (view.size()) {
		entt::entity playerID = *view.begin();
		auto &pos = engine.get<PositionComponent>(playerID);
		auto &rot = engine.get<RotationComponent>(playerID);
		auto &input = engine.get<InputComponent>(playerID);
		auto &net = engine.get<NetworkComponent>(playerID);
		SLNet::BitStream out;
		out.Write((SLNet::MessageID)ID_CLIENT_INPUT);
		out.Write(rot);
		out.Write(input.up);
		out.Write(input.down);
		out.Write(input.left);
		out.Write(input.right);
		out.Write(input.space);
		out.Write(input.shift);
		out.Write(input.ctrl);
		//out.Write(input.mousePos[0]);
		//out.Write(input.mousePos[1]);


		auto peer = engine.getPeer();
		peer->Send(&out, LOW_PRIORITY, PacketReliability::UNRELIABLE, 0, SLNet::UNASSIGNED_RAKNET_GUID, true);
	}
}