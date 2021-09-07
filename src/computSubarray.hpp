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
#include <vector>
#include <algorithm>
#include "Walker.hpp"
#include "globals.hpp"
#include "MemoryObject.hpp"
#include <omp.h>
#include <queue>
#include "Packet.hpp"

#define NUM_WALKERS		3

class configAndStats;
class stackedMemory;
class bank;
class layer;
//class MemoryObject;

class computSubarray:public physicalComponent{
private:
	static inline u64 extractRowIndexFromLocalAddress(LOCAL_ADDRESS_TYPE localAddr){
		return localAddr / G_NUM_BYTES_IN_ROW;
	}

	static inline u64 extractColCounterFromLocalAddress(LOCAL_ADDRESS_TYPE localAddr){
		return localAddr % G_NUM_BYTES_IN_ROW;
	}

public:

	FULCRU_WORD_TYPE SelfIndex = 0;
	std::queue <Packet<PlacementPacket>* > incomingPackets;

	void initialize(LOCAL_ADDRESS_TYPE addressOfTheReadStartAddress,LOCAL_ADDRESS_TYPE addressOfTheReadEndAdddress, LOCAL_ADDRESS_TYPE addressOfTheWriteStartAddress,
			FULCRU_WORD_TYPE t_RegA, FULCRU_WORD_TYPE t_RegB, FULCRU_WORD_TYPE t_SelfIndex ); //

	computSubarray(ID_TYPE l_id, physicalComponent * l_firstDimOwner, physicalComponent * l_secondDimOwner, physicalComponent * l_thirdDimOwner);
	~computSubarray( );
	void runOneSubClokCycle();
	//memoryArray * memoryArrayObj;
	MemoryObject * memoryArrayObj;
	stackedMemory * stackedMemoryObj;
	bank* bankObj;
	layer * layerObj;
	Walker* walkers[NUM_WALKERS];

	std::queue <Packet<PlacementPacket>*>* nextSubarraySameLayerQ = nullptr;
	std::queue <Packet<PlacementPacket>*>* nextSubarrayUpLayerQ = nullptr;
	std::queue <Packet<PlacementPacket>*>* nextSubarrayDownLayerQ = nullptr;


	bool finishedLocalHist = false;
	bool finishedPlacementRead = false;
	bool stalled = false;

	u64 currOpenRow = -1;


	std::queue <Packet<PlacementPacket>*>* getNextComputeSubArrayQIdeal(ID_TYPE dstId);
	std::queue <Packet<PlacementPacket>*>* getNextComputeSubArrayQRing(ID_TYPE dstId);
	std::queue <Packet<PlacementPacket>*>* getNextComputeSubArrayQSemiRing(ID_TYPE dstId);
	std::queue <Packet<PlacementPacket>*>* getNextComputeSubArrayQCrossbar(ID_TYPE dstId);
	std::queue <Packet<PlacementPacket>*>* getNextComputeSubArrayQDragonfly(ID_TYPE dstId);


	//--------------To be implemented functions
	void openANewSubBucket();
	void sealAllSubBuckets();
	void setMaskForBucketIDExtraction(FULCRU_WORD_TYPE maskForBucketExtaction, FULCRU_WORD_TYPE numberOfShiftsForBucketIDExtraction);

	FULCRU_WORD_TYPE extractBits(FULCRU_WORD_TYPE val, u32 highBitPos, u32 lowBitPos);

	bool isProceedRead();
	bool isProceedWrite();

	void initNextSubarrayRing();
	void initNextSubarraySemiRing();
	void initNextSubarrayCrossbar();
	void initNextSubarrayDragonfly();


	template <typename T>
	void writeData(u64 baseAddr, u64 index, const T& val){
		memoryArrayObj->writeX<T>(baseAddr + sizeof(T) * index, val);
	}

	template <typename T>
	void writeData(u64 addr, const T& val){
		writeData<T>(addr, 0, val);
	}

	template<typename T>
	void printMem(LOCAL_ADDRESS_TYPE startAddr, LOCAL_ADDRESS_TYPE endAddr);

	void printReadElements();
	void printReadElements(u64 maxElems);

	void printWriteElements();
	void printWriteElements(u64 maxElems);

	void printHist();
	void printHist(u64 maxElems);


	u64 waitCounter = 0;

	void runLocalRadixSort(){
		LOCAL_ADDRESS_TYPE startAddr = memoryArrayObj->readLocalAddr(G_ADDR_OF_READ_START_ADDR);
		LOCAL_ADDRESS_TYPE endAddr = memoryArrayObj->readLocalAddr(G_ADDR_OF_READ_END_ADDR);

		std::stable_sort((KEY_TYPE*)(memoryArrayObj->data + startAddr), (KEY_TYPE*)(memoryArrayObj->data + endAddr), radixComp);
	}

	LOCAL_ADDRESS_TYPE readStartAddrOfRange = 0;
	LOCAL_ADDRESS_TYPE readEndAddrOfRange = 0;

	LOCAL_ADDRESS_TYPE readEndAddr = 0;
	LOCAL_ADDRESS_TYPE writeStartAddr = 0;
	LOCAL_ADDRESS_TYPE writeEndAddr = 0;

	void initPerRadix(){
		readEndAddrOfRange = memoryArrayObj->readLocalAddr(G_ADDR_OF_READ_START_ADDR);
		readEndAddr = memoryArrayObj->readLocalAddr(G_ADDR_OF_READ_END_ADDR);

		writeStartAddr = memoryArrayObj->readLocalAddr(G_ADDR_OF_WRITE_START_ADDR);
		writeEndAddr = writeStartAddr;
	}

	u64 runLocalHist(){
		u64 elemProcessed = 0;
		memset(memoryArrayObj->data + histStartAddr, 0, G_NUM_HIST_ELEMS * sizeof(HIST_ELEM_TYPE));
		readStartAddrOfRange = readEndAddrOfRange;
		while(readEndAddrOfRange < readEndAddr){
			KEY_TYPE key = memoryArrayObj->readKey(readEndAddrOfRange);
			FULCRU_WORD_TYPE radix = extractBits(key, radixEndBit, radixStartBit);
			if(radix >= rangeEnd){
				break;
			}
			elemProcessed++;
			LOCAL_ADDRESS_TYPE histAddress = histStartAddr + (radix % G_NUM_HIST_ELEMS) * sizeof(HIST_ELEM_TYPE);
			memoryArrayObj->incrementHist(histAddress);
			readEndAddrOfRange += sizeof(KEY_TYPE);
		}
		return elemProcessed;
	}

	LOCAL_ADDRESS_TYPE currReadAddr = 0;
	LOCAL_ADDRESS_TYPE baseWriteAddr = 0;
	LOCAL_ADDRESS_TYPE currWriteAddr = 0;
	LOCAL_ADDRESS_TYPE targetAddr = 0;
	KEY_TYPE key = 0;

	enum PlacementSchState {
		PSTATE_KEY_READ,
		PSTATE_STALLED_ON_KEY_READ,
		PSTATE_HIST_READ,
		PSTATE_STALLED_ON_HIST_READ,
		PSTATE_HANDLE_QUEUE,
		PSTATE_PLACEMENT,
		PSTATE_STALLED_ON_PLACEMENT,
		PSTATE_INVALID
	} placementSchState = PSTATE_INVALID;


	void initPrePlacementPerRange(){
		finishedPlacementRead = false;
		currReadAddr = readEndAddrOfRange - sizeof(KEY_TYPE);

		if(currReadAddr < readStartAddrOfRange){
			//Finished going through all elements in range
			finishedPlacementRead = true;
			//#pragma omp atomic
			numOfProcessedSubarrays++;
			placementSchState = PSTATE_HANDLE_QUEUE;
		}
		else {
			placementSchState = PSTATE_KEY_READ;
		}
	}

	u64 targetRowIndex;


	void runPrePlacementConsumerOneCycle(){
		const u64 qSize = incomingPackets.size();
		if(qSize){
			if(qSize > maxQueueLoad){
				maxQueueLoad = qSize;
			}
			Packet<PlacementPacket>* tmpPacket = incomingPackets.front();
			incomingPackets.pop();
			producedPackets++;
			if(tmpPacket->dstId == SelfIndex){
				memoryArrayObj->writeX<PlacementPacket>(writeEndAddr, tmpPacket->payload);
				writeEndAddr += sizeof(PlacementPacket);

				//done with this packet
				placementPacketAllocator->free(tmpPacket);

				//#pragma omp atomic
				numOfInFlightPackets--;
			}
			else{
				//forward packet
				hopCounter++;
				//auto nextSubArrayQ = getNextComputeSubArrayQSemiRing(tmpPacket->dstId);
				//auto nextSubArrayQ = getNextComputeSubArrayQCrossbar(tmpPacket->dstId);
				auto nextSubArrayQ = getNextComputeSubArrayQDragonfly(tmpPacket->dstId);
				nextSubArrayQ->push(tmpPacket);
			}
		}
	}

	void runPrePlacementProducerOneCycle(){

		if(!finishedPlacementRead){
			stateCounter[placementSchState]++;

			switch(placementSchState){

			case PSTATE_KEY_READ:
			{
				key = memoryArrayObj->readKey(currReadAddr);
				FULCRU_WORD_TYPE radix = extractBits(key, radixEndBit, radixStartBit);

				targetAddr = histStartAddr + (radix % G_NUM_HIST_ELEMS) * sizeof(HIST_ELEM_TYPE);
				placementSchState = PSTATE_HIST_READ;
			}
				break;


			case PSTATE_HIST_READ:
			{
				HIST_ELEM_TYPE location = memoryArrayObj->decrementHist(targetAddr);

				//push into queue
				u64 dstSub = location >> locShiftAmt;
				LOCAL_ADDRESS_TYPE dstOff = location & ((1UL << locShiftAmt) - 1);

				Packet<PlacementPacket>* tmpPacket = placementPacketAllocator->alloc();
				tmpPacket->dstId = dstSub;
				tmpPacket->payload.key = key;
				tmpPacket->payload.offset = dstOff * sizeof(KEY_TYPE);

				incomingPackets.push(tmpPacket);
				//#pragma omp atomic
				numOfInFlightPackets++;
				//producedPackets++;

				// Done with this key, move onto the next key
				currReadAddr -= sizeof(KEY_TYPE);

				if(currReadAddr < readStartAddrOfRange){
					//Finished going through all elements in range
					finishedPlacementRead = true;
					//#pragma omp atomic
					numOfProcessedSubarrays++;
				}
				else{
					placementSchState = PSTATE_KEY_READ;
				}
			}
				break;

			default:
				std::cerr << "Invalid pre-placement state!!" << std::endl;
				exit(-1);
			}
		}
	}



	void initPlacementPerRadix(){
		finishedPlacementRead = false;
		currReadAddr = writeStartAddr;
		baseWriteAddr = memoryArrayObj->readLocalAddr(G_ADDR_OF_READ_START_ADDR);

		assert(writeStartAddr <= writeEndAddr);

		if(currReadAddr >= writeEndAddr){
			//Finished going through all elements in range
			finishedPlacementRead = true;
			//#pragma omp atomic
			numOfProcessedSubarrays++;
		}
		else {
			placementSchState = PSTATE_PLACEMENT;
		}
	}

	void runPlacementOneCycle(){
		if(!finishedPlacementRead){

			stateCounter[placementSchState]++;

			switch(placementSchState){

			case PSTATE_PLACEMENT:
			{
				const PlacementPacket& pkt = memoryArrayObj->readX<PlacementPacket>(currReadAddr);
				targetAddr = baseWriteAddr + pkt.offset;
				targetRowIndex = extractRowIndexFromLocalAddress(targetAddr);

				if(walkers[2]->latchedRowIndex != targetRowIndex){
					placementRowMiss++;
					waitCounter = G_ROW_ACCESS_LATENCY;
					placementSchState = PSTATE_STALLED_ON_PLACEMENT;		//row not latched on walker 0
				}
				else{
					placementRowHit++;
					//already latched
					LOCAL_ADDRESS_TYPE writeColAddr = extractColCounterFromLocalAddress(targetAddr);
					walkers[2]->latchedRow->writeKey(writeColAddr, pkt.key);

					if(targetAddr + sizeof(KEY_TYPE) > memoryArrayObj->readLocalAddr(G_ADDR_OF_READ_END_ADDR)){
						memoryArrayObj->writeLocalAddr(G_ADDR_OF_READ_END_ADDR, targetAddr + sizeof(KEY_TYPE));
					}

					//go to next element
					currReadAddr += sizeof(PlacementPacket);

					if(currReadAddr >= writeEndAddr){
						//Finished going through all elements in range
						finishedPlacementRead = true;
						//#pragma omp atomic
						numOfProcessedSubarrays++;
					}
				}

			}
				break;


			case PSTATE_STALLED_ON_PLACEMENT:
				if(!--waitCounter){
					currOpenRow = targetRowIndex;	//timer expired. Target row is now open.
					walkers[2]->latchedRow->data = memoryArrayObj->data + G_NUM_BYTES_IN_ROW * currOpenRow;
					walkers[2]->latchedRowIndex = currOpenRow;
					placementSchState = PSTATE_PLACEMENT;
				}
				break;


			default:
				std::cerr << "Invalid placement state!!" << std::endl;
				exit(-1);
			}
		}
	}


	void swapReadWriteArray(){
		LOCAL_ADDRESS_TYPE tmp;

		//Swap start address
		tmp = memoryArrayObj->readLocalAddr(G_ADDR_OF_READ_START_ADDR);
		memoryArrayObj->writeLocalAddr(G_ADDR_OF_READ_START_ADDR, writeStartAddr);
		memoryArrayObj->writeLocalAddr(G_ADDR_OF_WRITE_START_ADDR, tmp);

		//Set read end address
		//tmp = memoryArrayObj->readLocalAddr(G_ADDR_OF_READ_END_ADDR);
		memoryArrayObj->writeLocalAddr(G_ADDR_OF_READ_END_ADDR, writeEndAddr);
		//memoryArrayObj->writeLocalAddr(G_ADDR_OF_WRITE_END_ADDR, tmp);
	}

};




#endif /* COMPUTSUBARRAY_HPP_ */
