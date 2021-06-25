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
class dataPartitioning;
class stackedMemory;
class dataTransfer;
class bank;
class layer;
class computSubarray:public physicalComponent{
public:
	bool endOfReadData=false;
	bool subBlockLimitIsReached=false;
	FULCRU_WORD_TYPE RegA;
	FULCRU_WORD_TYPE RegB;
	FULCRU_WORD_TYPE SelfIndex;

	FULCRU_WORD_TYPE currentReadValue;
	FULCRU_WORD_TYPE currentWriteValue;


	LOCAL_ADDRESS_TYPE readStartAddress=0;
	LOCAL_ADDRESS_TYPE readEndAdddress=0;

	LOCAL_ADDRESS_TYPE writeStartAddress=0;
	LOCAL_ADDRESS_TYPE writeEndAdddress=0;

	LOCAL_ADDRESS_TYPE readAddressCounter=0;
	LOCAL_ADDRESS_TYPE writeAddressCounter=0;

	LOCAL_ADDRESS_TYPE readWaitCounter=0;
	LOCAL_ADDRESS_TYPE writeWaitCounter=0;
	FULCRU_WORD_TYPE maskForBucketExtaction=0;



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
	void setMaskForBucketIDExtraction(FULCRU_WORD_TYPE maskForBucketExtaction, FULCRU_WORD_TYPE numberOfShiftsForBucketIDExtraction);
};




#endif /* COMPUTSUBARRAY_HPP_ */
