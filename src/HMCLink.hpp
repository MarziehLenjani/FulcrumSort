#pragma once

#include <cstdlib>
#include <queue>
#include <cassert>
#include "types.hpp"
#include "globals.hpp"
#include "Packet.hpp"
#include "Stack.hpp"

class HMCLink {
	double currPacketLim = 0;

	std::queue <Packet<PlacementPacket>* > inQ;
	Stack* outStack = nullptr;

public:
	HMCLink(Stack* outStack) : outStack(outStack) {
		assert(G_HMC_LINK_PACKET_PER_CLOCK >= 1);	//Assumes send one or more packet per clock cycle
		//TODO: fix if multiple clock cycles are needed to send a packet
	}

	void send(Packet<PlacementPacket>* packet) {
		inQ.push(packet);
	}

	void clock(){
		while(!inQ.empty() && (currPacketLim >= 1)){
			auto pkt = inQ.front();
			inQ.pop();
			u64 dstBank = extractBankId(pkt->dstBankAddr);
			outStack->layerVector[0]->bankVector[dstBank]->packetQ.push(pkt);
			currPacketLim--;
		}
		if(!inQ.empty()){
			currPacketLim += G_HMC_LINK_PACKET_PER_CLOCK;
		}
		else{
			currPacketLim = 0;
		}
	}
};
