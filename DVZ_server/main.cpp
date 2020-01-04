#include <stdio.h>
#include <memory>
#include <chrono>
#include <map>
#include <thread>

#include "EntityAdmin.h"

#include "entt.hpp"
#include "Components.h"
#include "GamePacketID.h"


//https://fabiensanglard.net/quake3/network.php

#define FPS 60
#define SPF (1.0 / FPS)

int main(void) {
	printf("Starting server...\n");

	EntityAdmin admin;

	entt::registry registry;

	auto lastTime = std::chrono::system_clock::now();

	while (1) {

		auto currentTime = std::chrono::system_clock::now();

		std::chrono::duration<double> duration = currentTime - lastTime;
		auto timeTillNextFrame = SPF - duration.count();
		if ( timeTillNextFrame > (SPF / 100.0)) {
			std::this_thread::sleep_for(std::chrono::milliseconds((int)(timeTillNextFrame * 900)));
			continue;
		}

		if (duration.count() >= SPF) {
			admin.update((float)duration.count());
			lastTime = currentTime;
		}

	}

	return 0;
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
