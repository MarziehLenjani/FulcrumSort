/*
 * transferMedium.hpp
 *
 *  Created on: Feb 12, 2020
 *      Author: ml2au
 */

#ifndef TRANSFERMEDIUM_HPP_
#define TRANSFERMEDIUM_HPP_
#include "types.hpp"
class interSubarrayMove;
class bank;
class layer;
class computSubArray;
class vault;
class transferMedium{
	//int ID;

	bool busy=false;
	interSubarrayMove* scheduledTransfer=NULL;


public:
	int widthInWORD;
		int latencyPerWidthInCycle;
	bool canTransfer();
	transferMedium(computSubArray* ownerALPU_t,
			bank* ownerBank_t,
			layer* ownerLayer_t,
			vault* ownerVault_t);
	void setBusy(bool busy) ;
	vault* ownerVault;
	bank * ownerBank;
	layer * ownerLayer;
	computSubArray * ownerALPU;
	//Debug utilities
	bool beingTracked=false;
	bool trackAndWait=false;
};



#endif /* TRANSFERMEDIUM_HPP_ */
