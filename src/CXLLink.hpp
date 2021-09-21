#pragma once

#include <cstdlib>
#include <queue>
#include <cassert>
#include "types.hpp"
#include "globals.hpp"
#include "Packet.hpp"
#include "Device.hpp"
#include "Stack.hpp"

class CXLLink {
	double currPacketLim = 0;

	std::queue <Packet<PlacementPacket>* > inQ;
	Device* outDevice = nullptr;

public:
	CXLLink(Device* outDevice) : outDevice(outDevice) {
		assert(G_CXL_LINK_PACKET_PER_CLOCK >= 1);	//Assumes send one or more packet per clock cycle
		//TODO: fix if multiple clock cycles are needed to send a packet
	}

	void send(Packet<PlacementPacket>* packet) {
		inQ.push(packet);
	}

	void clock(){
		while(!inQ.empty() && (currPacketLim >= 1)){
			auto pkt = inQ.front();
			inQ.pop();
			u64 dstBank = extractBankId(pkt->dstSubAddr);
			u64 dstStack = extractStackId(pkt->dstSubAddr);

			outDevice->stackVector[dstStack]->layerVector[0]->bankVector[dstBank]->subarrayVector[0]->incomingPackets.push(pkt);
			currPacketLim--;
		}
		if(!inQ.empty()){
			currPacketLim += G_CXL_LINK_PACKET_PER_CLOCK;
		}
		else{
			currPacketLim = 0;
		}
	}
};
