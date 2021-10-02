/*
 * computSubarray.cpp
 *
 *  Created on: May 16, 2021
 *      Author: marzieh
 */


#include "Subarray.hpp"
#include "types.hpp"
#include <cstring>

#include "PulleySystem.hpp"
#include "Device.hpp"
#include "Stack.hpp"
#include "Layer.hpp"
#include "Bank.hpp"


#include "Walker.hpp"
#include "MemoryObject.hpp"
#include <cstdlib>

using namespace std;

Subarray::Subarray(ID_TYPE l_id, PhysicalComponent *l_parent) : PhysicalComponent(l_id, l_parent) {
	//TODO: do initializations specific for the subarray class here
	//memoryArrayObj = new MemoryObject(G_SIZE_OF_SUBARRAY_IN_BYTE);
	bank = (Bank*) parent;
	//layer = (Layer*) (bank->parent);
	//stack = (Stack*) (layer->parent);
	//device = (Device*) (stack->parent);
	//pulley = (PulleySystem*) (device->parent);
//	for (u64 i = 0; i < NUM_WALKERS; i++) {
//		walkers[i] = new Walker;
//	}

	keys = (KEY_TYPE*)malloc(sizeof(KEY_TYPE) * elemPerSubarray);
	placementPackets = (PlacementPacket*)malloc(sizeof(PlacementPacket) * elemPerSubarray);
}

Subarray::~Subarray() {
//	for (u64 i = 0; i < NUM_WALKERS; i++) {
//		delete walkers[i];
//	}
	//delete memoryArrayObj;

	free(keys);
	keys = nullptr;
	free(placementPackets);
	placementPackets = nullptr;
}

void Subarray::prePlacementProducePackets(std::queue <Packet<PlacementPacket>* > &packetQ, HIST_ELEM_TYPE* histogram){
	u64 pktIdxBase = selfIndex * elemPerSubarray;
	for(i64 currReadIdx = readEndIdx - 1; currReadIdx >= readStartIdx; currReadIdx--){
		KEY_TYPE key = keys[currReadIdx];
		FULCRU_WORD_TYPE radix = extractBits(key, radixEndBit, radixStartBit);
		HIST_ELEM_TYPE location = --histogram[radix % G_NUM_HIST_ELEMS];

		//push into queue
		u64 dstSubAddr = location >> locShiftAmt;
		LOCAL_ADDRESS_TYPE dstOff = location & ((1UL << locShiftAmt) - 1);

		Packet<PlacementPacket>* tmpPacket = packetPool + pktIdxBase + currReadIdx;
		tmpPacket->dstBankAddr = dstSubAddr / G_NUM_SUBARRAY_PER_BANK;
		tmpPacket->dstSubId = dstSubAddr % G_NUM_SUBARRAY_PER_BANK;
		tmpPacket->payload.key = key;
		tmpPacket->payload.offset = dstOff * sizeof(KEY_TYPE);

		packetQ.push(tmpPacket);

		if(tmpPacket->dstBankAddr == bank->selfIndex){
			//Destination is the current bank. Just have to change the subarray to the correct one.
			bank->numSubToSubPackets += abs((int)(tmpPacket->dstSubId - id));
		}
		else{
			//Destination is NOT the current bank. Packet have to traverse through all subarray upto 0.
			//Then reaching the correct bank, it has to traverse to the destination subarray.
			bank->numSubToSubPackets += (id + tmpPacket->dstSubId);
		}
	}
}

void Subarray::runPlacementOneCycle(){
	if(!finishedPlacementRead){

		//stateCounter[placementSchState]++;

		switch(placementSchState){

		case PSTATE_PLACEMENT:
		{
			//sequential read of placement packets
			const PlacementPacket& pkt = placementPackets[appendIdx];
			targetAddr = pkt.offset;

			if(currOpenRow != extractRowIndexFromLocalAddress(targetAddr)){
				//placementRowMiss++;
				bank->numRowActivations++;
				waitCounter = G_ROW_ACCESS_LATENCY;
				placementSchState = PSTATE_STALLED_ON_PLACEMENT;		//row not latched on walker 0
			}
			else{
				//placementRowHit++;
				//already latched
				keys[targetAddr / sizeof(KEY_TYPE)] = pkt.key;

//					if(targetAddr + sizeof(KEY_TYPE) > memoryArrayObj->readLocalAddr(G_ADDR_OF_READ_END_ADDR)){
//						memoryArrayObj->writeLocalAddr(G_ADDR_OF_READ_END_ADDR, targetAddr + sizeof(KEY_TYPE));
//					}

				//go to next element
				appendIdx++;

				if(appendIdx == elemPerSubarray){
					//Finished going through all elements in range
					finishedPlacementRead = true;

					#pragma omp atomic
					numOfProcessedSubarrays++;
				}
			}

		}
			break;


		case PSTATE_STALLED_ON_PLACEMENT:
			if(!--waitCounter){
				currOpenRow = extractRowIndexFromLocalAddress(targetAddr);	//timer expired. Target row is now open.
				//walkers[2]->latchedRow->data = memoryArrayObj->data + G_NUM_BYTES_IN_ROW * currOpenRow;
				//walkers[2]->latchedRowIndex = currOpenRow;
				placementSchState = PSTATE_PLACEMENT;
			}
			break;


		default:
			std::cerr << "Invalid placement state!!" << std::endl;
			exit(-1);
		}
	}
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

//void Subarray::initialize(LOCAL_ADDRESS_TYPE addressOfTheReadStartAddress,
//		LOCAL_ADDRESS_TYPE addressOfTheReadEndAdddress,
//		LOCAL_ADDRESS_TYPE addressOfTheWriteStartAddress,
//		FULCRU_WORD_TYPE t_RegA, FULCRU_WORD_TYPE t_RegB,
//		FULCRU_WORD_TYPE t_SelfIndex) {
////
////		LOCAL_ADDRESS_TYPE tempValueForRead;
////
////		memoryArrayObj->read(addressOfTheReadStartAddress, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
////		readStartAddress=stackedMemoryObj->dataPartitioningObj->metadatatoLocalAddress (tempValueForRead);
////
////
////		memoryArrayObj->read(addressOfTheReadEndAdddress, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
////		readEndAdddress=stackedMemoryObj->dataPartitioningObj->metadatatoLocalAddress (tempValueForRead);
////
////		memoryArrayObj->read(addressOfTheWriteStartAddress, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
////		writeStartAddress=stackedMemoryObj->dataPartitioningObj->metadatatoLocalAddress (tempValueForRead);
////
////		readAddressCounter=readStartAddress;
////		writeAddressCounter=writeStartAddress;
////
////		RegA=t_RegA;
////		RegB=t_RegB;
////		SelfIndex=t_SelfIndex;
////
////		 return;
//}

//void Subarray::setMaskForBucketIDExtraction(
//		FULCRU_WORD_TYPE maskForBucketExtaction,
//		FULCRU_WORD_TYPE numberOfShiftsForBucketIDExtraction) {
//	//RegA=maskForBucketExtaction;
//	//RegB=numberOfShiftsForBucketIDExtraction;
//}
//TODO:To be comepleted nd tested functions

//std::queue <Packet<PlacementPacket>*>* Subarray::getNextComputeSubArrayQRing(ID_TYPE dstId) {
//
//	//Alif: Now using precomputed pointers to speed up routing simulation
//	if(id == 0){
//		//Change layer if necessary
//		u64 currLayer = layer->id;
//		u64 destLayer = dstId / (G_NUM_SUBARRAY_PER_BANK * G_NUM_BANKS_PER_LAYER);
//		if(currLayer == destLayer){
//			return nextSubarraySameLayerQ;	//same layer
//		}
//		else if(currLayer < destLayer){
//			//go up
//			return nextSubarrayUpLayerQ;
//		}
//		else{
//			//go down
//			return nextSubarrayDownLayerQ;
//		}
//	}
//	else{
//		return nextSubarraySameLayerQ;
//	}
//}

//std::queue <Packet<PlacementPacket>*>* Subarray::getNextComputeSubArrayQSemiRing(ID_TYPE dstId) {
//	if(id == 0){
//		u64 currBank = bank->id;
//		u64 destBank = (dstId / G_NUM_SUBARRAY_PER_BANK) % G_NUM_BANKS_PER_LAYER;
//
//		if(currBank != destBank){
//			//go to next bank
//			const u64 nextBank = (currBank + 1) % G_NUM_BANKS_PER_LAYER;
//			return &(layer->bankVector[nextBank]->subarrayVector[0]->incomingPackets);
//		}
//
//		u64 currLayer = layer->id;
//		u64 destLayer = dstId / (G_NUM_SUBARRAY_PER_BANK * G_NUM_BANKS_PER_LAYER);
//
//		if(currLayer < destLayer){
//			//go up
//			return bank->upperLayerQ;
//		}
//		else if(currLayer > destLayer){
//			//go down
//			return bank->lowerLayerQ;
//		}
//	}
//
//	//if correct bank correct layer, go towards dest subarray, otherwise, go towards subarray 0
//	if((SelfIndex / G_NUM_SUBARRAY_PER_BANK) == (dstId / G_NUM_SUBARRAY_PER_BANK)){
//		if(SelfIndex > dstId){
//			return &(bank->subarrayVector[id - 1]->incomingPackets);
//		}
//		else{
//			return &(bank->subarrayVector[id + 1]->incomingPackets);
//		}
//	}
//	else{
//		return &(bank->subarrayVector[id - 1]->incomingPackets);
//	}
//}


//std::queue <Packet<PlacementPacket>*>* Subarray::getNextComputeSubArrayQIdeal(ID_TYPE dstId) {
//	return &pulley->subarrayVector[dstId]->incomingPackets;
//}

//std::queue <Packet<PlacementPacket>*>* Subarray::getNextComputeSubArrayQCrossbar(ID_TYPE dstId) {
//	//if correct bank correct layer, go towards dest subarray
//	if((SelfIndex / G_NUM_SUBARRAY_PER_BANK) == (dstId / G_NUM_SUBARRAY_PER_BANK)){
//		if(SelfIndex > dstId){
//			return &(bank->subarrayVector[id - 1]->incomingPackets);
//		}
//		else{
//			return &(bank->subarrayVector[id + 1]->incomingPackets);
//		}
//	}
//
//	if(id == 0){
//		u64 currBank = bank->id;
//		u64 destBank = (dstId / G_NUM_SUBARRAY_PER_BANK) % G_NUM_BANKS_PER_LAYER;
//		if(currBank != destBank){
//			//go towards logic layer
//			if(layer->id == 0){
//				//reached layer 0, do crossbar magic
//				//TODO: Add logic layer, instead of forwarding from layer 0
//				return &(layer->bankVector[destBank]->subarrayVector[0]->incomingPackets);
//			}
//			else{
//				return bank->lowerLayerQ;
//			}
//		}
//		else{
//			//correct bank, go towards correct layer
//			u64 currLayer = layer->id;
//			u64 destLayer = dstId / (G_NUM_SUBARRAY_PER_BANK * G_NUM_BANKS_PER_LAYER);
//
//			if(currLayer < destLayer){
//				//go up
//				return bank->upperLayerQ;
//			}
//			else if(currLayer > destLayer){
//				//go down
//				return bank->lowerLayerQ;
//			}
//		}
//	}
//	else{
//		//towards subarray 0
//		return &(bank->subarrayVector[id - 1]->incomingPackets);
//	}
//
//	return nullptr;
//}


//std::queue <Packet<PlacementPacket>*>* Subarray::getNextComputeSubArrayQDragonfly(ID_TYPE dstId) {
//	//if correct bank correct layer, go towards dest subarray
//	if((SelfIndex / G_NUM_SUBARRAY_PER_BANK) == (dstId / G_NUM_SUBARRAY_PER_BANK)){
//		if(SelfIndex > dstId){
//			return &(bank->subarrayVector[id - 1]->incomingPackets);
//		}
//		else{
//			return &(bank->subarrayVector[id + 1]->incomingPackets);
//		}
//	}
//
//	if(id == 0){
//		u64 currBank = bank->id;
//		u64 destBank = (dstId / G_NUM_SUBARRAY_PER_BANK) % G_NUM_BANKS_PER_LAYER;
//
//		if(currBank != destBank){
//			u64 nextBank = dragonNextDst[currBank][destBank];
//			return &(layer->bankVector[nextBank]->subarrayVector[0]->incomingPackets);
//		}
//		else{
//			//correct bank, go towards correct layer
//			u64 currLayer = layer->id;
//			u64 destLayer = dstId / (G_NUM_SUBARRAY_PER_BANK * G_NUM_BANKS_PER_LAYER);
//
//			if(currLayer < destLayer){
//				//go up
//				return bank->upperLayerQ;
//			}
//			else if(currLayer > destLayer){
//				//go down
//				return bank->lowerLayerQ;
//			}
//		}
//	}
//	else{
//		//towards subarray 0
//		return &(bank->subarrayVector[id - 1]->incomingPackets);
//	}
//
//	return nullptr;
//}



//std::queue <Packet<PlacementPacket>*>* Subarray::getNextComputeSubArrayQDragonflyNew(ID_TYPE dstId) {
//	//if correct bank correct layer, go towards dest subarray
//	if(extractUptoBank(dstId) == extractUptoBank(SelfIndex)){
//		if(SelfIndex > dstId){
//			return &(bank->subarrayVector[id - 1]->incomingPackets);
//		}
//		else{
//			return &(bank->subarrayVector[id + 1]->incomingPackets);
//		}
//	}
//
//	if(id == 0){
//		if(extractUptoStack(dstId) == extractUptoStack(SelfIndex)){
//			//correct stack
//
//			u64 currBank = extractBankId(SelfIndex);
//			u64 destBank = extractBankId(dstId);
//
//			if(currBank != destBank){
//				u64 nextBank = dragonNextDst[currBank][destBank];
//				return &(layer->bankVector[nextBank]->subarrayVector[0]->incomingPackets);
//			}
//			else{
//				//correct bank, go towards correct layer
//				u64 currLayer = extractLayerId(SelfIndex);
//				u64 destLayer = extractLayerId(dstId);
//
//				if(currLayer < destLayer){
//					//go up
//					return bank->upperLayerQ;
//				}
//				else if(currLayer > destLayer){
//					//go down
//					return bank->lowerLayerQ;
//				}
//			}
//		}
//		else{
//			//Incorrect stack. Go towards logic layer
//			return bank->lowerLayerQ;
//		}
//	}
//	else{
//		//towards subarray 0
//		return &(bank->subarrayVector[id - 1]->incomingPackets);
//	}
//
//	return nullptr;
//}


//void Subarray::initNextSubarrayRing(){
//	//Pre compute the next subarray packet queue pointer if routing within the same layer
//	Bank *nextBank = NULL;
//	if (bank->id != (G_NUM_BANKS_PER_LAYER - 1)) {
//		nextBank = layer->bankVector[bank->id + 1];
//	} else {
//		nextBank = layer->bankVector[0];
//	}
//
//	if (bank->id % 2 == 0) { //even bank, directing upward
//		if (id != (G_NUM_SUBARRAY_PER_BANK - 1)) {
//			nextSubarraySameLayerQ = &(bank->subarrayVector[id + 1]->incomingPackets);
//		} else {
//			nextSubarraySameLayerQ = &(nextBank->subarrayVector[G_NUM_SUBARRAY_PER_BANK - 1]->incomingPackets);
//		}
//	} else {
//		if (id != 0) { // odd bank directing downward
//			nextSubarraySameLayerQ = &(bank->subarrayVector[id - 1]->incomingPackets);
//		} else {
//			nextSubarraySameLayerQ = &(nextBank->subarrayVector[0]->incomingPackets);
//		}
//	}
//
//	if(id == 0){
//		u64 currLayer = layer->id;
//		u64 currBank = bank->id;
//
//
//		//pre compute the down layer's connecting subarray
//		if(currLayer != 0){
//			nextSubarrayDownLayerQ = &(stack->layerVector[currLayer - 1]->bankVector[currBank]->subarrayVector[0]->incomingPackets);
//		}
//
//		if(currLayer != (G_NUM_LAYERS_PER_STACK - 1)){
//			nextSubarrayUpLayerQ = &(stack->layerVector[currLayer + 1]->bankVector[currBank]->subarrayVector[0]->incomingPackets);
//		}
//	}
//}


//void Subarray::initNextSubarraySemiRing(){
//	//Pre compute the next subarray packet queue pointer if routing within the same layer
//	if(id == 0){
//		u64 currLayer = layer->id;
//		u64 currBank = bank->id;
//
//
//		//pre compute the down layer's connecting subarray
//		if(currLayer != 0){
//			nextSubarrayDownLayerQ = &(stack->layerVector[currLayer - 1]->bankVector[currBank]->subarrayVector[0]->incomingPackets);
//		}
//
//		if(currLayer != (G_NUM_LAYERS_PER_STACK - 1)){
//			nextSubarrayUpLayerQ = &(stack->layerVector[currLayer + 1]->bankVector[currBank]->subarrayVector[0]->incomingPackets);
//		}
//
//		//Next bank's subarray 0
//		Bank *nextBank = NULL;
//		if (bank->id != (G_NUM_BANKS_PER_LAYER - 1)) {
//			nextBank = layer->bankVector[bank->id + 1];
//		} else {
//			nextBank = layer->bankVector[0];
//		}
//		nextSubarraySameLayerQ = &(nextBank->subarrayVector[0]->incomingPackets);
//	}
//	else{
//		//send packet towards subarray 0 of the same bank
//		nextSubarraySameLayerQ = &(bank->subarrayVector[id - 1]->incomingPackets);
//	}
//}

//--------------To be implemented functions
//void Subarray::openANewSubBucket() {
//	// TODO: implement this function
//	//This function
//}

//void Subarray::sealAllSubBuckets() {
//	// TODO: implement this function
//	//This function
//}

FULCRU_WORD_TYPE Subarray::extractBits(FULCRU_WORD_TYPE val,
		u32 highBitPos, u32 lowBitPos) {
	u32 width = highBitPos - lowBitPos;
	u64 mask = (1UL << width) - 1;

	return (val >> lowBitPos) & mask;
}

//
//template<typename T>
//void Subarray::printMem(LOCAL_ADDRESS_TYPE startAddr,
//		LOCAL_ADDRESS_TYPE endAddr) {
//	for (; startAddr < endAddr; startAddr += sizeof(T)) {
//		cout << "0x" << hex << memoryArrayObj->readX<T>(startAddr) << ", ";
//	}
//	cout << dec << endl;
//}
//
//void Subarray::printReadElements() {
//	//cout << "[READ " << SelfIndex << "]: ";
//
//	LOCAL_ADDRESS_TYPE startAddr = memoryArrayObj->readLocalAddr(
//			G_ADDR_OF_READ_START_ADDR);
//	LOCAL_ADDRESS_TYPE endAddr = memoryArrayObj->readLocalAddr(
//			G_ADDR_OF_READ_END_ADDR);
//
//	printMem<KEY_TYPE>(startAddr, endAddr);
//}
//
//void Subarray::printReadElements(u64 maxElems) {
//	//cout << "[READ " << SelfIndex << "]: ";
//
//	LOCAL_ADDRESS_TYPE startAddr = memoryArrayObj->readLocalAddr(
//			G_ADDR_OF_READ_START_ADDR);
//	LOCAL_ADDRESS_TYPE endAddr = startAddr + maxElems * sizeof(KEY_TYPE);
//
//	printMem<KEY_TYPE>(startAddr, endAddr);
//}
//
//void Subarray::printWriteElements() {
//	//cout << "[WRITE " << SelfIndex << "]: ";
//
//	LOCAL_ADDRESS_TYPE startAddr = memoryArrayObj->readLocalAddr(
//			G_ADDR_OF_WRITE_START_ADDR);
//	LOCAL_ADDRESS_TYPE endAddr = memoryArrayObj->readLocalAddr(
//			G_ADDR_OF_WRITE_END_ADDR);
//
//	printMem<KEY_TYPE>(startAddr, endAddr);
//}
//
//void Subarray::printWriteElements(u64 maxElems) {
//	//cout << "[WRITE " << SelfIndex << "]: ";
//
//	LOCAL_ADDRESS_TYPE startAddr = memoryArrayObj->readLocalAddr(
//			G_ADDR_OF_WRITE_START_ADDR);
//	LOCAL_ADDRESS_TYPE endAddr = startAddr + maxElems * sizeof(KEY_TYPE);
//
//	printMem<KEY_TYPE>(startAddr, endAddr);
//}
//
//void Subarray::printHist() {
//	//cout << "[HIST " << SelfIndex << "]: ";
//
//	LOCAL_ADDRESS_TYPE startAddr = memoryArrayObj->readLocalAddr(
//			G_ADDR_OF_HIST_START_ADDR);
//	LOCAL_ADDRESS_TYPE endAddr = memoryArrayObj->readLocalAddr(
//			G_ADDR_OF_HIST_END_ADDR);
//
//	printMem<HIST_ELEM_TYPE>(startAddr, endAddr);
//}
//
//void Subarray::printHist(u64 maxElems) {
//	//cout << "[HIST " << SelfIndex << "]: ";
//
//	LOCAL_ADDRESS_TYPE startAddr = memoryArrayObj->readLocalAddr(
//			G_ADDR_OF_HIST_START_ADDR);
//	LOCAL_ADDRESS_TYPE endAddr = startAddr + maxElems * sizeof(HIST_ELEM_TYPE);
//
//	printMem<HIST_ELEM_TYPE>(startAddr, endAddr);
//}

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

