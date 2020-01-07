#pragma once
#include "MessageIdentifiers.h"
#include "RakNetTypes.h"
#include "EntityAdmin.h"

SLNet::MessageID getPacketID(SLNet::Packet *packet);
void closeClientConnection(EntityAdmin &admin, SLNet::RakNetGUID guid);