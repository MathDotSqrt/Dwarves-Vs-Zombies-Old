#include "EntityAdmin.h"
#include "Components.h"
#include "SingletonComponents.h"


#include "NetReader.h"
#include "MessageIdentifiers.h"
#include "RakNetTypes.h"
#include "GamePacketID.h"
#include "BitStream.h"
#include "NetUtil.h"


using namespace System;
using namespace SLNet;

void System::net_update(EntityAdmin &admin) {
	entt::registry &registry = admin.registry;
	auto peer = admin.getPeer();
	auto &map = registry.ctx<ConnectedClientMap>();

	Packet *packet = nullptr;
	for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()) {
		MessageID id = getPacketID(packet);
		BitStream in(packet->data, packet->length, false);
		BitStream out;

		switch (id) {
		case ID_NEW_INCOMING_CONNECTION: 
		{
			printf("New connection from [%s]\n", packet->systemAddress.ToString());

			entt::entity player = registry.create();
			registry.assign<PositionComponent>(player, glm::vec3(0, 0, 0));
			registry.assign<VelocityComponent>(player, glm::vec3(0, 0, 0));
			registry.assign<RotationComponent>(player, glm::vec3(0, 0, 0));
			registry.assign<DirComponent>(player, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
			registry.assign<InputComponent>(player, false, false, false, false, glm::vec2());

			out.Write((MessageID)ID_CLIENT_NET_ID);
			out.Write(player);
			peer->Send(&out, PacketPriority::IMMEDIATE_PRIORITY, PacketReliability::RELIABLE, 0, packet->systemAddress, false);
			map[packet->guid] = player;
		}
			break;
		case ID_DISCONNECTION_NOTIFICATION: 
		{
			printf("CLIENT DISCONNECTED\n");
			entt::entity playerID = map[packet->guid];
			registry.destroy(playerID);
			map.erase(packet->guid);
		}
			break;
		case ID_CLIENT_INPUT:
		{
			in.IgnoreBytes(sizeof(MessageID));
			entt::entity clientEntity;
			in.Read(clientEntity);
			auto &pos = registry.get<PositionComponent>(clientEntity);
			in.Read(pos);
		}
			break;
		default:
			printf("SOMETHING REVICEDC\n");
			break;
		}
	}
}

void System::net_broadcast(EntityAdmin &admin) {
	auto &registry = admin.registry;
	auto *peer = admin.getPeer();

	auto view = registry.view<PositionComponent>();

	for (entt::entity e : view) {
		BitStream stream;
		stream.Write((MessageID)ID_PLAYER_MOVE);
		stream.Write(e);
		stream.Write(view.get(e));
		peer->Send(&stream, PacketPriority::MEDIUM_PRIORITY, PacketReliability::UNRELIABLE, 0, UNASSIGNED_RAKNET_GUID, true);
	}
}
