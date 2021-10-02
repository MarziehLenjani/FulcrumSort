/*
 * computSubarray.hpp
 *
 *  Created on: May 16, 2021
 *      Author: marzieh
 */

#ifndef COMPUTSUBARRAY_HPP_
#define COMPUTSUBARRAY_HPP_

#include <cstdlib>
#include <queue>
#include <vector>
#include <algorithm>
#include "Walker.hpp"
#include "globals.hpp"
#include "MemoryObject.hpp"
#include <omp.h>
#include <queue>
#include "Packet.hpp"
#include "PhysicalComponent.hpp"

//#define NUM_WALKERS		3


//class PulleySystem;
//class Device;
//class Stack;
//class Layer;
class Bank;

//class MemoryObject;

class Subarray : public PhysicalComponent{
private:
	static inline u64 extractRowIndexFromLocalAddress(LOCAL_ADDRESS_TYPE localAddr){
		return localAddr / G_NUM_BYTES_IN_ROW;
	}

	static inline u64 extractColCounterFromLocalAddress(LOCAL_ADDRESS_TYPE localAddr){
		return localAddr % G_NUM_BYTES_IN_ROW;
	}

public:

	FULCRU_WORD_TYPE selfIndex = 0;
	KEY_TYPE* keys = nullptr;
	PlacementPacket* placementPackets = nullptr;

	i64 readStartIdx = 0;
	i64 readEndIdx = 0;

	i64 appendIdx = 0;

	u64 currOpenRow = -1;
	bool finishedPlacementRead = false;
	u64 waitCounter = 0;

	LOCAL_ADDRESS_TYPE targetAddr = 0;

//	LOCAL_ADDRESS_TYPE baseWriteAddr = 0;
//	LOCAL_ADDRESS_TYPE currWriteAddr = 0;
//
//
//	LOCAL_ADDRESS_TYPE readEndAddr = 0;
//	LOCAL_ADDRESS_TYPE writeStartAddr = 0;
//	LOCAL_ADDRESS_TYPE writeEndAddr = 0;
//	LOCAL_ADDRESS_TYPE currReadAddr = 0;



	Subarray(ID_TYPE l_id, PhysicalComponent * l_parent);
	~Subarray( );

	//MemoryObject * memoryArrayObj;

	//PulleySystem* pulley;
	//Device* device;
	//Stack * stack;
	//Layer * layer;
	Bank* bank;


	//Walker* walkers[NUM_WALKERS];

	//std::queue <Packet<PlacementPacket>*>* getNextComputeSubArrayQIdeal(ID_TYPE dstId);
	//std::queue <Packet<PlacementPacket>*>* getNextComputeSubArrayQRing(ID_TYPE dstId);
	//std::queue <Packet<PlacementPacket>*>* getNextComputeSubArrayQSemiRing(ID_TYPE dstId);
	//std::queue <Packet<PlacementPacket>*>* getNextComputeSubArrayQCrossbar(ID_TYPE dstId);
	//std::queue <Packet<PlacementPacket>*>* getNextComputeSubArrayQDragonfly(ID_TYPE dstId);

	//std::queue <Packet<PlacementPacket>*>* getNextComputeSubArrayQDragonflyNew(ID_TYPE dstId);


	//--------------To be implemented functions
	FULCRU_WORD_TYPE extractBits(FULCRU_WORD_TYPE val, u32 highBitPos, u32 lowBitPos);


	//void initNextSubarrayRing();
	//void initNextSubarraySemiRing();
	//void initNextSubarrayCrossbar();
	//void initNextSubarrayDragonfly();


	//template <typename T>
	//void writeData(u64 baseAddr, u64 index, const T& val){
	//	memoryArrayObj->writeX<T>(baseAddr + sizeof(T) * index, val);
	//}

//	template <typename T>
//	void writeData(u64 addr, const T& val){
//		writeData<T>(addr, 0, val);
//	}
//
//	template<typename T>
//	void printMem(LOCAL_ADDRESS_TYPE startAddr, LOCAL_ADDRESS_TYPE endAddr);
//
//	void printReadElements();
//	void printReadElements(u64 maxElems);
//
//	void printWriteElements();
//	void printWriteElements(u64 maxElems);
//
//	void printHist();
//	void printHist(u64 maxElems);

	void runLocalRadixSort(){
		std::stable_sort(keys, keys + elemPerSubarray, radixComp);
	}

	void initPerRadix(){
		readStartIdx = 0;
		appendIdx = 0;


		//writeStartAddr = memoryArrayObj->readLocalAddr(G_ADDR_OF_WRITE_START_ADDR);
		//writeEndAddr = writeStartAddr;
	}

	void runLocalHist(HIST_ELEM_TYPE* histogram){
		readEndIdx = readStartIdx;
		while(readEndIdx < elemPerSubarray){
			KEY_TYPE key = keys[readEndIdx];
			FULCRU_WORD_TYPE radix = extractBits(key, radixEndBit, radixStartBit);
			if(radix >= rangeEnd){
				break;
			}
			histogram[radix % G_NUM_HIST_ELEMS]++;
			readEndIdx++;
		}
	}

	enum PlacementSchState {
		//PSTATE_KEY_READ,
		//PSTATE_STALLED_ON_KEY_READ,
		//PSTATE_HIST_READ,
		//PSTATE_STALLED_ON_HIST_READ,
		//PSTATE_HANDLE_QUEUE,
		PSTATE_PLACEMENT,
		PSTATE_STALLED_ON_PLACEMENT,
		PSTATE_INVALID
	} placementSchState = PSTATE_INVALID;


	void prePlacementProducePackets(std::queue <Packet<PlacementPacket>* > &packetQ, HIST_ELEM_TYPE* histogram);

	void appendPacket(PlacementPacket& payload){
		placementPackets[appendIdx++] = payload;
	}


//	void runPrePlacementConsumerOneCycle(){
//		const u64 qSize = incomingPackets.size();
//		if(qSize){
//			if(qSize > maxQueueLoad){
//				maxQueueLoad = qSize;
//			}
//			Packet<PlacementPacket>* tmpPacket = incomingPackets.front();
//			incomingPackets.pop();
//			if(tmpPacket->dstSubAddr == SelfIndex){
//				memoryArrayObj->writeX<PlacementPacket>(writeEndAddr, tmpPacket->payload);
//				writeEndAddr += sizeof(PlacementPacket);
//
//				//done with this packet
//				placementPacketAllocator->free(tmpPacket);
//
//				//#pragma omp atomic
//				numOfInFlightPackets--;
//			}
//			else{
//				//forward packet
//				hopCounter++;
//				//auto nextSubArrayQ = getNextComputeSubArrayQIdeal(tmpPacket->dstSubAddr);
//				//auto nextSubArrayQ = getNextComputeSubArrayQSemiRing(tmpPacket->dstSubAddr);
//				//auto nextSubArrayQ = getNextComputeSubArrayQCrossbar(tmpPacket->dstSubAddr);
//				//auto nextSubArrayQ = getNextComputeSubArrayQDragonfly(tmpPacket->dstSubAddr);
//				auto nextSubArrayQ = getNextComputeSubArrayQDragonflyNew(tmpPacket->dstSubAddr);
//				nextSubArrayQ->push(tmpPacket);
//			}
//		}
//	}
//
//	void runPrePlacementProducerOneCycle(){
//
//		if(!finishedPlacementRead){
//			stateCounter[placementSchState]++;
//
//			switch(placementSchState){
//
//			case PSTATE_KEY_READ:
//			{
//				key = memoryArrayObj->readKey(currReadAddr);
//				FULCRU_WORD_TYPE radix = extractBits(key, radixEndBit, radixStartBit);
//
//				targetAddr = histStartAddr + (radix % G_NUM_HIST_ELEMS) * sizeof(HIST_ELEM_TYPE);
//				placementSchState = PSTATE_HIST_READ;
//			}
//				break;
//
//
//			case PSTATE_HIST_READ:
//			{
//				HIST_ELEM_TYPE location = memoryArrayObj->decrementHist(targetAddr);
//
//				//push into queue
//				u64 dstSub = location >> locShiftAmt;
//				LOCAL_ADDRESS_TYPE dstOff = location & ((1UL << locShiftAmt) - 1);
//
//				Packet<PlacementPacket>* tmpPacket = placementPacketAllocator->alloc();
//				tmpPacket->dstSubAddr = dstSub;
//				tmpPacket->payload.key = key;
//				tmpPacket->payload.offset = dstOff * sizeof(KEY_TYPE);
//
//				incomingPackets.push(tmpPacket);
//				//#pragma omp atomic
//				numOfInFlightPackets++;
//				//producedPackets++;
//
//				// Done with this key, move onto the next key
//				currReadAddr -= sizeof(KEY_TYPE);
//
//				if(currReadAddr < readStartAddrOfRange){
//					//Finished going through all elements in range
//					finishedPlacementRead = true;
//					//#pragma omp atomic
//					numOfProcessedSubarrays++;
//				}
//				else{
//					placementSchState = PSTATE_KEY_READ;
//				}
//			}
//				break;
//
//			default:
//				std::cerr << "Invalid pre-placement state!!" << std::endl;
//				exit(-1);
//			}
//		}
//	}


	void initPlacementPerRadix(){
		finishedPlacementRead = false;

		appendIdx = 0;

		//currReadAddr = writeStartAddr;
		//baseWriteAddr = memoryArrayObj->readLocalAddr(G_ADDR_OF_READ_START_ADDR);

		//assert(writeStartAddr <= writeEndAddr);

//		if(currReadAddr >= writeEndAddr){
//			//Finished going through all elements in range
//			finishedPlacementRead = true;
//			//#pragma omp atomic
//			numOfProcessedSubarrays++;
//		}
//		else {
//			placementSchState = PSTATE_PLACEMENT;
//		}
		placementSchState = PSTATE_PLACEMENT;
	}

	void runPlacementOneCycle();


//	void swapReadWriteArray(){
//		LOCAL_ADDRESS_TYPE tmp;
//
//		//Swap start address
//		tmp = memoryArrayObj->readLocalAddr(G_ADDR_OF_READ_START_ADDR);
//		memoryArrayObj->writeLocalAddr(G_ADDR_OF_READ_START_ADDR, writeStartAddr);
//		memoryArrayObj->writeLocalAddr(G_ADDR_OF_WRITE_START_ADDR, tmp);
//
//		//Set read end address
//		//tmp = memoryArrayObj->readLocalAddr(G_ADDR_OF_READ_END_ADDR);
//		memoryArrayObj->writeLocalAddr(G_ADDR_OF_READ_END_ADDR, writeEndAddr);
//		//memoryArrayObj->writeLocalAddr(G_ADDR_OF_WRITE_END_ADDR, tmp);
//	}

};




#endif /* COMPUTSUBARRAY_HPP_ */
