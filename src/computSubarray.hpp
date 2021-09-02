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


	std::queue <Packet<PlacementPacket>*>* getNextComputeSubArrayQ(ID_TYPE dstId);

	//--------------To be implemented functions
	void openANewSubBucket();
	void sealAllSubBuckets();
	void setMaskForBucketIDExtraction(FULCRU_WORD_TYPE maskForBucketExtaction, FULCRU_WORD_TYPE numberOfShiftsForBucketIDExtraction);

	FULCRU_WORD_TYPE extractBits(FULCRU_WORD_TYPE val, u32 highBitPos, u32 lowBitPos);

	bool isProceedRead();
	bool isProceedWrite();

	void initNextSubarray();

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

//	void initializeLocalSort();
//	void runLocalSortOneClockCycle();

//	static void eventRunLocalSort(void* obj, void* arg);


//	void initLocalRadixSort(){
//
//	}

//	typedef enum {
//		RR_READ,
//		RR_WRITE
//	} RowRequestType;

	typedef struct {
		Walker* walker;
		//RowRequestType reqType;
		u64 rowIndex;
	} RowRequestInfo;

	std::queue<RowRequestInfo> pendingRowAccessQ;

	bool scheduleRowAccess(u64 rowIndex, Walker* walker){ //, RowRequestType reqType){
		if(!walker->isPendingReq){
			walker->isPendingReq = true;
			//pendingRowAccessQ.push({walker, reqType, rowIndex});
			pendingRowAccessQ.push({walker, rowIndex});
			return true;
		}
		return false;
	}

	enum RowSchedulerState {
		SCH_STATE_IDLE,
		SCH_STATE_BUSY,
	} rowSchState = SCH_STATE_IDLE;

	u64 waitCounter = 0;
	//u64

	void rowSchedulerClock(){
		if(pendingRowAccessQ.empty()){
			//nothing to do
			return;
		}

#if (PAGE_POLICY == OPEN_PAGE_POLICY)

		if(rowSchState == SCH_STATE_BUSY){
			if(!--waitCounter){
				//Counter expired. Requested row is now opened.
				RowRequestInfo& currRRInfo = pendingRowAccessQ.front();
				currOpenRow = currRRInfo.rowIndex;
				rowSchState = SCH_STATE_IDLE;
			}
		}

		if(rowSchState == SCH_STATE_IDLE) {
			RowRequestInfo& currRRInfo = pendingRowAccessQ.front();
			if(currOpenRow != currRRInfo.rowIndex){
				//Currently open row is not the requested one.
				//Close current row and open the requested row.
				waitCounter = G_ROW_CLOSE_LATENCY + G_ROW_OPEN_LATENCY;
				rowSchState = SCH_STATE_BUSY;
			}
			else{
				//Row is open
				currRRInfo.walker->latchedRow->data = memoryArrayObj->data + G_NUM_BYTES_IN_ROW * currOpenRow;
				currRRInfo.walker->latchedRowIndex = currOpenRow;

				currRRInfo.walker->isPendingReq = false;
				pendingRowAccessQ.pop();
				stalled = false;
			}
		}

#elif(PAGE_POLICY == CLOSE_PAGE_POLICY)
#error Close page policy is not implemented yet
#else
#error Invalid page policy
#endif

	}

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

	void initLocalHistPerRange(){
		finishedLocalHist = false;
		stalled = false;

		//Reset histogram. No need to worry about simulation cycles as it would be negligible compared to others
		memset(memoryArrayObj->data + histStartAddr, 0, G_NUM_HIST_ELEMS * sizeof(HIST_ELEM_TYPE));

		readStartAddrOfRange = readEndAddrOfRange;
		rowSchState = SCH_STATE_IDLE;
	}

	void runLocalHistOneCycle(){
		rowSchedulerClock();

		if(!finishedLocalHist && !stalled){

			if(readEndAddrOfRange >= readEndAddr){
				//Finished going through all data elements
				finishedLocalHist = true;
				#pragma omp atomic
				numOfProcessedSubarrays++;
				return;
			}

			u64 readRowIndex = extractRowIndexFromLocalAddress(readEndAddrOfRange);
			//u64 readAddressCounter = extractColCounterFromLocalAddress(readStartAddr);
			if(walkers[0]->latchedRowIndex != readRowIndex){
				//schedule current row read in walker 0
				scheduleRowAccess(readRowIndex, walkers[0]);

				////schedule next row read in walker 1 in advance
				//scheduleRowAccess(readRowIndex + 1, walkers[1]);

				stalled = true;

				return;
			}

			//walker 0 now contains required row
			u64 readColAddr = extractColCounterFromLocalAddress(readEndAddrOfRange);
			KEY_TYPE key = walkers[0]->latchedRow->readKey(readColAddr);
			FULCRU_WORD_TYPE radix = extractBits(key, radixEndBit, radixStartBit);

			if(radix >= rangeEnd){
				//done building local histogram for this range
				finishedLocalHist = true;
				#pragma omp atomic
				numOfProcessedSubarrays++;
				return;
			}


			LOCAL_ADDRESS_TYPE histAddress = histStartAddr + (radix % G_NUM_HIST_ELEMS) * sizeof(HIST_ELEM_TYPE);
			u64 histRowIndex = extractRowIndexFromLocalAddress(histAddress);
			if(walkers[1]->latchedRowIndex != histRowIndex){
				//schedule histogram row to be latched in walker 1
				scheduleRowAccess(histRowIndex, walkers[1]);

				stalled = true;

				return;
			}

			LOCAL_ADDRESS_TYPE histColAddr = extractColCounterFromLocalAddress(histAddress);
			HIST_ELEM_TYPE currHistVal = walkers[1]->latchedRow->readHist(histColAddr);
			walkers[1]->latchedRow->writeHist(histColAddr, currHistVal + 1);

			// Done with this key, move onto the next key
			readEndAddrOfRange += sizeof(KEY_TYPE);
		}
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
	LOCAL_ADDRESS_TYPE targetAddr = 0;
	KEY_TYPE key = 0;

	enum PlacementSchState {
		PSTATE_KEY_READ,
		PSTATE_STALLED_ON_KEY_READ,
		PSTATE_HIST_READ,
		PSTATE_STALLED_ON_HIST_READ,
		PSTATE_HANDLE_QUEUE,
		PSTATE_STALLED_ON_PLACEMENT,
		PSTATE_INVALID
	} placementSchState = PSTATE_INVALID;


	void initPlacementPerRange(){
		finishedPlacementRead = false;
		stalled = false;
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



//	void runPlacementReadOneCycle(){
//		// We read from the end towards the beginning to maintain stable placement of data elements
//
//		if(currReadAddr < readStartAddrOfRange){
//			//Finished going through all elements in range
//			finishedPlacementRead = true;
//			placementState = PLACEMENT_STATE_HANDLE_QUEUE;
//			//#pragma omp atomic
//			numOfProcessedSubarrays++;
//			return;
//		}
//
//		u64 readRowIndex = extractRowIndexFromLocalAddress(currReadAddr);
//		if(walkers[0]->latchedRowIndex != readRowIndex){
//			//schedule current row read in walker 0
//			scheduleRowAccess(readRowIndex, walkers[0]);
//
//			stalled = true;
//			return;
//		}
//
//		//walker 0 now contains required row
//		u64 readColAddr = extractColCounterFromLocalAddress(currReadAddr);
//		key = walkers[0]->latchedRow->readKey(readColAddr);
//		FULCRU_WORD_TYPE radix = extractBits(key, radixEndBit, radixStartBit);
//
//		LOCAL_ADDRESS_TYPE histAddress = histStartAddr + (radix % G_NUM_HIST_ELEMS) * sizeof(HIST_ELEM_TYPE);
//		u64 histRowIndex = extractRowIndexFromLocalAddress(histAddress);
//		if(walkers[1]->latchedRowIndex != histRowIndex){
//			//schedule histogram row to be latched in walker 1
//			scheduleRowAccess(histRowIndex, walkers[1]);
//
//			stalled = true;
//			return;
//		}
//
//		LOCAL_ADDRESS_TYPE histColAddr = extractColCounterFromLocalAddress(histAddress);
//		HIST_ELEM_TYPE location = walkers[1]->latchedRow->readHist(histColAddr);
//		location--;
//		walkers[1]->latchedRow->writeHist(histColAddr, location);
//
//		//push into queue
//		const u64 numOfWriteElementsPerSub = (reservedBytesForReadWriteArray / sizeof(KEY_TYPE));
//		u64 dstSub = location / numOfWriteElementsPerSub;
//		LOCAL_ADDRESS_TYPE dstOff = location % numOfWriteElementsPerSub;
//
//		PlacementPacket pkt = {key, (LOCAL_ADDRESS_TYPE)(dstOff * sizeof(KEY_TYPE))};
//
//		dataTransfer* tmpPacket = new dataTransfer(sizeof(PlacementPacket), dstSub, (READ_DATA_TYPE_IN_MEMORY_ARRAY*)&pkt);
//		incomingPackets.push(tmpPacket);
//		//#pragma omp atomic
//		numOfInFlightPackets++;
//
//		// Done with this key, move onto the next key
//		currReadAddr -= sizeof(KEY_TYPE);
//
//		if(incomingPackets.size() >= PLACEMENT_QUEUE_HIGH_WATER_MARK){
//			//queue almost full, switch to handle packets
//			placementState = PLACEMENT_STATE_HANDLE_QUEUE;
//		}
//	}

//	void runPlacementQueueOneCycle(){
//		if(incomingPackets.size()){
//			dataTransfer* tmpPacket = incomingPackets.front();
//			if(isDestinationForDataTransfer(tmpPacket)){
//				PlacementPacket* pkt = (PlacementPacket*)(tmpPacket->payload);
//				LOCAL_ADDRESS_TYPE writeAddr = writeStartAddr + pkt->offset;
//				u64 writeRowIndex = extractRowIndexFromLocalAddress(writeAddr);
//				if(walkers[2]->latchedRowIndex != writeRowIndex){
//					scheduleRowAccess(writeRowIndex, walkers[2]);
//					stalled = true;
//					return;
//				}
//
//				LOCAL_ADDRESS_TYPE writeColAddr = extractColCounterFromLocalAddress(writeAddr);
//				walkers[2]->latchedRow->writeKey(writeColAddr, pkt->key);
//
//				if(writeAddr >= writeEndAddr){
//					writeEndAddr = writeAddr + sizeof(KEY_TYPE);
//				}
//
//				//done with this packet
//				incomingPackets.pop();
//				delete tmpPacket;
//				//#pragma omp atomic
//				numOfInFlightPackets--;
//			}
//			else{
//				//forward packet
//				incomingPackets.pop();
//				std::queue <dataTransfer*>* nextSubArrayQ = getNextComputeSubArrayQ(tmpPacket);
//				nextSubArrayQ->push(tmpPacket);
//			}
//		}
//
//		if((incomingPackets.size() <= PLACEMENT_QUEUE_LOW_WATER_MARK) && !finishedPlacementRead){
//			//queue almost empty, and not all elements are read yet. Switch to read elements.
//			placementState = PLACEMENT_STATE_HANDLE_READ;
//			return;
//		}
//	}


//	void runPlacementOneCycle(){
//		rowSchedulerClock();
//		if(!finishedPlacementRead && !stalled && (placementState == PLACEMENT_STATE_HANDLE_READ)){
//			runPlacementReadOneCycle();
//		}
//		else if(!stalled && (placementState == PLACEMENT_STATE_HANDLE_QUEUE)){
//			runPlacementQueueOneCycle();
//		}
//	}

	void runPlacementOneCycleV1(){

//		stateCounter[placementSchState]++;
//
//		switch(placementSchState){
//
//		case PSTATE_CHECK_RANGE:
//			if(currReadAddr < readStartAddrOfRange){
//				//Finished going through all elements in range
//				finishedPlacementRead = true;
//				//#pragma omp atomic
//				numOfProcessedSubarrays++;
//				placementSchState = PSTATE_HANDLE_QUEUE;
//			}
//			else {
//				targetRowIndex = extractRowIndexFromLocalAddress(currReadAddr);
//				if(walkers[0]->latchedRowIndex != targetRowIndex){
//					if(targetRowIndex == currOpenRow){
//						walkers[0]->latchedRow->data = memoryArrayObj->data + G_NUM_BYTES_IN_ROW * currOpenRow;
//						walkers[0]->latchedRowIndex = currOpenRow;
//						placementSchState = PSTATE_KEY_READ;		//already open
//					}
//					else{
//						waitCounter = G_ROW_ACCESS_LATENCY;
//						placementSchState = PSTATE_STALLED_ON_KEY_READ;		//row not latched on walker 0
//					}
//				}
//				else{
//					placementSchState = PSTATE_KEY_READ;		//already latched
//				}
//			}
//			break;
//
//		case PSTATE_STALLED_ON_KEY_READ:
//			if(!--waitCounter){
//				currOpenRow = targetRowIndex;	//timer expired. Target row is now open.
//				walkers[0]->latchedRow->data = memoryArrayObj->data + G_NUM_BYTES_IN_ROW * currOpenRow;
//				walkers[0]->latchedRowIndex = currOpenRow;
//				placementSchState = PSTATE_KEY_READ;
//			}
//			break;
//
//		case PSTATE_KEY_READ:
//		{
//			//walker 0 now contains required row
//			u64 readColAddr = extractColCounterFromLocalAddress(currReadAddr);
//			key = walkers[0]->latchedRow->readKey(readColAddr);
//			FULCRU_WORD_TYPE radix = extractBits(key, radixEndBit, radixStartBit);
//
//			targetAddr = histStartAddr + (radix % G_NUM_HIST_ELEMS) * sizeof(HIST_ELEM_TYPE);
//			targetRowIndex = extractRowIndexFromLocalAddress(targetAddr);
//			if(walkers[1]->latchedRowIndex != targetRowIndex){
//				if(targetRowIndex == currOpenRow){
//					walkers[1]->latchedRow->data = memoryArrayObj->data + G_NUM_BYTES_IN_ROW * currOpenRow;
//					walkers[1]->latchedRowIndex = currOpenRow;
//					placementSchState = PSTATE_HIST_READ;		//already open
//				}
//				else{
//					waitCounter = G_ROW_ACCESS_LATENCY;
//					placementSchState = PSTATE_STALLED_ON_HIST_READ;		//row not latched on walker 0
//				}
//			}
//			else{
//				placementSchState = PSTATE_HIST_READ;	//already latched
//			}
//		}
//			break;
//
//		case PSTATE_STALLED_ON_HIST_READ:
//			if(!--waitCounter){
//				currOpenRow = targetRowIndex;	//timer expired. Target row is now open.
//				walkers[1]->latchedRow->data = memoryArrayObj->data + G_NUM_BYTES_IN_ROW * currOpenRow;
//				walkers[1]->latchedRowIndex = currOpenRow;
//				placementSchState = PSTATE_HIST_READ;
//			}
//			break;
//
//		case PSTATE_HIST_READ:
//		{
//			LOCAL_ADDRESS_TYPE histColAddr = extractColCounterFromLocalAddress(targetAddr);
//			HIST_ELEM_TYPE location = walkers[1]->latchedRow->readHist(histColAddr);
//			location--;
//			walkers[1]->latchedRow->writeHist(histColAddr, location);
//
//			//push into queue
//			const u64 numOfWriteElementsPerSub = (reservedBytesForReadWriteArray / sizeof(KEY_TYPE));
//			u64 dstSub = location / numOfWriteElementsPerSub;
//			LOCAL_ADDRESS_TYPE dstOff = location % numOfWriteElementsPerSub;
//
//			PlacementPacket pkt = {key, (LOCAL_ADDRESS_TYPE)(dstOff * sizeof(KEY_TYPE))};
//			dataTransfer* tmpPacket = new dataTransfer(sizeof(PlacementPacket), dstSub, (READ_DATA_TYPE_IN_MEMORY_ARRAY*)&pkt);
//			incomingPackets.push(tmpPacket);
//			//#pragma omp atomic
//			numOfInFlightPackets++;
//
//			// Done with this key, move onto the next key
//			currReadAddr -= sizeof(KEY_TYPE);
//
//			if(incomingPackets.size() >= PLACEMENT_QUEUE_HIGH_WATER_MARK){
//				//queue almost full, switch to handle packets
//				placementSchState = PSTATE_HANDLE_QUEUE;
//			}
//			else{
//				placementSchState = PSTATE_CHECK_RANGE;
//			}
//		}
//			break;
//
//		case PSTATE_HANDLE_QUEUE:
//			if(!incomingPackets.empty()){
//				tmpPacket = incomingPackets.front();
//				incomingPackets.pop();
//				if(isDestinationForDataTransfer(tmpPacket)){
//					PlacementPacket* pkt = (PlacementPacket*)(tmpPacket->payload);
//					targetAddr = writeStartAddr + pkt->offset;
//					targetRowIndex = extractRowIndexFromLocalAddress(targetAddr);
//
//					if(walkers[2]->latchedRowIndex != targetRowIndex){
//						if(targetRowIndex == currOpenRow){
//							walkers[2]->latchedRow->data = memoryArrayObj->data + G_NUM_BYTES_IN_ROW * currOpenRow;
//							walkers[2]->latchedRowIndex = currOpenRow;
//							placementSchState = PSTATE_DO_PLACEMENT;		//already open
//						}
//						else{
//							waitCounter = G_ROW_ACCESS_LATENCY;
//							placementSchState = PSTATE_STALLED_ON_PLACEMENT;		//row not latched on walker 0
//						}
//					}
//					else{
//						placementSchState = PSTATE_DO_PLACEMENT;	//already latched
//					}
//				}
//				else{
//					//forward packet
//					hopCounter++;
//					auto nextSubArrayQ = getNextComputeSubArrayQ(tmpPacket);
//					//if(nextSubArrayQ->size() >= PLACEMENT_QUEUE_MAX_CAPACITY){
//					//	placementSchState = PSTATE_STALLED_ON_QUEUE_PUSH;
//					//}
//					//else{
//						nextSubArrayQ->push(tmpPacket);
//						placementSchState = PSTATE_CHECK_QUEUE_LOW_WATERMARK;
//					//}
//				}
//			}
//			else if(!finishedPlacementRead){
//				//queue empty
//				placementSchState = PSTATE_CHECK_RANGE;
//			}
//			break;
//
//		case PSTATE_STALLED_ON_PLACEMENT:
//			if(!--waitCounter){
//				currOpenRow = targetRowIndex;	//timer expired. Target row is now open.
//				walkers[2]->latchedRow->data = memoryArrayObj->data + G_NUM_BYTES_IN_ROW * currOpenRow;
//				walkers[2]->latchedRowIndex = currOpenRow;
//				placementSchState = PSTATE_DO_PLACEMENT;
//			}
//			break;
//
//
//		case PSTATE_DO_PLACEMENT:
//		{
//			LOCAL_ADDRESS_TYPE writeColAddr = extractColCounterFromLocalAddress(targetAddr);
//			PlacementPacket* pkt = (PlacementPacket*)(tmpPacket->payload);
//			walkers[2]->latchedRow->writeKey(writeColAddr, pkt->key);
//			if(targetAddr >= writeEndAddr){
//				writeEndAddr = targetAddr + sizeof(KEY_TYPE);
//			}
//
//			//done with this packet
//			delete tmpPacket;
//			//#pragma omp atomic
//			numOfInFlightPackets--;
//			placementSchState = PSTATE_CHECK_QUEUE_LOW_WATERMARK;
//		}
//			break;
//
//
////		case PSTATE_STALLED_ON_QUEUE_PUSH:
////		{
////			auto nextSubArrayQ = getNextComputeSubArrayQ(tmpPacket);
////			if(nextSubArrayQ->size() < PLACEMENT_QUEUE_MAX_CAPACITY){
////				nextSubArrayQ->push(tmpPacket);
////				placementSchState = PSTATE_CHECK_QUEUE_LOW_WATERMARK;
////			}
////		}
////			break;
//
//		case PSTATE_CHECK_QUEUE_LOW_WATERMARK:
//			if((incomingPackets.size() <= PLACEMENT_QUEUE_LOW_WATER_MARK) && !finishedPlacementRead){
//				//queue almost empty, and not all elements are read yet. Switch to read elements.
//				placementSchState = PSTATE_CHECK_RANGE;
//			}
//			else{
//				placementSchState = PSTATE_HANDLE_QUEUE;
//			}
//			break;
//
//
//		default:
//			std::cerr << "Invalid placement state!!" << std::endl;
//			exit(-1);
//		}
	}



	void runPlacementOneCycleV2(){

		stateCounter[placementSchState]++;

		switch(placementSchState){

		case PSTATE_KEY_READ:
			targetRowIndex = extractRowIndexFromLocalAddress(currReadAddr);
			if(walkers[0]->latchedRowIndex == targetRowIndex){
				//walker 0 now contains required row
				u64 readColAddr = extractColCounterFromLocalAddress(currReadAddr);
				key = walkers[0]->latchedRow->readKey(readColAddr);
				FULCRU_WORD_TYPE radix = extractBits(key, radixEndBit, radixStartBit);

				targetAddr = histStartAddr + (radix % G_NUM_HIST_ELEMS) * sizeof(HIST_ELEM_TYPE);
				targetRowIndex = extractRowIndexFromLocalAddress(targetAddr);
				if(walkers[1]->latchedRowIndex != targetRowIndex){
					waitCounter = G_ROW_ACCESS_LATENCY;
					placementSchState = PSTATE_STALLED_ON_HIST_READ;		//row not latched on walker 0
				}
				else{
					placementSchState = PSTATE_HIST_READ;	//already latched
				}
			}
			else{
				waitCounter = G_ROW_ACCESS_LATENCY;
				placementSchState = PSTATE_STALLED_ON_KEY_READ;		//row not latched on walker 0
			}

			break;


		case PSTATE_STALLED_ON_KEY_READ:
			if(!--waitCounter){
				currOpenRow = targetRowIndex;	//timer expired. Target row is now open.
				walkers[0]->latchedRow->data = memoryArrayObj->data + G_NUM_BYTES_IN_ROW * currOpenRow;
				walkers[0]->latchedRowIndex = currOpenRow;
				placementSchState = PSTATE_KEY_READ;
			}
			break;


		case PSTATE_HIST_READ:
		{
			LOCAL_ADDRESS_TYPE histColAddr = extractColCounterFromLocalAddress(targetAddr);
			HIST_ELEM_TYPE location = walkers[1]->latchedRow->decrementHist(histColAddr);

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

			// Done with this key, move onto the next key
			currReadAddr -= sizeof(KEY_TYPE);

			if(currReadAddr < readStartAddrOfRange){
				//Finished going through all elements in range
				finishedPlacementRead = true;
				//#pragma omp atomic
				numOfProcessedSubarrays++;
				placementSchState = PSTATE_HANDLE_QUEUE;
			}
			else if(incomingPackets.size() >= PLACEMENT_QUEUE_HIGH_WATER_MARK){
				//queue almost full, switch to handle packets
				placementSchState = PSTATE_HANDLE_QUEUE;
			}
			else{
				placementSchState = PSTATE_KEY_READ;		//read next element
			}
		}
			break;


		case PSTATE_STALLED_ON_HIST_READ:
			if(!--waitCounter){
				currOpenRow = targetRowIndex;	//timer expired. Target row is now open.
				walkers[1]->latchedRow->data = memoryArrayObj->data + G_NUM_BYTES_IN_ROW * currOpenRow;
				walkers[1]->latchedRowIndex = currOpenRow;
				placementSchState = PSTATE_HIST_READ;
			}
			break;


		case PSTATE_HANDLE_QUEUE:
			if(!incomingPackets.empty()){
				Packet<PlacementPacket>* tmpPacket = incomingPackets.front();
				//incomingPackets.pop();
				if(tmpPacket->dstId == SelfIndex){
					targetAddr = writeStartAddr + tmpPacket->payload.offset;
					targetRowIndex = extractRowIndexFromLocalAddress(targetAddr);

					if(walkers[2]->latchedRowIndex != targetRowIndex){
						waitCounter = G_ROW_ACCESS_LATENCY;
						placementSchState = PSTATE_STALLED_ON_PLACEMENT;		//row not latched on walker 0
					}
					else{
						//already latched
						LOCAL_ADDRESS_TYPE writeColAddr = extractColCounterFromLocalAddress(targetAddr);
						walkers[2]->latchedRow->writeKey(writeColAddr, tmpPacket->payload.key);
						if(targetAddr >= writeEndAddr){
							writeEndAddr = targetAddr + sizeof(KEY_TYPE);
						}

						//done with this packet
						placementPacketAllocator->free(tmpPacket);

						//#pragma omp atomic
						numOfInFlightPackets--;
						incomingPackets.pop();
					}
				}
				else{
					//forward packet
					hopCounter++;
					auto nextSubArrayQ = getNextComputeSubArrayQ(tmpPacket->dstId);
					incomingPackets.pop();
					nextSubArrayQ->push(tmpPacket);
				}
			}

			if(!finishedPlacementRead && (incomingPackets.size() <= PLACEMENT_QUEUE_LOW_WATER_MARK)){
				//queue almost empty, and not all elements are read yet. Switch to read elements.
				placementSchState = PSTATE_KEY_READ;
			}
			break;


		case PSTATE_STALLED_ON_PLACEMENT:
			if(!--waitCounter){
				currOpenRow = targetRowIndex;	//timer expired. Target row is now open.
				walkers[2]->latchedRow->data = memoryArrayObj->data + G_NUM_BYTES_IN_ROW * currOpenRow;
				walkers[2]->latchedRowIndex = currOpenRow;
				placementSchState = PSTATE_HANDLE_QUEUE;
			}
			break;


		default:
			std::cerr << "Invalid placement state!!" << std::endl;
			exit(-1);
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
