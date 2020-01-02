#include <stdio.h>
#include <memory>
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"

#define MAX_CONNECTIONS 10
#define SERVER_PORT 60000

using namespace SLNet;

MessageID getPacketID(Packet *p);

int main(void) {
	auto deleter = [](RakPeerInterface *peer) {
		RakPeerInterface::DestroyInstance(peer);
	};
	std::unique_ptr<RakPeerInterface, decltype(deleter)> peer(RakPeerInterface::GetInstance(), deleter);

	SocketDescriptor sd(SERVER_PORT, nullptr);
	peer->Startup(MAX_CONNECTIONS, &sd, 1);
	peer->SetMaximumIncomingConnections(MAX_CONNECTIONS - 1);

	printf("Starting server...\n");

	while (1) {
		Packet *packet;
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()) {
			MessageID id = getPacketID(packet);

			switch (id) {
			case ID_NEW_INCOMING_CONNECTION:
				printf("New connection from [%s]\n", packet->systemAddress.ToString());
				break;
			default:
				printf("SOMETHING REVICEDC\n");
				break;
			}
		}
	}

	return 0;
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
