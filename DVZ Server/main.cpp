#include <stdio.h>
#include <iostream>
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"

#define MAX_CONNECTIONS 10
#define SERVER_PORT 60000

using namespace SLNet;

MessageID GetPacketIdent(SLNet::Packet *p);

int main(void) {

	//Gives me one instance of a peer
	RakPeerInterface *peer = RakPeerInterface::GetInstance();
	peer->Startup(MAX_CONNECTIONS, &SLNet::SocketDescriptor(SERVER_PORT, 0), 1);
	peer->SetMaximumIncomingConnections(MAX_CONNECTIONS);
	peer->GetLocalIP(0);
	while (1) {
		SLNet::Packet *packet;
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()) {
			MessageID m = GetPacketIdent(packet);

			if (m == ID_NEW_INCOMING_CONNECTION) {
				printf("New Connection\n");
			}
			else if (m == ID_CONNECTION_REQUEST_ACCEPTED) {
				printf("Connected\n");
			}

		}
	}

	SLNet::RakPeerInterface::DestroyInstance(peer);
}

//Sometimes a packet will send a timestamp before its messageID
MessageID GetPacketIdent(SLNet::Packet *p) {
	if ((unsigned char)p->data[0] == ID_TIMESTAMP) {
		return (MessageID)p->data[sizeof(MessageID) + sizeof(SLNet::Time)];
	}
	else {
		return p->data[0];
	}
}