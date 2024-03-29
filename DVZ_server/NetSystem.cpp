#include "EntityAdmin.h"
#include "Components.h"
#include "SingletonComponents.h"
#include "AABB.h"

#include "NetSystem.h"
#include "MessageIdentifiers.h"
#include "RakNetTypes.h"
#include "GamePacketID.h"
#include "BitStream.h"
#include "NetUtil.h"

#include "VoxelUtil.h"
#include "ChunkManager.h"
#include "RLEncoder.h"

#include "zlib.h"

using namespace System;
using namespace SLNet;

void incomming_connection_packet(RakPeerInterface *peer, Packet *packet, entt::registry &registry) {
	using namespace Component;
	printf("New connection from [%s]\n", packet->systemAddress.ToString());

	entt::entity player = registry.create();
	registry.assign<NetClient>(player, packet->guid);
	registry.assign<VoxelCollision>(player, Physics::AABB(glm::vec3(-.3f, -1.5f, -.3f), glm::vec3(.3f, .3f, .3f)));
	registry.assign<Position>(player, glm::vec3(0, 20, 0));
	registry.assign<Rotation>(player, glm::vec3(0, 0, 0));
	//registry.assign<Velocity>(player, glm::vec3(0, 0, 0));
	//registry.assign<Acceleration>(player, glm::vec3(0));
	registry.assign<Dir>(player, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
	registry.assign<Input>(player);
	registry.assign<AFK>(player);
	registry.assign<ChunkBoundry>(player, glm::vec3(0, 0, 0));
	registry.assign<ClientChunkSnapshot>(player);
	
	BitStream out;
	out.Write((MessageID)ID_CLIENT_NET_ID);
	out.Write(player);
	peer->Send(&out, PacketPriority::IMMEDIATE_PRIORITY, PacketReliability::RELIABLE, 0, packet->systemAddress, false);
}

void disconnection_packet(Packet *packet, entt::registry &registry) {
	printf("CLIENT DISCONNECTED\n");
	auto &map = registry.ctx<ConnectedClientMap>();
	
	if (auto iter = map.find(packet->guid); iter != map.end()) {
		entt::entity playerID = iter->second;
		registry.destroy(playerID);		//destroy should call a signal handler to manager ConnectedClientMap
	}
}

void client_input_packet(Packet *packet, entt::registry &registry) {
	using namespace Component;
	auto &map = registry.ctx<ConnectedClientMap>();
	auto iter = map.find(packet->guid);

	if (iter == map.end()) {
		return;
	}

	entt::entity clientEntity = map[packet->guid];

	//auto &pos = registry.get<PositionComponent>(clientEntity);
	auto &rot = registry.get<Rotation>(clientEntity);
	auto &input = registry.get<Input>(clientEntity);

	BitStream in(packet->data, packet->length, false);
	in.IgnoreBytes(sizeof(MessageID));
	//in.Read(pos);
	in.Read(rot);
	input.up = in.ReadBit();
	input.down = in.ReadBit();
	input.left = in.ReadBit();
	input.right = in.ReadBit();
	input.space = in.ReadBit();
	input.shift = in.ReadBit();
	input.ctrl = in.ReadBit();
}

void client_move_packet(Packet *packet, entt::registry &registry) {
	using namespace Component;
	auto &map = registry.ctx<ConnectedClientMap>();
	auto iter = map.find(packet->guid);

	if (iter == map.end()) {
		return;
	}

	entt::entity clientEntity = map[packet->guid];

	auto &pos = registry.get<Position>(clientEntity);
	auto &rot = registry.get<Rotation>(clientEntity);

	BitStream in(packet->data, packet->length, false);
	in.IgnoreBytes(sizeof(MessageID));
	in.Read(pos);
	in.Read(rot);
}

void block_place_packet(Packet *packet, EntityAdmin &admin) {
	
	auto peer = admin.getPeer();

	BitStream in(packet->data, packet->length, false);
	in.IgnoreBytes(sizeof(MessageID));
	
	
	glm::i32vec3 blockPos;
	Voxel::Block block;
	in.Read(blockPos);
	in.Read(block);

	auto &manager = admin.getChunkManager();
	manager.setBlock(blockPos.x, blockPos.y, blockPos.z, block);
}

void System::net_update(EntityAdmin &admin, float delta) {
	entt::registry &registry = admin.registry;
	auto peer = admin.getPeer();

	Packet *packet = nullptr;
	for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()) {
		MessageID id = getPacketID(packet);

		switch (id) {
		case ID_NEW_INCOMING_CONNECTION: 
			incomming_connection_packet(peer, packet, registry);
			break;
		case ID_DISCONNECTION_NOTIFICATION: 
			disconnection_packet(packet, registry);
			break;
		case ID_CLIENT_INPUT:
			//client_input_packet(packet, registry);
			break;
		case ID_CLIENT_MOVE:
			client_move_packet(packet, registry);
			break;
		case ID_BLOCK_PLACE:
			block_place_packet(packet, admin);
			break;
		default:
			printf("Uncaught packet [%d]\n", id);
			break;
		}
	}
}

void System::net_disconnect(EntityAdmin &admin, float delta){
	auto &registry = admin.registry;
	auto peer = admin.getPeer();
	auto &buffer = registry.ctx<CloseConnectionBuffer>();

	if (buffer.size()) {
		auto guid = buffer.back();
		peer->CloseConnection(guid, true);
		buffer.pop_back();
	}
}

void System::net_broadcast(EntityAdmin &admin, float delta) {
	using namespace Component;
	
	auto &registry = admin.registry;
	auto *peer = admin.getPeer();
	auto &map = registry.ctx<ConnectedClientMap>();

	auto view = registry.view<NetClient, Position, Rotation>();

	for (entt::entity e : view) {
		NetClient client = view.get<NetClient>(e);
		Position pos = view.get<Position>(e);
		Rotation rot = view.get<Rotation>(e);

		BitStream stream;
		stream.Write((MessageID)ID_PLAYER_MOVE);
		stream.Write(e);
		stream.Write(pos);
		stream.Write(rot);
		peer->Send(&stream, PacketPriority::MEDIUM_PRIORITY, PacketReliability::UNRELIABLE_SEQUENCED, 0, client.guid, true);
	}
}

std::array<uint8, 10 * sizeof(uint8) * 1024> buffer;

void System::net_voxel(EntityAdmin &admin, float delta) {
	using namespace Component;
	auto &registry = admin.registry;
	auto &manager = admin.getChunkManager();
	auto *peer = admin.getPeer();

	std::map<const Voxel::Chunk*, std::vector<NetClient>> chunkFullUpdateMap;
	std::map<const Voxel::Chunk*, std::map<int, std::vector<NetClient>>> blockUpdateMap;


	auto view = registry.view<NetClient, ChunkBoundry, ClientChunkSnapshot>();
	view.each([&manager, &chunkFullUpdateMap, &blockUpdateMap](auto &net, auto &bound, auto &snapshot) {
		
		//todo only call this when player moves boundries
		//Voxel::setSnapshotCenter(bound, snapshot);



		auto RADIUS = ClientChunkSnapshot::VIEW_RADIUS;
		for (int z = -RADIUS; z <= RADIUS; z++) {
			for (int x = -RADIUS; x <= RADIUS; x++) {
				const auto chunk_pos = bound + glm::i32vec3(x, 0, z);
				if (!manager.isInbound(chunk_pos.x, chunk_pos.z)) {
					continue;
				}

				auto &modcount_snapshot = snapshot.chunkSnapshots[Util::zorder_hash(chunk_pos.x, 0, chunk_pos.z)];

				//auto &stamp = Voxel::getChunkModCounter(chunk_pos, snapshot);

				const auto &chunk = manager.getChunk(chunk_pos.x, chunk_pos.z);
				const auto modcount = chunk.getModCount();
				const auto mod_diff = modcount - modcount_snapshot;

				//if there is a mod_diff need to send client chunk data
				if (mod_diff > 0) {
					if (chunk.isChunkModHistoryComplete(mod_diff)) {
						//printf("BlockDelta: [%ld]\n", net);
						blockUpdateMap[&chunk][mod_diff].push_back(net);
					}
					else {
						//printf("ChunkEncoding: [%ld]\n", net);
						chunkFullUpdateMap[&chunk].push_back(net);
					}
					modcount_snapshot = modcount;
				}
			}
		}
		
	});

	//send block deltas packet per chunk, per num_block packets
	for (const auto &chunk_map_pair : blockUpdateMap) {
		const auto &chunk = *chunk_map_pair.first;
		const auto &map = chunk_map_pair.second;
		
		for (const auto &pair : map) {
			const auto mod_diff = pair.first;
			const auto &clients = pair.second;

			BitStream stream;
			stream.Write((MessageID)ID_BLOCK_PLACE);
			stream.Write((int8)chunk.cx);
			stream.Write((int8)chunk.cz);
			stream.Write((uint8)mod_diff);
			
			//write all the block deltas
			for (int i = 0; i < mod_diff; i++) {
				const auto &mod_event = chunk.getModBuffer().read(i);
				stream.Write(mod_event.index);
				stream.Write(mod_event.block);
			}

			//printf("Block delta packet size: [%zu]\n", stream.GetNumberOfBytesUsed());

			for (const auto &client : clients) {
				SLNet::RakNetGUID guid = client.guid;
				peer->Send(&stream, PacketPriority::LOW_PRIORITY, PacketReliability::RELIABLE, 0, guid, false);
			}

		}
	}

	for (auto &pair : chunkFullUpdateMap) {
		//printf("%d %d %d\n", chunk.cx, chunk.cy, chunk.cz);
		const auto &chunk = *pair.first;
		const auto &clients = pair.second;

		unsigned long buffer_size = sizeof(buffer);

		const auto rl_chunk = Voxel::encode_chunk(chunk);
		const uint32 inflated_bytes = (unsigned int)(sizeof(rl_chunk[0]) * rl_chunk.size());
		unsigned char * ptr = (unsigned char*)rl_chunk.data();

		auto status = compress2(buffer.data(), &buffer_size, ptr, inflated_bytes, Z_BEST_COMPRESSION);
		const uint32 deflated_bytes = buffer_size;
		//printf("ZLIB | inflated [%ld] deflated: [%ld]\n", num_bytes, buffer_size);


		BitStream stream;
		stream.Write((MessageID)ID_RL_CHUNK_DATA);
		stream.Write((int8)chunk.cx);
		stream.Write((int8)chunk.cz);
		stream.Write(inflated_bytes);						//uncompressed num_bytes;
		stream.Write(deflated_bytes);
		stream.WriteAlignedBytes(buffer.data(), deflated_bytes);
		//printf("[%ld]\n", stream.GetNumberOfBytesUsed());

		for (auto &net : clients) {
			SLNet::RakNetGUID guid = net.guid;
			peer->Send(&stream, PacketPriority::LOW_PRIORITY, PacketReliability::RELIABLE, 0, guid, false);
		}

		
	}
}
