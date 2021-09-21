/*
 * stackedMemory.hpp
 *
 *  Created on: May 16, 2021
 *      Author: marzieh
 */

#ifndef STACKEDMEMORY_HPP_
#define STACKEDMEMORY_HPP_

#include <vector>

#include "Bank.hpp"
#include "Layer.hpp"
#include "PhysicalComponent.hpp"
#include "Subarray.hpp"

class dataPartitioning;
class LogicLayer;

class Stack:public PhysicalComponent{
public:
	std::vector<Layer *> layerVector;
	LogicLayer* logicLayer = nullptr;

	//std::vector<Subarray *> computSubarrayVector; //no need to delete this, this is just a copy of pointers
	bool checkIfProcessHasEnd();
	bool checkIfSubBlockLimitIsReached();
	Stack(ID_TYPE l_id, PhysicalComponent * l_parent);
	~Stack();

	//void incrementSubarraysSelfindexes();
//	void sealAllSubBuckets();
//	void openANewSubBucket();
//	void setMaskForBucketIDExtraction(FULCRU_WORD_TYPE maskForBucketExtaction,FULCRU_WORD_TYPE numberOfShiftsForBucketIDExtraction);
//
//	void initializeHistGenGlobal();
//	void runHistGenGlobalOneClockCycle();
//
//	void initializePrefixSumWithinArrayGlobal();
//	void runPrefixSumWithinArrayGlobalOneClockCycle();
//
//	void initializePrefixSumNextArrayGlobal();
//	void runPrefixSumNextArrayGlobalOneClockCycle();
//
//	void initializePlacementGlobal();
//	void runPlacementGlobalOneClockCycle();
};



#endif /* STACKEDMEMORY_HPP_ */
