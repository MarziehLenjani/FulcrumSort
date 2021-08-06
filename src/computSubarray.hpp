/*
 * computSubarray.hpp
 *
 *  Created on: May 16, 2021
 *      Author: marzieh
 */

#ifndef COMPUTSUBARRAY_HPP_
#define COMPUTSUBARRAY_HPP_

#include <queue>
#include "physicalComponent.hpp"
#include "configAndStats.hpp"
#include "memoryArray.hpp"

class stackedMemory;
class dataTransfer;
class bank;
class layer;
class computSubarray:public physicalComponent{
public:

	bool subBlockLimitIsReached=false;
	FULCRU_WORD_TYPE RegA;
	FULCRU_WORD_TYPE RegB;
	FULCRU_WORD_TYPE SelfIndex;

	LOCAL_ADDRESS_TYPE readStartAddress=0;
	LOCAL_ADDRESS_TYPE readEndAdddress=0;

	LOCAL_ADDRESS_TYPE writeStartAddress=0;
	//LOCAL_ADDRESS_TYPE writeEndAdddress=0;

	LOCAL_ADDRESS_TYPE readAddressCounter=0;
	LOCAL_ADDRESS_TYPE writeAddressCounter=0;

	LOCAL_ADDRESS_TYPE readWaitCounter=0;
	LOCAL_ADDRESS_TYPE writeWaitCounter=0;

	u64 nwrdsInRow;
	u64 rowCycleInSubClockCycle;

	bool readEnded = false;

	// TODO: Place the hist array inside memory object
	Histogram hist[NUM_BINS];

	// TODO: Move these two metadata inside memory object
	bool isSumAvailable;
	FULCRU_WORD_TYPE prefixSumOfLastSubarray;


	std::queue <dataTransfer*> incomingPackets;

	void initialize(LOCAL_ADDRESS_TYPE addressOfTheReadStartAddress,LOCAL_ADDRESS_TYPE addressOfTheReadEndAdddress, LOCAL_ADDRESS_TYPE addressOfTheWriteStartAddress,
			FULCRU_WORD_TYPE t_RegA, FULCRU_WORD_TYPE t_RegB, FULCRU_WORD_TYPE t_SelfIndex ); //

	computSubarray(ID_TYPE l_id, configAndStats * l_confObj, physicalComponent * l_firstDimOwner, physicalComponent * l_secondDimOwner, physicalComponent * l_thirdDimOwner);
	~computSubarray( );
	void runOneSubClokCycle();
	memoryArray * memoryArrayObj;
	stackedMemory * stackedMemoryObj;
	bank* bankObj;
	layer * layerObj;
	bool isDestinationForDataTransfer(dataTransfer* packet);

	computSubarray* getNextComputeSubArray(dataTransfer * pckt);

	//--------------To be implemented functions
	void openANewSubBucket();
	void sealAllSubBuckets();
	void setMaskForBucketIDExtraction(FULCRU_WORD_TYPE maskForBucketExtaction, FULCRU_WORD_TYPE numberOfShiftsForBucketIDExtraction);

	FULCRU_WORD_TYPE extractBits(FULCRU_WORD_TYPE val, u32 highBitPos, u32 lowBitPos);

	bool isProceedRead();
	bool isProceedWrite();

	void initializeHistGenGlobal();
	void runHistGenGlobalOneClockCycle();

	void initializePrefixSumWithinArrayGlobal();
	void runPrefixSumWithinArrayGlobalOneClockCycle();

	void initializePrefixSumNextArrayGlobal();
	void runPrefixSumNextArrayGlobalOneClockCycle();

	void initializePlacementGlobal();
	void runPlacementGlobalOneClockCycle();
};




#endif /* COMPUTSUBARRAY_HPP_ */
