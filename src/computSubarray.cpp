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

computSubarray::computSubarray(ID_TYPE l_id, physicalComponent *l_firstDimOwner,
		physicalComponent *l_secondDimOwner, physicalComponent *l_thirdDimOwner) :
		physicalComponent(l_id, l_firstDimOwner, l_secondDimOwner,
				l_thirdDimOwner) {
	//TODO: do initializations specific for the subarray class here
	memoryArrayObj = new MemoryObject(G_SIZE_OF_SUBARRAY_IN_BYTE);
	bankObj = (bank*) firstDimOwner;
	layerObj = (layer*) (bankObj->firstDimOwner);
	stackedMemoryObj = (stackedMemory*) (layerObj->firstDimOwner);
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

std::queue <Packet<PlacementPacket>*>* computSubarray::getNextComputeSubArrayQ(ID_TYPE dstId) {

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

//	computSubarray *nextSubArray = NULL;
//	layer *nextLayer = NULL;
//	// we have ring per two vaults, where the lowest layer has ID of 0, every two banks are connected to the same vault, so every four banks are part of a ring
//
//	if ((bankObj->id % 4) < 2) { //even vaults send upwards
//		if (layerObj->id < (G_NUM_LAYERS - 1)) {
//			nextLayer = stackedMemoryObj->layerVector[layerObj->id + 1];
//		} else {
//			nextLayer = stackedMemoryObj->layerVector[layerObj->id - 1];
//		}
//	} else { //odd vaults send  downward
//		if (layerObj->id > 0) {
//			nextLayer = stackedMemoryObj->layerVector[layerObj->id - 1];
//		} else {
//			nextLayer = stackedMemoryObj->layerVector[layerObj->id + 1];
//		}
//	}
//	bank *nextBank = NULL;
//
//	if (bankObj->id != (G_NUM_BANKS_PER_LAYER - 1)) {
//		nextBank = layerObj->bankVector[bankObj->id + 1];
//	} else {
//		nextBank = layerObj->bankVector[0];
//	}
//	//we have a ring per layer, where the lowest subarray has ID of 0, even banks direct packets upward
//	bool sameLayer = true;
//	if (id == 0) { //subarray zero always decides if the same layer
//		FULCRU_WORD_TYPE firstIndexInLayer = layerObj->bankVector[0]->computSubarrayVector[0]->SelfIndex;
//		FULCRU_WORD_TYPE lastIndexInLayer =	layerObj->bankVector[G_NUM_BANKS_PER_LAYER - 1]->computSubarrayVector[G_NUM_SUBARRAY_PER_BANK - 1]->SelfIndex;
//		if (firstIndexInLayer < lastIndexInLayer) {
//			if (pckt->destinationID >= firstIndexInLayer && pckt->destinationID <= lastIndexInLayer) {
//				sameLayer = true;
//			} else {
//				sameLayer = false;
//
//			}
//		} else {
//
//			if (pckt->destinationID >= firstIndexInLayer || pckt->destinationID <= lastIndexInLayer) {
//				sameLayer = true;
//			} else {
//				sameLayer = false;
//
//			}
//		}
//
//	}
//
//	if (sameLayer) {
//		if (bankObj->id % 2 == 0) { //even bank, directing upward
//			if (id != (G_NUM_SUBARRAY_PER_BANK - 1)) {
//				nextSubArray = bankObj->computSubarrayVector[id + 1];
//			} else {
//				nextSubArray =	nextBank->computSubarrayVector[G_NUM_SUBARRAY_PER_BANK - 1];
//
//			}
//		} else {
//			if (id != 0) { // odd bank directing downward
//				nextSubArray = bankObj->computSubarrayVector[id - 1];
//			} else {
//				nextSubArray = nextBank->computSubarrayVector[0];
//			}
//
//		}
//	} else {
//		nextSubArray = nextLayer->bankVector[bankObj->id]->computSubarrayVector[0];
//	}
//
//	return nextSubArray;
}

void computSubarray::initNextSubarray(){
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

//void computSubarray::initializeHistGenGlobal(){
//	memset(hist, 0, G_NUM_BINS * sizeof(Histogram));
//	readEnded = false;
//
//	LOCAL_ADDRESS_TYPE tempValueForRead;
//
//	memoryArrayObj->read(G_ADDRESS_OF_START_ADDRESS, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
//	readStartAddress = stackedMemoryObj->dataPartitioningObj->metadatatoLocalAddress (tempValueForRead);
//
//
//	memoryArrayObj->read(G_ADDRESS_OF_END_ADDRESS, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
//	readEndAdddress = stackedMemoryObj->dataPartitioningObj->metadatatoLocalAddress (tempValueForRead);
//
//	//writeStartAddress = readEndAdddress;	// For now, write starts just after the read portion
//
//	readAddressCounter = readStartAddress;
//	//writeAddressCounter = writeStartAddress;
//
//	nwrdsInRow=confObj->getConfig<CONF_NUMBER_OF_WORDS_IN_A_ROW_TYPE>(CONF_NUMBER_OF_WORDS_IN_A_ROW_NAME);
//	rowCycleInSubClockCycle= confObj->getConfig<CONF_ROW_CYCLE_IN_SUB_CLOCK_CYCLE_TYPE>(CONF_ROW_CYCLE_IN_SUB_CLOCK_CYCLE_NAME);
//}

//bool computSubarray::isProceedRead(){
//	if(readEnded){
//		return false;
//	}
//	bool proceedRead=false;
//	if(readAddressCounter%nwrdsInRow !=0 ){
//		proceedRead=true;
//	}else{
//		if(writeWaitCounter==0){
//			readWaitCounter++;
//		}
//		if(readWaitCounter==rowCycleInSubClockCycle){
//			readWaitCounter=0;
//			proceedRead=true;
//		}
//	}
//	return proceedRead;
//}
//
//bool computSubarray::isProceedWrite(){
//	bool proceedWrite = false;
//	if(writeAddressCounter%nwrdsInRow !=0 ){
//		proceedWrite=true;
//	}else{
//		if(readWaitCounter==0){
//			writeWaitCounter++;
//		}
//		if(writeWaitCounter==rowCycleInSubClockCycle){
//			writeWaitCounter=0;
//			proceedWrite=true;
//		}
//	}
//	return proceedWrite;
//}

//void computSubarray::runHistGenGlobalOneClockCycle(){
//	if(isProceedRead()){
//		LOCAL_ADDRESS_TYPE currentReadValue;
//		memoryArrayObj->read(readAddressCounter, sizeof(FULCRU_WORD_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(currentReadValue));
//
//		FULCRU_WORD_TYPE dstSubForBin = extractBits(currentReadValue, stackedMemoryObj->logTotSubarray - 1, 0);
//		FULCRU_WORD_TYPE bin = extractBits(currentReadValue, stackedMemoryObj->logTotSubarray + G_LOG_NUM_BINS, stackedMemoryObj->logTotSubarray);
//
//		dataTransfer* tmpPacket=new dataTransfer(sizeof(FULCRU_WORD_TYPE), dstSubForBin, (READ_DATA_TYPE_IN_MEMORY_ARRAY*) &bin);
//		incomingPackets.push(tmpPacket);
//
//		readAddressCounter += sizeof(FULCRU_WORD_TYPE);
//		if(readAddressCounter == readEndAdddress){
//			readEnded = true;
//		}
//	}
//
//	if(isProceedWrite()){
//		//TODO: get the packet
//		//write it in the subarray
//		//delete the packet
//		if(incomingPackets.size()>0){
//			dataTransfer* tmpPacket=incomingPackets.front();
//			incomingPackets.pop();
//
//			if(isDestinationForDataTransfer(tmpPacket)){
//
//				// Will do read-modify-write based on the payload (the payload contains the bin)
//				FULCRU_WORD_TYPE bin = *(tmpPacket->payload);
//				hist[bin].cnt++;
//
//				delete tmpPacket;
//			}else{
//				auto nextSubArray=getNextComputeSubArray(tmpPacket);
//				nextSubArray->incomingPackets.push(tmpPacket);
//
//			}
//		}
//	}
//}

//void computSubarray::initializePrefixSumWithinArrayGlobal(){
//	readEnded = false;
//
//	//TODO: move hist to memory object
//	readStartAddress = 1;		//Not 0, as we need to access Hist[][bin-1]
//	readEndAdddress = G_NUM_BINS;
//	readAddressCounter = readStartAddress;
//}

//void computSubarray::runPrefixSumWithinArrayGlobalOneClockCycle(){
//	if(isProceedRead()){
//		hist[readAddressCounter].cnt += hist[readAddressCounter-1].cnt;
//		readAddressCounter++;
//
//		if(readAddressCounter == readEndAdddress){
//			readEnded = true;
//		}
//	}
//
//	if(isProceedWrite()){
//		//TODO: Anything to do here?
//	}
//}
//
//void computSubarray::initializePrefixSumNextArrayGlobal(){
//	readEnded = false;
//
//	//TODO: move hist to memory object
//	//Go from NUM_BINS -> 0 to make the prefix sum available early
//	readStartAddress = G_NUM_BINS-1;
//	readEndAdddress = 0;
//	readAddressCounter = readStartAddress;
//
//	isSumAvailable = false;
//	prefixSumOfLastSubarray = 0xDEADBEEF;		// Set to an odd value to detect if used pre-maturely
//	if(SelfIndex == 0){
//		isSumAvailable = true;
//		prefixSumOfLastSubarray = 0;
//	}
//}
//
//void computSubarray::runPrefixSumNextArrayGlobalOneClockCycle(){
//	if(isProceedRead() && isSumAvailable){
//		hist[readAddressCounter].cnt += prefixSumOfLastSubarray;
//		if((readAddressCounter == (G_NUM_BINS - 1)) && (SelfIndex != (stackedMemoryObj->totNumComputeSubarray - 1))){
//			//propagate value to next subarray
//			dataTransfer* tmpPacket=new dataTransfer(sizeof(FULCRU_WORD_TYPE), SelfIndex + 1, (READ_DATA_TYPE_IN_MEMORY_ARRAY*) &hist[readAddressCounter].cnt);
//			incomingPackets.push(tmpPacket);
//		}
//		if(readAddressCounter == readEndAdddress){
//			readEnded = true;
//		}
//		readAddressCounter--;
//	}
//
//	if(isProceedWrite()){
//		if(incomingPackets.size()>0){
//			dataTransfer* tmpPacket=incomingPackets.front();
//			incomingPackets.pop();
//
//			if(isDestinationForDataTransfer(tmpPacket)){
//				prefixSumOfLastSubarray = *(tmpPacket->payload);
//				isSumAvailable = true;
//
//				delete tmpPacket;
//			}else{
//				auto nextSubArray=getNextComputeSubArray(tmpPacket);
//				nextSubArray->incomingPackets.push(tmpPacket);
//
//			}
//		}
//	}
//}
//
//void computSubarray::initializePlacementGlobal(){
//	readEnded = false;
//
//	LOCAL_ADDRESS_TYPE tempValueForRead;
//
//	memoryArrayObj->read(G_ADDRESS_OF_START_ADDRESS, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
//	readStartAddress = stackedMemoryObj->dataPartitioningObj->metadatatoLocalAddress (tempValueForRead);
//
//
//	memoryArrayObj->read(G_ADDRESS_OF_END_ADDRESS, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(tempValueForRead));
//	readEndAdddress = stackedMemoryObj->dataPartitioningObj->metadatatoLocalAddress (tempValueForRead);
//
//	writeStartAddress = readEndAdddress;	// For now, write starts just after the read portion
//
//	readAddressCounter = readStartAddress;
//	//writeAddressCounter = writeStartAddress;
//}
//
//void computSubarray::runPlacementGlobalOneClockCycle(){
//	if(isProceedRead()){
//		LOCAL_ADDRESS_TYPE currentReadValue;
//		memoryArrayObj->read(readAddressCounter, sizeof(FULCRU_WORD_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(currentReadValue));
//
//		FULCRU_WORD_TYPE dstSubForBin = extractBits(currentReadValue, stackedMemoryObj->logTotSubarray - 1, 0);
//		//FULCRU_WORD_TYPE bin = extractBits(currentReadValue, stackedMemoryObj->logTotSubarray + LOG_NUM_BINS, stackedMemoryObj->logTotSubarray);
//
//		//send key
//		dataTransfer* tmpPacket=new dataTransfer(sizeof(FULCRU_WORD_TYPE), dstSubForBin, (READ_DATA_TYPE_IN_MEMORY_ARRAY*) &currentReadValue);
//		incomingPackets.push(tmpPacket);
//
//		readAddressCounter += sizeof(FULCRU_WORD_TYPE);
//		if(readAddressCounter == readEndAdddress){
//			readEnded = true;
//		}
//	}
//
//	if(isProceedWrite()){
//		if(incomingPackets.size()>0){
//			dataTransfer* tmpPacket=incomingPackets.front();
//			incomingPackets.pop();
//
//			if(isDestinationForDataTransfer(tmpPacket)){
//				FULCRU_WORD_TYPE key = *(tmpPacket->payload);
//
//				FULCRU_WORD_TYPE bin = extractBits(key, stackedMemoryObj->logTotSubarray + G_LOG_NUM_BINS, stackedMemoryObj->logTotSubarray);
//
//				LOCAL_ADDRESS_TYPE location = writeStartAddress + (hist[bin].cnt + hist[bin].used) * sizeof(LOCAL_ADDRESS_TYPE);
//				hist[bin].used++;
//
//				memoryArrayObj->write(location, sizeof(FULCRU_WORD_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(key));
//
//				delete tmpPacket;
//			}else{
//				auto nextSubArray=getNextComputeSubArray(tmpPacket);
//				nextSubArray->incomingPackets.push(tmpPacket);
//			}
//		}
//	}
//}

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

