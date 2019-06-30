#include <stdio.h>
#include <iostream>
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"

#define MAX_CLIENTS 100
#define SERVER_PORT 60000

using namespace SLNet;
using namespace std;

int main(void) {
	RakPeerInterface *peer = RakPeerInterface::GetInstance();
	Packet *packet;

	bool isServer = false;

	printf("Are you a server or a client: ");
	char ans;
	cin >> ans;

	if (ans == 'c' || ans == 'C') {
		SocketDescriptor sd;
		peer->Startup(1, &sd, 1);
		isServer = false;

	}
	else {
		SocketDescriptor sd(SERVER_PORT, 0);
		peer->Startup(MAX_CLIENTS, &sd, 1);
		isServer = true;
	}

	if (isServer) {
		printf("Starting Server...\n");
		peer->SetMaximumIncomingConnections(MAX_CLIENTS);
	}
	else {
		printf("Enter IP or hit enter for 127.0.0.1: ");
		char str[100];

		cin >> str;

		printf("Connecting to [%s]...", str);
		peer->Connect(str, SERVER_PORT, 0, 0);

	}

	while (1)
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				printf("Another client has disconnected.\n");
				break;
			case ID_REMOTE_CONNECTION_LOST:
				printf("Another client has lost the connection.\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				printf("Another client has connected.\n");
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("Our connection request has been accepted.\n");
				break;
			case ID_NEW_INCOMING_CONNECTION:
				printf("A connection is incoming.\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				if (isServer) {
					printf("A client has disconnected.\n");
				}
				else {
					printf("We have been disconnected.\n");
				}
				break;
			case ID_CONNECTION_LOST:
				if (isServer) {
					printf("A client lost the connection.\n");
				}
				else {
					printf("Connection lost.\n");
				}
				break;
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
		}
	}

	RakPeerInterface::DestroyInstance(peer);
}