#include <stdio.h>
#include <iostream>
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "GamePacketID.h"
#include "BitStream.h"
#include "entt.hpp"
#include "Components.h"

#define MAX_CONNECTIONS 10
#define SERVER_PORT 60000

using namespace SLNet;

MessageID GetPacketIdent(SLNet::Packet *p);

int main(void) {

	entt::registry engine;

	//Gives me one instance of a peer
	RakPeerInterface *peer = RakPeerInterface::GetInstance();
	SLNet::SocketDescriptor sd(SERVER_PORT, 0);
	peer->Startup(MAX_CONNECTIONS, &sd, 1);
	peer->SetMaximumIncomingConnections(MAX_CONNECTIONS + 1);

	while (1) {
		SLNet::Packet *packet;
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()) {
			MessageID m = GetPacketIdent(packet);
			//printf("Packet Received [%d] \n", (int)m);

			switch (m) {
			case ID_NEW_INCOMING_CONNECTION:
			{
				entt::entity id = engine.create();

				engine.assign<PositionComponent>(id, 0.0f, 0.0f, 0.0f);
				engine.assign<RotationComponent>(id, 0.0f, 0.0f, 0.0f);

				printf("New Connection With Given Net ID\n");

				printf("Sending Client Net ID [%d]\n",id);
				
				SLNet::BitStream out;
				out.Write((SLNet::MessageID)GamePacketID::ID_CLIENT_NET_ID);
				out.Write(id);
				peer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				break;
			}
			case GamePacketID::ID_PLAYER_POS:
				SLNet::BitStream in(packet->data, packet->length, false);
				in.IgnoreBytes(sizeof(SLNet::MessageID));
				
				entt::entity id;
				in.Read(id);

				struct {
					float x, y, z;
				} pos, rot;

				in.Read(pos);
				in.Read(rot);
				
				SLNet::BitStream out;
				out.Write((SLNet::MessageID)GamePacketID::ID_PLAYER_POS);
				out.Write(id);
				out.Write(pos.x);
				out.Write(pos.y);
				out.Write(pos.z);
				out.Write(rot.x);
				out.Write(rot.y);
				out.Write(rot.z);
				peer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);
				break;
			
			}

			
		}
	}

	SLNet::RakPeerInterface::DestroyInstance(peer);
}

//peer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);

//Sometimes a packet will send a timestamp before its messageID
MessageID GetPacketIdent(SLNet::Packet *p) {
	if ((unsigned char)p->data[0] == ID_TIMESTAMP) {
		return (MessageID)p->data[sizeof(MessageID) + sizeof(SLNet::Time)];
	}
	else {
		return p->data[0];
	}
}