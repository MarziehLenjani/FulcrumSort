/*
 * bank.hpp
 *
 *  Created on: Feb 12, 2020
 *      Author: ml2au
 */

#ifndef BANK_HPP_
#define BANK_HPP_
#include <cstddef>

#include "types.hpp"

#include <queue>
#include "PhysicalComponent.hpp"
#include "Packet.hpp"
#include "Subarray.hpp"

class Bank:public PhysicalComponent{
private:
	std::queue <Packet<PlacementPacket>*>* getNextBankQ_dragonfly(ID_TYPE dstBankAddr);

public:
	ID_TYPE selfIndex = 0;

	std::vector<Subarray *> subarrayVector;

	//Histogram is stored in bank instead of subarray to reduce simulation time.
	//The timing is calculated assuming histograms are stored on subarray.
	HIST_ELEM_TYPE histogram[G_NUM_HIST_ELEMS];

	std::queue <Packet<PlacementPacket>* > packetQ;
	std::queue <Packet<PlacementPacket>*>* upperLayerQ = nullptr;
	std::queue <Packet<PlacementPacket>*>* lowerLayerQ = nullptr;

	Bank(ID_TYPE l_id, PhysicalComponent * l_parent);
	~Bank();

	void prePlacementProducePackets();
	void prePlacementConsumePacketsOneCycle();

	void initLayerQ();
	void runLocalHist();
};





#endif
