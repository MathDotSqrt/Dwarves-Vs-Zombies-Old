#include <stdio.h>
#include <memory>
#include <chrono>
#include <map>
#include <thread>

#include "entt.hpp"
#include "Components.h"
#include "GamePacketID.h"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "BitStream.h"

//https://fabiensanglard.net/quake3/network.php

#define FPS 60
#define MAX_CONNECTIONS 10
#define SERVER_PORT 60000

using namespace SLNet;

std::map<RakNetGUID, entt::entity> connectedClients;

void networkSystem(entt::registry &registry, RakPeerInterface *peer);
void update(entt::registry &registry);
void broadcast(entt::registry &registry, RakPeerInterface *peer);
MessageID getPacketID(Packet *p);

int main(void) {
	printf("Starting server...\n");
	auto deleter = [](RakPeerInterface *peer) {
		RakPeerInterface::DestroyInstance(peer);
	};
	std::unique_ptr<RakPeerInterface, decltype(deleter)> peer(RakPeerInterface::GetInstance(), deleter);

	SocketDescriptor sd(SERVER_PORT, nullptr);
	peer->Startup(MAX_CONNECTIONS, &sd, 1);
	peer->SetMaximumIncomingConnections(MAX_CONNECTIONS - 1);

	entt::registry registry;

	auto lastTime = std::chrono::system_clock::now();

	while (1) {

		auto currentTime = std::chrono::system_clock::now();

		std::chrono::duration<double> duration = currentTime - lastTime;
		if (duration.count() >= 1.0 / FPS) {
			networkSystem(registry, peer.get());
			broadcast(registry, peer.get());

			lastTime = currentTime;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return 0;
}


void networkSystem(entt::registry &registry, RakPeerInterface *peer) {
	Packet *packet;
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
			connectedClients[packet->guid] = player;
		}
			break;
		case ID_DISCONNECTION_NOTIFICATION: 
		{
			printf("CLIENT DISCONNECTED\n");
			entt::entity playerID = connectedClients[packet->guid];
			registry.destroy(playerID);
			connectedClients.erase(packet->guid);
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

void update(entt::registry &registry) {
	
}

void broadcast(entt::registry &registry, RakPeerInterface *peer) {
	auto view = registry.view<PositionComponent>();

	BitStream stream;

	for (entt::entity e : view) {
		stream.Reset();
		stream.Write((MessageID)ID_PLAYER_MOVE);
		stream.Write(e);
		stream.Write(registry.get<PositionComponent>(e));

		peer->Send(&stream, PacketPriority::MEDIUM_PRIORITY, PacketReliability::UNRELIABLE, 0, UNASSIGNED_RAKNET_GUID, true);
	}
}

MessageID getPacketID(Packet *packet) {
	MessageID id = (MessageID)packet->data[0];
	
	if (id == ID_TIMESTAMP) {
		return (MessageID)packet->data[sizeof(MessageID) + sizeof(Time)];
	}
	else {
		return (MessageID)packet->data[0];
	}
}






















//using namespace SLNet;
//
//MessageID GetPacketIdent(SLNet::Packet *p);
////this is test
//int main(void) {
//
//	entt::registry engine;
//
//	//Gives me one instance of a peer
//	RakPeerInterface *peer = RakPeerInterface::GetInstance();
//	SLNet::SocketDescriptor sd(SERVER_PORT, 0);
//	peer->Startup(MAX_CONNECTIONS, &sd, 1);
//	peer->SetMaximumIncomingConnections(MAX_CONNECTIONS + 1);
//
//	while (0) {
//		SLNet::Packet *packet;
//		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()) {
//			MessageID m = GetPacketIdent(packet);
//			//printf("Packet Received [%d] \n", (int)m);
//
//			switch (m) {
//			case ID_NEW_INCOMING_CONNECTION:
//			{
//				entt::entity id = engine.create();
//
//				engine.assign<PositionComponent>(id, 0.0f, 0.0f, 0.0f);
//				engine.assign<RotationComponent>(id, 0.0f, 0.0f, 0.0f);
//
//				printf("New Connection With Given Net ID\n");
//
//				printf("Sending Client Net ID [%d]\n",id);
//				
//				SLNet::BitStream out;
//				out.Write((SLNet::MessageID)GamePacketID::ID_CLIENT_NET_ID);
//				out.Write(id);
//				peer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
//				break;
//			}
//			case GamePacketID::ID_PLAYER_POS:
//				SLNet::BitStream in(packet->data, packet->length, false);
//				in.IgnoreBytes(sizeof(SLNet::MessageID));
//				
//				entt::entity id;
//				in.Read(id);
//
//				struct {
//					float x, y, z;
//				} pos, rot;
//
//				in.Read(pos);
//				in.Read(rot);
//				
//				SLNet::BitStream out;
//				out.Write((SLNet::MessageID)GamePacketID::ID_PLAYER_POS);
//				out.Write(id);
//				out.Write(pos.x);
//				out.Write(pos.y);
//				out.Write(pos.z);
//				out.Write(rot.x);
//				out.Write(rot.y);
//				out.Write(rot.z);
//				peer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);
//				break;
//			
//			}
//
//			
//		}
//	}
//
//	SLNet::RakPeerInterface::DestroyInstance(peer);
//}
//
////peer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);
//
////Sometimes a packet will send a timestamp before its messageID
//MessageID GetPacketIdent(SLNet::Packet *p) {
//	if ((unsigned char)p->data[0] == ID_TIMESTAMP) {
//		return (MessageID)p->data[sizeof(MessageID) + sizeof(SLNet::Time)];
//	}
//	else {
//		return p->data[0];
//	}
//}
