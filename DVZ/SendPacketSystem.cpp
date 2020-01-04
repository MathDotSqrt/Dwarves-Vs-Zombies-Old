#include "SendPacketSystem.h"
#include "Components.h"
#include "Engine.h"

#include "RakPeerInterface.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "GamePacketID.h"

SendPacketSystem::SendPacketSystem(int priority) : System(priority) {

}

SendPacketSystem::~SendPacketSystem() {

}

void SendPacketSystem::addedToEngine(Engine *engine) {

}
void SendPacketSystem::removedFromEngine(Engine *engine) {

}
void SendPacketSystem::update(Engine *engine, float delta) {
	auto view = engine->view<PlayerComponent, NetworkComponent>();
	if (view.size()) {
		entt::entity playerID = *view.begin();
		auto &pos = engine->get<PositionComponent>(playerID);
		auto &rot = engine->get<RotationComponent>(playerID);
		auto &input = engine->get<InputComponent>(playerID);
		auto &net = engine->get<NetworkComponent>(playerID);
		SLNet::BitStream out;
		out.Write((SLNet::MessageID)ID_CLIENT_INPUT);
		//out.Write(net.netID);
		//out.Write(pos);
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
		

		auto peer = engine->getPeer();
		peer->Send(&out, LOW_PRIORITY, PacketReliability::UNRELIABLE, 0, SLNet::UNASSIGNED_RAKNET_GUID, true);
	}
}