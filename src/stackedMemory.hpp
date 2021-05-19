/*
 * stackedMemory.hpp
 *
 *  Created on: May 16, 2021
 *      Author: marzieh
 */

#ifndef STACKEDMEMORY_HPP_
#define STACKEDMEMORY_HPP_


#include "physicalComponent.hpp"
#include <vector>
#include "layer.hpp"
#include "bank.hpp"
#include "computSubarray.hpp"

class stackedMemory:public physicalComponent{
public:
	std::vector<layer *> layerVector;
	std::vector<computSubarray *> computSubarrayVector; //no need to delete this, this is just a copy of pointers
	bool endOfProcessing=false;
	stackedMemory(ID_TYPE l_id, configAndStats * l_confObj, physicalComponent * l_firstDimOwner, physicalComponent * l_secondDimOwner, physicalComponent * l_thirdDimOwner);
	~stackedMemory();
	void runOneSubClokCycle();
	ID_TYPE numLayers;
	ID_TYPE numBanksPerLayer;
	ID_TYPE numSubArraysPerBank;


	ID_TYPE totNumComputeSubarray;
};



#endif /* STACKEDMEMORY_HPP_ */
