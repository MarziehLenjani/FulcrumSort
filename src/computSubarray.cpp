/*
 * computSubarray.cpp
 *
 *  Created on: May 16, 2021
 *      Author: marzieh
 */
#include "computSubarray.hpp"
#include "layer.hpp"
#include "bank.hpp"
#include "stackedMemory.hpp"
#include "dataPartitioning.hpp"
#include "dataTransfer.hpp"


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
    	currentReadValue=tempValueForRead;
    	LOCAL_ADDRESS_TYPE destinationId=(currentReadValue & RegA) >>RegB; //RegA is initialized with maskForBucketExtaction
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
				writeAddressCounter+=sizeof(FULCRU_WORD_TYPE);
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
	bool hasArrived=false;
	if(packet->destinationID==SelfIndex){
		hasArrived=true;
	}
	return hasArrived;

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



