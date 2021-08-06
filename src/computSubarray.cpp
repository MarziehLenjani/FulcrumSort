/*
 * computSubarray.cpp
 *
 *  Created on: May 16, 2021
 *      Author: marzieh
 */

#include "types.hpp"
#include "computSubarray.hpp"
#include "layer.hpp"
#include "bank.hpp"
#include "stackedMemory.hpp"
#include "dataPartitioning.hpp"
#include "dataTransfer.hpp"
#include <cstring>


computSubarray::computSubarray(ID_TYPE l_id, configAndStats * l_confObj, physicalComponent * l_firstDimOwner, physicalComponent * l_secondDimOwner, physicalComponent * l_thirdDimOwner)
:physicalComponent(l_id, l_confObj, l_firstDimOwner, l_secondDimOwner, l_thirdDimOwner){
	//TODO: do initializations specific for the subarray class here
	memoryArrayObj= new memoryArray(0, l_confObj, this, NULL, NULL);
	bankObj = (bank*)firstDimOwner;
	layerObj = (layer*)(bankObj->firstDimOwner);
	stackedMemoryObj = (stackedMemory*)(layerObj->firstDimOwner);
}

computSubarray::~computSubarray(){
	//TODO: make sure it calls the decosntuctor
	delete memoryArrayObj;

}
void computSubarray::runOneSubClokCycle(){
	//TODO: implement this function
	bool proceedRead=false;
	bool proceedWrite=false;
	CONF_NUMBER_OF_WORDS_IN_A_ROW_TYPE nwrdsInRow=confObj->getConfig<CONF_NUMBER_OF_WORDS_IN_A_ROW_TYPE>(CONF_NUMBER_OF_WORDS_IN_A_ROW_NAME);
	CONF_ROW_CYCLE_IN_SUB_CLOCK_CYCLE_TYPE rowCycleInSubClockCycle= confObj->getConfig<CONF_ROW_CYCLE_IN_SUB_CLOCK_CYCLE_TYPE>(CONF_ROW_CYCLE_IN_SUB_CLOCK_CYCLE_NAME);
    if(readAddressCounter%nwrdsInRow !=0 ){
    	proceedRead=true;
    }else{
    	if(writeWaitCounter==0){
    		readWaitCounter++;
    	}
    	if(readWaitCounter==rowCycleInSubClockCycle){
			readWaitCounter=0;
			proceedRead=true;
		}
    }
    if(proceedRead){

    	readAddressCounter+=sizeof(FULCRU_WORD_TYPE);
    	//TODO: implement what happens after reading here
    	LOCAL_ADDRESS_TYPE tempValueForRead;
    	memoryArrayObj->memory_read(readAddressCounter, sizeof(FULCRU_WORD_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
    	LOCAL_ADDRESS_TYPE destinationId=(tempValueForRead & RegA) >>RegB; //RegA is initialized with maskForBucketExtaction
    	dataTransfer* tmpPacket=new dataTransfer(sizeof(tempValueForRead),destinationId,(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &tempValueForRead);
    	incomingPackets.push(tmpPacket);

    }
//-----------------------------
    if(writeAddressCounter%nwrdsInRow !=0 ){
    	proceedWrite=true;
    }else{
    	if(readWaitCounter==0){
    		writeWaitCounter++;
    	}
    	if(writeWaitCounter==rowCycleInSubClockCycle){
    		writeWaitCounter=0;
			proceedWrite=true;
		}
    }
    if(proceedWrite){
    	//TODO: get the packet
    	//write it in the subarray
    	//delete the packet
    	if(incomingPackets.size()>0){
    		dataTransfer* tmpPacket=incomingPackets.front();
    		incomingPackets.pop();

			if(isDestinationForDataTransfer(tmpPacket)){
				//packet has arrived
				writeAddressCounter += sizeof(FULCRU_WORD_TYPE);
				LOCAL_ADDRESS_TYPE tempValueForWrite=*(tmpPacket->payload);

				memoryArrayObj->memory_write(writeAddressCounter, sizeof(FULCRU_WORD_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForWrite));

				delete tmpPacket;
			}else{
				auto nextSubArray=getNextComputeSubArray(tmpPacket);
				nextSubArray->incomingPackets.push(tmpPacket);

			}
    	}
    }
    //-------------------------


}
void computSubarray::initialize(LOCAL_ADDRESS_TYPE addressOfTheReadStartAddress,
		LOCAL_ADDRESS_TYPE addressOfTheReadEndAdddress,
		LOCAL_ADDRESS_TYPE addressOfTheWriteStartAddress,
		FULCRU_WORD_TYPE t_RegA, FULCRU_WORD_TYPE t_RegB, FULCRU_WORD_TYPE t_SelfIndex){

		LOCAL_ADDRESS_TYPE tempValueForRead;

		memoryArrayObj->memory_read(addressOfTheReadStartAddress, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
		readStartAddress=stackedMemoryObj->dataPartitioningObj->metadatatoLocalAddress (tempValueForRead);


		memoryArrayObj->memory_read(addressOfTheReadEndAdddress, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
		readEndAdddress=stackedMemoryObj->dataPartitioningObj->metadatatoLocalAddress (tempValueForRead);

		memoryArrayObj->memory_read(addressOfTheWriteStartAddress, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
		writeStartAddress=stackedMemoryObj->dataPartitioningObj->metadatatoLocalAddress (tempValueForRead);

		readAddressCounter=readStartAddress;
		writeAddressCounter=writeStartAddress;

		RegA=t_RegA;
		RegB=t_RegB;
		SelfIndex=t_SelfIndex;

		 return;
}

bool computSubarray::isDestinationForDataTransfer(dataTransfer* packet){
	return packet->destinationID == SelfIndex;
}

void computSubarray::setMaskForBucketIDExtraction(FULCRU_WORD_TYPE maskForBucketExtaction, FULCRU_WORD_TYPE numberOfShiftsForBucketIDExtraction){
	RegA=maskForBucketExtaction;
	RegB=numberOfShiftsForBucketIDExtraction;
}
//TODO:To be comepleted nd tested functions

computSubarray* computSubarray::getNextComputeSubArray(dataTransfer * pckt){

	computSubarray* nextSubArray=NULL;
	layer * nextLayer=NULL;
	// we have ring per two vaults, where the lowest layer has ID of 0, every two banks are connected to the same vault, so every four banks are part of a ring

	if(bankObj->id%4<2){ //even vaults send upwards
		if(layerObj->id<(stackedMemoryObj->numLayers-1) ){
			nextLayer=stackedMemoryObj->layerVector[layerObj->id+1];
		}else{
			nextLayer=stackedMemoryObj->layerVector[layerObj->id-1];
		}
	}else{ //odd vaults send  downward
		if(layerObj->id>0 ){
			nextLayer=stackedMemoryObj->layerVector[layerObj->id-1];
		}else{
			nextLayer=stackedMemoryObj->layerVector[layerObj->id+1];
		}
	}
	bank* nextBank=NULL;

	if(bankObj->id!=layerObj->nBankPerLayer-1){
		nextBank=layerObj->bankVector[bankObj->id+1];
	}else{
		nextBank=layerObj->bankVector[0];
	}
	//we have a ring per layer, where the lowest subarray has ID of 0, even banks direct packets upward
	bool sameLayer=true;
	if(id==0){ //subarray zero always decides if the same layer
		FULCRU_WORD_TYPE firstIndexInLayer=layerObj->bankVector[0]->computSubarrayVector[0]->SelfIndex;
		FULCRU_WORD_TYPE lastIndexInLayer=layerObj->bankVector[layerObj->nBankPerLayer-1]->computSubarrayVector[bankObj->nSubPerBank-1]->SelfIndex;
		if(firstIndexInLayer<lastIndexInLayer){
			if(pckt->destinationID>=firstIndexInLayer && pckt->destinationID<=lastIndexInLayer){
				sameLayer=true;
			}else{
				sameLayer=false;


			}
		}else{

			if(pckt->destinationID>=firstIndexInLayer || pckt->destinationID<=lastIndexInLayer){
					sameLayer=true;
				}else{
					sameLayer=false;

				}
		}


	}

	if(sameLayer){
		if(bankObj->id%2==0){ //even bank, directing upward
			if(id!=(bankObj->nSubPerBank-1)){
				nextSubArray=bankObj->computSubarrayVector[id+1];
			}else{

				nextSubArray=nextBank->computSubarrayVector[bankObj->nSubPerBank-1];

			}
		}else{
			if(id!=0){ // odd bank directing downward
				nextSubArray=bankObj->computSubarrayVector[id-1];
			}else{
				nextSubArray=nextBank->computSubarrayVector[0];
			}

		}
	}else{
		nextSubArray=nextLayer->bankVector[bankObj->id]->computSubarrayVector[0];
	}


	return nextSubArray;
}

//--------------To be implemented functions
void computSubarray::openANewSubBucket(){
	// TODO: implement this function
	//This function
}

void computSubarray::sealAllSubBuckets(){
	// TODO: implement this function
	//This function
}

FULCRU_WORD_TYPE computSubarray::extractBits(FULCRU_WORD_TYPE val, u32 highBitPos, u32 lowBitPos){
	// Make sure parameters are okay
	assert(highBitPos < (sizeof(FULCRU_WORD_TYPE) * 8));
	assert(lowBitPos < (sizeof(FULCRU_WORD_TYPE) * 8));
	assert(lowBitPos <= highBitPos);

	u32 width = highBitPos - lowBitPos;
	u64 mask = (1UL << width) - 1;

	return (val >> lowBitPos) & mask;
}

void computSubarray::initializeHistGenGlobal(){
	memset(hist, 0, NUM_BINS * sizeof(Histogram));
	readEnded = false;

	LOCAL_ADDRESS_TYPE tempValueForRead;

	memoryArrayObj->memory_read(ADDRESS_OF_START_ADDRESS, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
	readStartAddress = stackedMemoryObj->dataPartitioningObj->metadatatoLocalAddress (tempValueForRead);


	memoryArrayObj->memory_read(ADDRESS_OF_END_ADDRESS, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
	readEndAdddress = stackedMemoryObj->dataPartitioningObj->metadatatoLocalAddress (tempValueForRead);

	//writeStartAddress = readEndAdddress;	// For now, write starts just after the read portion

	readAddressCounter = readStartAddress;
	//writeAddressCounter = writeStartAddress;

	nwrdsInRow=confObj->getConfig<CONF_NUMBER_OF_WORDS_IN_A_ROW_TYPE>(CONF_NUMBER_OF_WORDS_IN_A_ROW_NAME);
	rowCycleInSubClockCycle= confObj->getConfig<CONF_ROW_CYCLE_IN_SUB_CLOCK_CYCLE_TYPE>(CONF_ROW_CYCLE_IN_SUB_CLOCK_CYCLE_NAME);
}

bool computSubarray::isProceedRead(){
	if(readEnded){
		return false;
	}
	bool proceedRead=false;
	if(readAddressCounter%nwrdsInRow !=0 ){
		proceedRead=true;
	}else{
		if(writeWaitCounter==0){
			readWaitCounter++;
		}
		if(readWaitCounter==rowCycleInSubClockCycle){
			readWaitCounter=0;
			proceedRead=true;
		}
	}
	return proceedRead;
}

bool computSubarray::isProceedWrite(){
	bool proceedWrite = false;
	if(writeAddressCounter%nwrdsInRow !=0 ){
		proceedWrite=true;
	}else{
		if(readWaitCounter==0){
			writeWaitCounter++;
		}
		if(writeWaitCounter==rowCycleInSubClockCycle){
			writeWaitCounter=0;
			proceedWrite=true;
		}
	}
	return proceedWrite;
}

void computSubarray::runHistGenGlobalOneClockCycle(){
	if(isProceedRead()){
		LOCAL_ADDRESS_TYPE currentReadValue;
		memoryArrayObj->memory_read(readAddressCounter, sizeof(FULCRU_WORD_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(currentReadValue));

		FULCRU_WORD_TYPE dstSubForBin = extractBits(currentReadValue, stackedMemoryObj->logTotSubarray - 1, 0);
		FULCRU_WORD_TYPE bin = extractBits(currentReadValue, stackedMemoryObj->logTotSubarray + LOG_NUM_BINS, stackedMemoryObj->logTotSubarray);

		dataTransfer* tmpPacket=new dataTransfer(sizeof(FULCRU_WORD_TYPE), dstSubForBin, (READ_DATA_TYPE_IN_MEMORY_ARRAY*) &bin);
		incomingPackets.push(tmpPacket);

		readAddressCounter += sizeof(FULCRU_WORD_TYPE);
		if(readAddressCounter == readEndAdddress){
			readEnded = true;
		}
	}

	if(isProceedWrite()){
		//TODO: get the packet
		//write it in the subarray
		//delete the packet
		if(incomingPackets.size()>0){
			dataTransfer* tmpPacket=incomingPackets.front();
			incomingPackets.pop();

			if(isDestinationForDataTransfer(tmpPacket)){

				// Will do read-modify-write based on the payload (the payload contains the bin)
				FULCRU_WORD_TYPE bin = *(tmpPacket->payload);
				hist[bin].cnt++;

				delete tmpPacket;
			}else{
				auto nextSubArray=getNextComputeSubArray(tmpPacket);
				nextSubArray->incomingPackets.push(tmpPacket);

			}
		}
	}
}

void computSubarray::initializePrefixSumWithinArrayGlobal(){
	readEnded = false;

	//TODO: move hist to memory object
	readStartAddress = 1;		//Not 0, as we need to access Hist[][bin-1]
	readEndAdddress = NUM_BINS;
	readAddressCounter = readStartAddress;
}

void computSubarray::runPrefixSumWithinArrayGlobalOneClockCycle(){
	if(isProceedRead()){
		hist[readAddressCounter].cnt += hist[readAddressCounter-1].cnt;
		readAddressCounter++;

		if(readAddressCounter == readEndAdddress){
			readEnded = true;
		}
	}

	if(isProceedWrite()){
		//TODO: Anything to do here?
	}
}

void computSubarray::initializePrefixSumNextArrayGlobal(){
	readEnded = false;

	//TODO: move hist to memory object
	//Go from NUM_BINS -> 0 to make the prefix sum available early
	readStartAddress = NUM_BINS-1;
	readEndAdddress = 0;
	readAddressCounter = readStartAddress;

	isSumAvailable = false;
	prefixSumOfLastSubarray = 0xDEADBEEF;		// Set to an odd value to detect if used pre-maturely
	if(SelfIndex == 0){
		isSumAvailable = true;
		prefixSumOfLastSubarray = 0;
	}
}

void computSubarray::runPrefixSumNextArrayGlobalOneClockCycle(){
	if(isProceedRead() && isSumAvailable){
		hist[readAddressCounter].cnt += prefixSumOfLastSubarray;
		if((readAddressCounter == (NUM_BINS - 1)) && (SelfIndex != (stackedMemoryObj->totNumComputeSubarray - 1))){
			//propagate value to next subarray
			dataTransfer* tmpPacket=new dataTransfer(sizeof(FULCRU_WORD_TYPE), SelfIndex + 1, (READ_DATA_TYPE_IN_MEMORY_ARRAY*) &hist[readAddressCounter].cnt);
			incomingPackets.push(tmpPacket);
		}
		if(readAddressCounter == readEndAdddress){
			readEnded = true;
		}
		readAddressCounter--;
	}

	if(isProceedWrite()){
		if(incomingPackets.size()>0){
			dataTransfer* tmpPacket=incomingPackets.front();
			incomingPackets.pop();

			if(isDestinationForDataTransfer(tmpPacket)){
				prefixSumOfLastSubarray = *(tmpPacket->payload);
				isSumAvailable = true;

				delete tmpPacket;
			}else{
				auto nextSubArray=getNextComputeSubArray(tmpPacket);
				nextSubArray->incomingPackets.push(tmpPacket);

			}
		}
	}
}

void computSubarray::initializePlacementGlobal(){
	readEnded = false;

	LOCAL_ADDRESS_TYPE tempValueForRead;

	memoryArrayObj->memory_read(ADDRESS_OF_START_ADDRESS, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
	readStartAddress = stackedMemoryObj->dataPartitioningObj->metadatatoLocalAddress (tempValueForRead);


	memoryArrayObj->memory_read(ADDRESS_OF_END_ADDRESS, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
	readEndAdddress = stackedMemoryObj->dataPartitioningObj->metadatatoLocalAddress (tempValueForRead);

	writeStartAddress = readEndAdddress;	// For now, write starts just after the read portion

	readAddressCounter = readStartAddress;
	//writeAddressCounter = writeStartAddress;
}

void computSubarray::runPlacementGlobalOneClockCycle(){
	if(isProceedRead()){
		LOCAL_ADDRESS_TYPE currentReadValue;
		memoryArrayObj->memory_read(readAddressCounter, sizeof(FULCRU_WORD_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(currentReadValue));

		FULCRU_WORD_TYPE dstSubForBin = extractBits(currentReadValue, stackedMemoryObj->logTotSubarray - 1, 0);
		//FULCRU_WORD_TYPE bin = extractBits(currentReadValue, stackedMemoryObj->logTotSubarray + LOG_NUM_BINS, stackedMemoryObj->logTotSubarray);

		//send key
		dataTransfer* tmpPacket=new dataTransfer(sizeof(FULCRU_WORD_TYPE), dstSubForBin, (READ_DATA_TYPE_IN_MEMORY_ARRAY*) &currentReadValue);
		incomingPackets.push(tmpPacket);

		readAddressCounter += sizeof(FULCRU_WORD_TYPE);
		if(readAddressCounter == readEndAdddress){
			readEnded = true;
		}
	}

	if(isProceedWrite()){
		if(incomingPackets.size()>0){
			dataTransfer* tmpPacket=incomingPackets.front();
			incomingPackets.pop();

			if(isDestinationForDataTransfer(tmpPacket)){
				FULCRU_WORD_TYPE key = *(tmpPacket->payload);

				FULCRU_WORD_TYPE bin = extractBits(key, stackedMemoryObj->logTotSubarray + LOG_NUM_BINS, stackedMemoryObj->logTotSubarray);

				LOCAL_ADDRESS_TYPE location = writeStartAddress + (hist[bin].cnt + hist[bin].used) * sizeof(LOCAL_ADDRESS_TYPE);
				hist[bin].used++;

				memoryArrayObj->memory_write(location, sizeof(FULCRU_WORD_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(key));

				delete tmpPacket;
			}else{
				auto nextSubArray=getNextComputeSubArray(tmpPacket);
				nextSubArray->incomingPackets.push(tmpPacket);
			}
		}
	}
}


