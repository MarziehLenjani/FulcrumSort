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
#include <cstring>
#include "Walker.hpp"
#include "MemoryObject.hpp"

using namespace std;

computSubarray::computSubarray(ID_TYPE l_id, physicalComponent *l_parent) : physicalComponent(l_id, l_parent) {
	//TODO: do initializations specific for the subarray class here
	memoryArrayObj = new MemoryObject(G_SIZE_OF_SUBARRAY_IN_BYTE);
	bankObj = (bank*) parent;
	layerObj = (layer*) (bankObj->parent);
	stackedMemoryObj = (stackedMemory*) (layerObj->parent);
	for (u64 i = 0; i < NUM_WALKERS; i++) {
		walkers[i] = new Walker;
	}
}

computSubarray::~computSubarray() {
	//TODO: make sure it calls the decosntuctor
	for (u64 i = 0; i < NUM_WALKERS; i++) {
		delete walkers[i];
	}
	delete memoryArrayObj;
}

//void computSubarray::runOneSubClokCycle(){
//	//TODO: implement this function
//	bool proceedRead=false;
//	bool proceedWrite=false;
//	CONF_NUMBER_OF_WORDS_IN_A_ROW_TYPE nwrdsInRow=confObj->getConfig<CONF_NUMBER_OF_WORDS_IN_A_ROW_TYPE>(CONF_NUMBER_OF_WORDS_IN_A_ROW_NAME);
//	CONF_ROW_CYCLE_IN_SUB_CLOCK_CYCLE_TYPE rowCycleInSubClockCycle= confObj->getConfig<CONF_ROW_CYCLE_IN_SUB_CLOCK_CYCLE_TYPE>(CONF_ROW_CYCLE_IN_SUB_CLOCK_CYCLE_NAME);
//    if(readAddressCounter%nwrdsInRow !=0 ){
//    	proceedRead=true;
//    }else{
//    	if(writeWaitCounter==0){
//    		readWaitCounter++;
//    	}
//    	if(readWaitCounter==rowCycleInSubClockCycle){
//			readWaitCounter=0;
//			proceedRead=true;
//		}
//    }
//    if(proceedRead){
//    	readAddressCounter+=sizeof(FULCRU_WORD_TYPE);
//    	//TODO: implement what happens after reading here
//    	LOCAL_ADDRESS_TYPE tempValueForRead;
//    	memoryArrayObj->read(readAddressCounter, sizeof(FULCRU_WORD_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
//    	LOCAL_ADDRESS_TYPE destinationId=(tempValueForRead & RegA) >>RegB; //RegA is initialized with maskForBucketExtaction
//    	dataTransfer* tmpPacket=new dataTransfer(sizeof(tempValueForRead),destinationId,(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &tempValueForRead);
//    	incomingPackets.push(tmpPacket);
//
//    }
////-----------------------------
//    if(writeAddressCounter%nwrdsInRow !=0 ){
//    	proceedWrite=true;
//    }else{
//    	if(readWaitCounter==0){
//    		writeWaitCounter++;
//    	}
//    	if(writeWaitCounter==rowCycleInSubClockCycle){
//    		writeWaitCounter=0;
//			proceedWrite=true;
//		}
//    }
//    if(proceedWrite){
//    	//TODO: get the packet
//    	//write it in the subarray
//    	//delete the packet
//    	if(incomingPackets.size()>0){
//    		dataTransfer* tmpPacket=incomingPackets.front();
//    		incomingPackets.pop();
//
//			if(isDestinationForDataTransfer(tmpPacket)){
//				//packet has arrived
//				writeAddressCounter += sizeof(FULCRU_WORD_TYPE);
//				LOCAL_ADDRESS_TYPE tempValueForWrite=*(tmpPacket->payload);
//
//				memoryArrayObj->write(writeAddressCounter, sizeof(FULCRU_WORD_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForWrite));
//
//				delete tmpPacket;
//			}else{
//				auto nextSubArray=getNextComputeSubArray(tmpPacket);
//				nextSubArray->incomingPackets.push(tmpPacket);
//
//			}
//    	}
//    }
//    //-------------------------
//
//
//}

void computSubarray::initialize(LOCAL_ADDRESS_TYPE addressOfTheReadStartAddress,
		LOCAL_ADDRESS_TYPE addressOfTheReadEndAdddress,
		LOCAL_ADDRESS_TYPE addressOfTheWriteStartAddress,
		FULCRU_WORD_TYPE t_RegA, FULCRU_WORD_TYPE t_RegB,
		FULCRU_WORD_TYPE t_SelfIndex) {
//
//		LOCAL_ADDRESS_TYPE tempValueForRead;
//
//		memoryArrayObj->read(addressOfTheReadStartAddress, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
//		readStartAddress=stackedMemoryObj->dataPartitioningObj->metadatatoLocalAddress (tempValueForRead);
//
//
//		memoryArrayObj->read(addressOfTheReadEndAdddress, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
//		readEndAdddress=stackedMemoryObj->dataPartitioningObj->metadatatoLocalAddress (tempValueForRead);
//
//		memoryArrayObj->read(addressOfTheWriteStartAddress, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
//		writeStartAddress=stackedMemoryObj->dataPartitioningObj->metadatatoLocalAddress (tempValueForRead);
//
//		readAddressCounter=readStartAddress;
//		writeAddressCounter=writeStartAddress;
//
//		RegA=t_RegA;
//		RegB=t_RegB;
//		SelfIndex=t_SelfIndex;
//
//		 return;
}

void computSubarray::setMaskForBucketIDExtraction(
		FULCRU_WORD_TYPE maskForBucketExtaction,
		FULCRU_WORD_TYPE numberOfShiftsForBucketIDExtraction) {
	//RegA=maskForBucketExtaction;
	//RegB=numberOfShiftsForBucketIDExtraction;
}
//TODO:To be comepleted nd tested functions

std::queue <Packet<PlacementPacket>*>* computSubarray::getNextComputeSubArrayQRing(ID_TYPE dstId) {

	//Alif: Now using precomputed pointers to speed up routing simulation
	if(id == 0){
		//Change layer if necessary
		u64 currLayer = layerObj->id;
		u64 destLayer = dstId / (G_NUM_SUBARRAY_PER_BANK * G_NUM_BANKS_PER_LAYER);
		if(currLayer == destLayer){
			return nextSubarraySameLayerQ;	//same layer
		}
		else if(currLayer < destLayer){
			//go up
			return nextSubarrayUpLayerQ;
		}
		else{
			//go down
			return nextSubarrayDownLayerQ;
		}
	}
	else{
		return nextSubarraySameLayerQ;
	}
}

std::queue <Packet<PlacementPacket>*>* computSubarray::getNextComputeSubArrayQSemiRing(ID_TYPE dstId) {
	if(id == 0){
		u64 currBank = bankObj->id;
		u64 destBank = (dstId / G_NUM_SUBARRAY_PER_BANK) % G_NUM_BANKS_PER_LAYER;

		if(currBank != destBank){
			//go to next bank
			const u64 nextBank = (currBank + 1) % G_NUM_BANKS_PER_LAYER;
			return &(layerObj->bankVector[nextBank]->computSubarrayVector[0]->incomingPackets);
		}

		u64 currLayer = layerObj->id;
		u64 destLayer = dstId / (G_NUM_SUBARRAY_PER_BANK * G_NUM_BANKS_PER_LAYER);

		if(currLayer < destLayer){
			//go up
			return nextSubarrayUpLayerQ;
		}
		else if(currLayer > destLayer){
			//go down
			return nextSubarrayDownLayerQ;
		}
	}

	//if correct bank correct layer, go towards dest subarray, otherwise, go towards subarray 0
	if((SelfIndex / G_NUM_SUBARRAY_PER_BANK) == (dstId / G_NUM_SUBARRAY_PER_BANK)){
		if(SelfIndex > dstId){
			return &(bankObj->computSubarrayVector[id - 1]->incomingPackets);
		}
		else{
			return &(bankObj->computSubarrayVector[id + 1]->incomingPackets);
		}
	}
	else{
		return &(bankObj->computSubarrayVector[id - 1]->incomingPackets);
	}
}


std::queue <Packet<PlacementPacket>*>* computSubarray::getNextComputeSubArrayQIdeal(ID_TYPE dstId) {
	return &stackedMemoryObj->computSubarrayVector[dstId]->incomingPackets;
}

std::queue <Packet<PlacementPacket>*>* computSubarray::getNextComputeSubArrayQCrossbar(ID_TYPE dstId) {
	//if correct bank correct layer, go towards dest subarray
	if((SelfIndex / G_NUM_SUBARRAY_PER_BANK) == (dstId / G_NUM_SUBARRAY_PER_BANK)){
		if(SelfIndex > dstId){
			return &(bankObj->computSubarrayVector[id - 1]->incomingPackets);
		}
		else{
			return &(bankObj->computSubarrayVector[id + 1]->incomingPackets);
		}
	}

	if(id == 0){
		u64 currBank = bankObj->id;
		u64 destBank = (dstId / G_NUM_SUBARRAY_PER_BANK) % G_NUM_BANKS_PER_LAYER;
		if(currBank != destBank){
			//go towards logic layer
			if(layerObj->id == 0){
				//reached layer 0, do crossbar magic
				//TODO: Add logic layer, instead of forwarding from layer 0
				return &(layerObj->bankVector[destBank]->computSubarrayVector[0]->incomingPackets);
			}
			else{
				return nextSubarrayDownLayerQ;
			}
		}
		else{
			//correct bank, go towards correct layer
			u64 currLayer = layerObj->id;
			u64 destLayer = dstId / (G_NUM_SUBARRAY_PER_BANK * G_NUM_BANKS_PER_LAYER);

			if(currLayer < destLayer){
				//go up
				return nextSubarrayUpLayerQ;
			}
			else if(currLayer > destLayer){
				//go down
				return nextSubarrayDownLayerQ;
			}
		}
	}
	else{
		//towards subarray 0
		return &(bankObj->computSubarrayVector[id - 1]->incomingPackets);
	}
}


std::queue <Packet<PlacementPacket>*>* computSubarray::getNextComputeSubArrayQDragonfly(ID_TYPE dstId) {
	//if correct bank correct layer, go towards dest subarray
	if((SelfIndex / G_NUM_SUBARRAY_PER_BANK) == (dstId / G_NUM_SUBARRAY_PER_BANK)){
		if(SelfIndex > dstId){
			return &(bankObj->computSubarrayVector[id - 1]->incomingPackets);
		}
		else{
			return &(bankObj->computSubarrayVector[id + 1]->incomingPackets);
		}
	}

	if(id == 0){
		u64 currBank = bankObj->id;
		u64 destBank = (dstId / G_NUM_SUBARRAY_PER_BANK) % G_NUM_BANKS_PER_LAYER;

		if(currBank != destBank){
			u64 nextBank = dragonNextDst[currBank][destBank];
			return &(layerObj->bankVector[nextBank]->computSubarrayVector[0]->incomingPackets);
		}
		else{
			//correct bank, go towards correct layer
			u64 currLayer = layerObj->id;
			u64 destLayer = dstId / (G_NUM_SUBARRAY_PER_BANK * G_NUM_BANKS_PER_LAYER);

			if(currLayer < destLayer){
				//go up
				return nextSubarrayUpLayerQ;
			}
			else if(currLayer > destLayer){
				//go down
				return nextSubarrayDownLayerQ;
			}
		}
	}
	else{
		//towards subarray 0
		return &(bankObj->computSubarrayVector[id - 1]->incomingPackets);
	}
}


void computSubarray::initNextSubarrayRing(){
	//Pre compute the next subarray packet queue pointer if routing within the same layer
	bank *nextBank = NULL;
	if (bankObj->id != (G_NUM_BANKS_PER_LAYER - 1)) {
		nextBank = layerObj->bankVector[bankObj->id + 1];
	} else {
		nextBank = layerObj->bankVector[0];
	}

	if (bankObj->id % 2 == 0) { //even bank, directing upward
		if (id != (G_NUM_SUBARRAY_PER_BANK - 1)) {
			nextSubarraySameLayerQ = &(bankObj->computSubarrayVector[id + 1]->incomingPackets);
		} else {
			nextSubarraySameLayerQ = &(nextBank->computSubarrayVector[G_NUM_SUBARRAY_PER_BANK - 1]->incomingPackets);
		}
	} else {
		if (id != 0) { // odd bank directing downward
			nextSubarraySameLayerQ = &(bankObj->computSubarrayVector[id - 1]->incomingPackets);
		} else {
			nextSubarraySameLayerQ = &(nextBank->computSubarrayVector[0]->incomingPackets);
		}
	}

	if(id == 0){
		u64 currLayer = layerObj->id;
		u64 currBank = bankObj->id;


		//pre compute the down layer's connecting subarray
		if(currLayer != 0){
			nextSubarrayDownLayerQ = &(stackedMemoryObj->layerVector[currLayer - 1]->bankVector[currBank]->computSubarrayVector[0]->incomingPackets);
		}

		if(currLayer != (G_NUM_LAYERS - 1)){
			nextSubarrayUpLayerQ = &(stackedMemoryObj->layerVector[currLayer + 1]->bankVector[currBank]->computSubarrayVector[0]->incomingPackets);
		}
	}
}


void computSubarray::initNextSubarraySemiRing(){
	//Pre compute the next subarray packet queue pointer if routing within the same layer
	if(id == 0){
		u64 currLayer = layerObj->id;
		u64 currBank = bankObj->id;


		//pre compute the down layer's connecting subarray
		if(currLayer != 0){
			nextSubarrayDownLayerQ = &(stackedMemoryObj->layerVector[currLayer - 1]->bankVector[currBank]->computSubarrayVector[0]->incomingPackets);
		}

		if(currLayer != (G_NUM_LAYERS - 1)){
			nextSubarrayUpLayerQ = &(stackedMemoryObj->layerVector[currLayer + 1]->bankVector[currBank]->computSubarrayVector[0]->incomingPackets);
		}

		//Next bank's subarray 0
		bank *nextBank = NULL;
		if (bankObj->id != (G_NUM_BANKS_PER_LAYER - 1)) {
			nextBank = layerObj->bankVector[bankObj->id + 1];
		} else {
			nextBank = layerObj->bankVector[0];
		}
		nextSubarraySameLayerQ = &(nextBank->computSubarrayVector[0]->incomingPackets);
	}
	else{
		//send packet towards subarray 0 of the same bank
		nextSubarraySameLayerQ = &(bankObj->computSubarrayVector[id - 1]->incomingPackets);
	}
}

void computSubarray::initNextSubarrayCrossbar(){
	if(id == 0){
		u64 currLayer = layerObj->id;
		u64 currBank = bankObj->id;


		//pre compute the down layer's connecting subarray
		if(currLayer != 0){
			nextSubarrayDownLayerQ = &(stackedMemoryObj->layerVector[currLayer - 1]->bankVector[currBank]->computSubarrayVector[0]->incomingPackets);
		}

		if(currLayer != (G_NUM_LAYERS - 1)){
			nextSubarrayUpLayerQ = &(stackedMemoryObj->layerVector[currLayer + 1]->bankVector[currBank]->computSubarrayVector[0]->incomingPackets);
		}
	}
}

void computSubarray::initNextSubarrayDragonfly(){
	assert(G_NUM_BANKS_PER_LAYER == 64);
	if(id == 0){
		u64 currLayer = layerObj->id;
		u64 currBank = bankObj->id;


		//pre compute the down layer's connecting subarray
		if(currLayer != 0){
			nextSubarrayDownLayerQ = &(stackedMemoryObj->layerVector[currLayer - 1]->bankVector[currBank]->computSubarrayVector[0]->incomingPackets);
		}

		if(currLayer != (G_NUM_LAYERS - 1)){
			nextSubarrayUpLayerQ = &(stackedMemoryObj->layerVector[currLayer + 1]->bankVector[currBank]->computSubarrayVector[0]->incomingPackets);
		}
	}
}

//--------------To be implemented functions
void computSubarray::openANewSubBucket() {
	// TODO: implement this function
	//This function
}

void computSubarray::sealAllSubBuckets() {
	// TODO: implement this function
	//This function
}

FULCRU_WORD_TYPE computSubarray::extractBits(FULCRU_WORD_TYPE val,
		u32 highBitPos, u32 lowBitPos) {
	u32 width = highBitPos - lowBitPos;
	u64 mask = (1UL << width) - 1;

	return (val >> lowBitPos) & mask;
}


template<typename T>
void computSubarray::printMem(LOCAL_ADDRESS_TYPE startAddr,
		LOCAL_ADDRESS_TYPE endAddr) {
	for (; startAddr < endAddr; startAddr += sizeof(T)) {
		cout << "0x" << hex << memoryArrayObj->readX<T>(startAddr) << ", ";
	}
	cout << dec << endl;
}

void computSubarray::printReadElements() {
	cout << "[READ " << SelfIndex << "]: ";

	LOCAL_ADDRESS_TYPE startAddr = memoryArrayObj->readLocalAddr(
			G_ADDR_OF_READ_START_ADDR);
	LOCAL_ADDRESS_TYPE endAddr = memoryArrayObj->readLocalAddr(
			G_ADDR_OF_READ_END_ADDR);

	printMem<KEY_TYPE>(startAddr, endAddr);
}

void computSubarray::printReadElements(u64 maxElems) {
	cout << "[READ " << SelfIndex << "]: ";

	LOCAL_ADDRESS_TYPE startAddr = memoryArrayObj->readLocalAddr(
			G_ADDR_OF_READ_START_ADDR);
	LOCAL_ADDRESS_TYPE endAddr = startAddr + maxElems * sizeof(KEY_TYPE);

	printMem<KEY_TYPE>(startAddr, endAddr);
}

void computSubarray::printWriteElements() {
	cout << "[WRITE " << SelfIndex << "]: ";

	LOCAL_ADDRESS_TYPE startAddr = memoryArrayObj->readLocalAddr(
			G_ADDR_OF_WRITE_START_ADDR);
	LOCAL_ADDRESS_TYPE endAddr = memoryArrayObj->readLocalAddr(
			G_ADDR_OF_WRITE_END_ADDR);

	printMem<KEY_TYPE>(startAddr, endAddr);
}

void computSubarray::printWriteElements(u64 maxElems) {
	cout << "[WRITE " << SelfIndex << "]: ";

	LOCAL_ADDRESS_TYPE startAddr = memoryArrayObj->readLocalAddr(
			G_ADDR_OF_WRITE_START_ADDR);
	LOCAL_ADDRESS_TYPE endAddr = startAddr + maxElems * sizeof(KEY_TYPE);

	printMem<KEY_TYPE>(startAddr, endAddr);
}

void computSubarray::printHist() {
	cout << "[HIST " << SelfIndex << "]: ";

	LOCAL_ADDRESS_TYPE startAddr = memoryArrayObj->readLocalAddr(
			G_ADDR_OF_HIST_START_ADDR);
	LOCAL_ADDRESS_TYPE endAddr = memoryArrayObj->readLocalAddr(
			G_ADDR_OF_HIST_END_ADDR);

	printMem<HIST_ELEM_TYPE>(startAddr, endAddr);
}

void computSubarray::printHist(u64 maxElems) {
	cout << "[HIST " << SelfIndex << "]: ";

	LOCAL_ADDRESS_TYPE startAddr = memoryArrayObj->readLocalAddr(
			G_ADDR_OF_HIST_START_ADDR);
	LOCAL_ADDRESS_TYPE endAddr = startAddr + maxElems * sizeof(HIST_ELEM_TYPE);

	printMem<HIST_ELEM_TYPE>(startAddr, endAddr);
}

//void computSubarray::initializeLocalSort(){
//	RegA = 0;		//holds count for radix bit = 0
//	RegB = 0;		//holds count for radix bit = 1
//
//}
//
//void computSubarray::runLocalSortOneClockCycle(){
//	//currReadAddress = DataAddress
//	//rowOfCurrReadAddress
//	//if(readAddress)
//
//}
//
//void computSubarray::eventRunLocalSort(void* obj, void* arg){
//	computSubarray* sub = (computSubarray*)obj;
//
//	//do the initialization
//	sub->RegA = 0;
//	sub->RegB = 0;
//}

