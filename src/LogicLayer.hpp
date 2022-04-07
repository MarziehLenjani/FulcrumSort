#pragma once

#include <vector>
#include <queue>
#include "types.hpp"
#include "globals.hpp"
#include "PhysicalComponent.hpp"
#include "Packet.hpp"

class HMCLink;

class LogicLayer : public PhysicalComponent{
public:
	std::queue <Packet<PlacementPacket>*> packetQs[G_NUM_BANKS_PER_LAYER];
	HMCLink* hmcLinks[G_NUM_STACKS_PER_DEVICE];

	LogicLayer(PhysicalComponent * parent);
	~LogicLayer();

	void initLinks();

	void runOneCycle();
};
