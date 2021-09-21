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

#include "PhysicalComponent.hpp"
#include "Packet.hpp"
#include "Subarray.hpp"

class Bank:public PhysicalComponent{
public:
	std::vector<Subarray *> subarrayVector;

	std::queue <Packet<PlacementPacket>*>* upperLayerQ = nullptr;
	std::queue <Packet<PlacementPacket>*>* lowerLayerQ = nullptr;

	Bank(ID_TYPE l_id, PhysicalComponent * l_parent);
	~Bank();

	void initLayerQ();
};





#endif
