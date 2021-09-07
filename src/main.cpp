#include <stdio.h>
#include <iostream>
#include "physicalComponent.hpp"
#include "stackedMemory.hpp"
#include "statConfigNamesAndTypes.hpp"
#include "dataPartitioning.hpp"
#include "test.hpp"
#include "computSubarray.hpp"
#include <math.h>
#include <cassert>
#include <algorithm>
#include <random>

using namespace std;

void initDragonfly();

//#ifdef USE_THE_MAIN_MAIN
int main(int argc, char **argv)
{
	if(argc != 2){
		cout << "usage: <elem_per_subarray>" << endl;
		exit(-1);
	}

	u64 G_NUM_OF_DATA_ELEMENTS = atol(argv[1]) * G_NUM_TOTAL_SUBARRAY;

	u64 totalSimCyclesLocalSort = 0;
	u64 totalSimCyclesHistGen = 0;
	u64 totalSimCyclesPrePlacement = 0;
	u64 totalSimCyclesPlacement = 0;
	u64 currStepCycles = 0;
    long long int c=0;

    assert((G_NUM_BANKS_PER_LAYER % 2) == 0);

    //----------------------building a 3d stack component
	stackedMemory stackedMemoryObj((ID_TYPE)0, NULL, NULL, NULL);
	//-----------------------building dataPartitioning object to its APIs
	dataPartitioning dataPartitioningObj(&stackedMemoryObj);

	//----------------
	CONF_TEST_NUMBER_TYPE testNumber = 0;
	initDragonfly();

	if(testNumber==(CONF_TEST_NUMBER_TYPE)0){
		int maxNumClockCycle=100000;

		int minRand = 0;
		int maxRand = ((1UL << G_KEY_BITS) - 1);
		int seed = 56;

		bool writeMetadat=true;

		// Subarray is organized as:
		// [metadata]   [histogram]   [read_array]   [write_array]
		// Read and write array is swapped every iteration

		// [metadata] is organized as:
		// [hist_start_addr] [hist_end_addr] [read_start_addr] [read_end_addr] [write_start_addr] [write_end_addr] ... others

		cout << "Total compute subarrays: " << G_NUM_TOTAL_SUBARRAY << endl;
		cout << "Data elements: " << G_NUM_OF_DATA_ELEMENTS << endl;
		cout << endl << endl;

		placementPacketAllocator = new PacketAllocator<PlacementPacket>(G_NUM_OF_DATA_ELEMENTS);

		// Initialize known meta data
		histStartAddr = G_NUM_BYTES_IN_ROW;		//start of 2nd row
		histEndAddr = histStartAddr + G_NUM_HIST_ELEMS * sizeof(HIST_ELEM_TYPE);

		//read array starts right after the histogram's reserved space
		//read end address is filled by partition function
		u64 readStartAddr = ceil(histEndAddr * 1.0 / G_NUM_BYTES_IN_ROW) * G_NUM_BYTES_IN_ROW;

		//check the alignment of readStartAddr (should be aligned to a row)
		u64 rowMask = G_NUM_BYTES_IN_ROW - 1;
		assert((readStartAddr & rowMask) == 0UL);

		u64 maxElemsPerSubarray = G_NUM_OF_DATA_ELEMENTS / G_NUM_TOTAL_SUBARRAY;
		if(G_NUM_OF_DATA_ELEMENTS % G_NUM_TOTAL_SUBARRAY){
			maxElemsPerSubarray++;
		}
		reservedBytesForReadWriteArray = maxElemsPerSubarray * sizeof(KEY_TYPE);
		//Reserved bytes should be a power of two, and should be aligned to a row.
		//This is to make sure that placement location can be calculated with bitwise operations rather than division/mod.
		reservedBytesForReadWriteArray = getNextPow2(reservedBytesForReadWriteArray, G_NUM_BYTES_IN_ROW);
		locShiftAmt = round(log2(reservedBytesForReadWriteArray / sizeof(KEY_TYPE)));

		u64 writeStartAddr = readStartAddr + reservedBytesForReadWriteArray;

		//check the alignment of writeStartAddr (should be aligned to a row)
		assert((writeStartAddr & rowMask) == 0UL);

		//check if the end of write array fits within the subarray
		assert((writeStartAddr + reservedBytesForReadWriteArray) <=	G_SIZE_OF_SUBARRAY_IN_BYTE);

		for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
			sub->writeData<LOCAL_ADDRESS_TYPE>(G_ADDR_OF_HIST_START_ADDR, histStartAddr);
			sub->writeData<LOCAL_ADDRESS_TYPE>(G_ADDR_OF_HIST_END_ADDR, histEndAddr);
			sub->writeData<LOCAL_ADDRESS_TYPE>(G_ADDR_OF_READ_START_ADDR, readStartAddr);
			sub->writeData<LOCAL_ADDRESS_TYPE>(G_ADDR_OF_WRITE_START_ADDR, writeStartAddr);
		}

		// Initialize Subarrays selfindexes
		stackedMemoryObj.initializeSubarraysSelfindexes();

		ERROR_RETURN_TYPE ret = dataPartitioningObj.generateRandomlyAndPartitionEquallyAmongAllComputeSubArray (minRand, maxRand, seed, readStartAddr, writeMetadat, G_ADDR_OF_READ_START_ADDR, G_ADDR_OF_READ_END_ADDR, G_NUM_OF_DATA_ELEMENTS);

//		cout << "Initial: " << endl;
//		for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
//			sub->printReadElements();
//		}
//		cout << endl << endl;

		for(u64 i = 0; i < 8; i++){
			stateCounter[i] = 0;
		}

		u64 totWaitCyclesInQ = 0;

		for(radixStartBit = 0; radixStartBit < G_KEY_BITS; radixStartBit += G_RADIX_BITS){
			radixEndBit = min(G_KEY_BITS, radixStartBit + G_RADIX_BITS);

			// Make sure parameters are okay
			assert(radixEndBit <= (sizeof(FULCRU_WORD_TYPE) * 8));
			assert(radixStartBit < (sizeof(FULCRU_WORD_TYPE) * 8));
			assert(radixStartBit <= radixEndBit);

			cout << "Processing bits [" << radixEndBit - 1 << " : " << radixStartBit << "]" << endl;

			//----------------------------------------Local Sort----------------------------------------------
			radixSortShift = radixStartBit;
			radixSortMask = radixSortMask = (1UL << radixEndBit) - (1UL << radixSortShift);

			// Only approximately model local sort time
			for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
				sub->runLocalRadixSort();
				//sub->printReadElements();
			}
			currStepCycles = G_LOCAL_SORT_INIT_CYCLES + (reservedBytesForReadWriteArray / sizeof(KEY_TYPE)) * (radixEndBit - radixStartBit) * G_LOCAL_SORT_CYCLES_PER_ELEM_PER_BIT;
			simCycles += currStepCycles;
			totalSimCyclesLocalSort += currStepCycles;
			printSimCycle("\tFinished local sort");

			// Initialization that are needed only once per radix bits
			for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
				sub->initPerRadix();
			}


			//----------------------------------------Range Iteration-----------------------------------------
			for(KEY_TYPE rangeStart = 0; rangeStart < (1UL << G_RADIX_BITS); rangeStart += G_NUM_HIST_ELEMS){
				rangeEnd = rangeStart + G_NUM_HIST_ELEMS;

				cout << "\tRange [" << rangeStart << " : " << rangeEnd << "]" << endl;

				//----------------------------------- Local Histogram ----------------------------------------
//				for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
//					sub->initLocalHistPerRange();
//				}
//
//				numOfProcessedSubarrays = 0;
//				while(numOfProcessedSubarrays != stackedMemoryObj.totNumComputeSubarray){
//					for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
//						sub->runLocalHistOneCycle();
//					}
//					simCycles++;
//				}

				u64 maxElemProcessed = 0;
				for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
					u64 elemProcessed = sub->runLocalHist();
					if(elemProcessed > maxElemProcessed){
						maxElemProcessed = elemProcessed;
					}
				}
				currStepCycles = (maxElemProcessed * G_LOCAL_HIST_CYCLES_PER_ELEM) + (G_LOCAL_HIST_RESET_CYCLES_PER_ELEM * G_NUM_HIST_ELEMS);
				simCycles += currStepCycles;
				totalSimCyclesHistGen += currStepCycles;
				printSimCycle("\t\tFinished building local histogram");
				//for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
				//	sub->printHist();
				//}


				//----------------------------------- Histogram Reduction -------------------------------------
				for(u64 i = 1; i < G_NUM_TOTAL_SUBARRAY; i++){
					HIST_ELEM_TYPE* currSubHist = (HIST_ELEM_TYPE*)(stackedMemoryObj.computSubarrayVector[i]->memoryArrayObj->data + histStartAddr);
					HIST_ELEM_TYPE* prevSubHist = (HIST_ELEM_TYPE*)(stackedMemoryObj.computSubarrayVector[i-1]->memoryArrayObj->data + histStartAddr);

					for(u64 j = 0; j < G_NUM_HIST_ELEMS; j++){
						currSubHist[j] += prevSubHist[j];
					}
				}
				//Approximate the timing of histogram reduction. Can be done because the accesses are sequential.
				currStepCycles = (G_NUM_HIST_ELEMS + G_NUM_TOTAL_SUBARRAY) * G_REDUCTION_PER_HIST_ELEM_CYCLES;
				simCycles += currStepCycles;
				totalSimCyclesHistGen += currStepCycles;

				//Calculate prefix sum of the last subarray
				HIST_ELEM_TYPE prefixSum[G_NUM_HIST_ELEMS];
				prefixSum[0] = lastIdx;
				HIST_ELEM_TYPE* lastSubHist = (HIST_ELEM_TYPE*)(stackedMemoryObj.computSubarrayVector[G_NUM_TOTAL_SUBARRAY-1]->memoryArrayObj->data + histStartAddr);
				for(u64 i = 1; i < G_NUM_HIST_ELEMS; i++){
					prefixSum[i] = prefixSum[i-1] + lastSubHist[i-1];
				}
				lastIdx = prefixSum[G_NUM_HIST_ELEMS - 1] + lastSubHist[G_NUM_HIST_ELEMS - 1];

				//Add offset to get final location
				for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
					HIST_ELEM_TYPE* histArray = (HIST_ELEM_TYPE*)(sub->memoryArrayObj->data + histStartAddr);
					for(u64 i = 0; i < G_NUM_HIST_ELEMS; i++){
						histArray[i] += prefixSum[i];
					}
				}
				//Approximate the timing of offset calculation (we can also ignore this part as the time required would be really small)
				currStepCycles = G_NUM_HIST_ELEMS * G_OFFSET_PER_HIST_ELEM_CYCLES;
				simCycles += currStepCycles;
				totalSimCyclesHistGen += currStepCycles;
				printSimCycle("\t\tFinished reducing histogram");


				//--------------------------------------- Pre-placement ------------------------------------------
//				for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
//					sub->printReadElements();
//				}
				numOfProcessedSubarrays = 0;
				numOfInFlightPackets = 0;
				producedPackets = 0;

				for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
					sub->initPrePlacementPerRange();
				}

				while((numOfProcessedSubarrays != G_NUM_TOTAL_SUBARRAY) || (numOfInFlightPackets != 0)){
					for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
						sub->runPrePlacementConsumerOneCycle();
						sub->runPrePlacementProducerOneCycle();
					}
					simCycles++;
					totalSimCyclesPrePlacement++;
					totWaitCyclesInQ += numOfInFlightPackets;
				}
				//cout << "Produced packets: " << producedPackets << endl;
				printSimCycle("\t\tFinished Pre-placement");
			}

			//--------------------------------------- Placement ------------------------------------------
			numOfProcessedSubarrays = 0;
			for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
				sub->initPlacementPerRadix();
			}

			while(numOfProcessedSubarrays != G_NUM_TOTAL_SUBARRAY){
				for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
					sub->runPlacementOneCycle();
				}
				simCycles++;
				totalSimCyclesPlacement++;
			}
			printSimCycle("\t\tFinished Placement");


			//Prepare for next radix
			//swap src and dst arrays
			//for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
			//	sub->swapReadWriteArray();
			//}
			lastIdx = 0;

//			for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
//				sub->printReadElements();
//			}

			printSimCycle("\tdone");
			cout << endl;
		}

//		cout << "After sort: " << endl;
//		for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
//			sub->printReadElements();
//		}

		printSimCycle("FINISHED!");
		u64 totalCycles = totalSimCyclesLocalSort + totalSimCyclesHistGen + totalSimCyclesPrePlacement + totalSimCyclesPlacement;
		cout << endl;
		cout << "            Total cycles: " << totalCycles << endl;
		cout << "       Local Sort cycles: " << totalSimCyclesLocalSort << " (" << totalSimCyclesLocalSort * 100.0 / totalCycles << " %)" << endl;
		cout << "    Histogram gen cycles: " << totalSimCyclesHistGen << " (" << totalSimCyclesHistGen * 100.0 / totalCycles << " %)" << endl;
		cout << "    Pre-palcement cycles: " << totalSimCyclesPrePlacement << " (" << totalSimCyclesPrePlacement * 100.0 / totalCycles << " %)" << endl;
		cout << "        Placement cycles: " << totalSimCyclesPlacement << " (" << totalSimCyclesPlacement * 100.0 / totalCycles << " %)" << endl;

		delete placementPacketAllocator;

		//Check validity of output
		//1. Merge output of subarrays
		vector<KEY_TYPE> outData;
		outData.reserve(G_NUM_OF_DATA_ELEMENTS);
		for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
			LOCAL_ADDRESS_TYPE startAddr = sub->memoryArrayObj->readLocalAddr(G_ADDR_OF_READ_START_ADDR);
			LOCAL_ADDRESS_TYPE endAddr = sub->memoryArrayObj->readLocalAddr(G_ADDR_OF_READ_END_ADDR);
			while(startAddr < endAddr){
				outData.push_back(sub->memoryArrayObj->readKey(startAddr));
				startAddr += sizeof(KEY_TYPE);
			}
		}

		assert(outData.size() == G_NUM_OF_DATA_ELEMENTS);

		sort(dataArray, dataArray + G_NUM_OF_DATA_ELEMENTS);
		for(u64 i = 0; i < G_NUM_OF_DATA_ELEMENTS; i++){
			if(dataArray[i] != outData[i]){
				cout << "[FAIL at " << i << "] expected: " << dataArray[i] << "       actual: " << outData[i] << endl;
				return -1;
			}
		}

		cout << "State counters: " << endl;
		for(u64 i = 0; i < 8; i++){
			cout << stateCounter[i] * 1.0 / G_NUM_OF_DATA_ELEMENTS << endl;
		}

		cout << endl << "Average hop count: " << hopCounter * 1.0 / G_NUM_OF_DATA_ELEMENTS << endl;
		cout << endl << "Average wait cycles in Q: " << totWaitCyclesInQ * 1.0 / G_NUM_OF_DATA_ELEMENTS << endl;

		//cout << endl << "Placement row accesses: " << placementRowMiss + placementRowHit << endl;
		cout << endl << "Placement row hits: " << placementRowHit - placementRowMiss << endl;
		cout << "Placement row misses: " << placementRowMiss << endl;

		cout << endl << "Max queue size: " << maxQueueLoad << endl;

		cout << "[VALIDITY CHECK PASSED]" << endl;
	}
//	else if(testNumber==(CONF_TEST_NUMBER_TYPE)1){
//		int maxNumClockCycle=1000; //temp required before implementation ends
//
//		//step 1: Filling Subarrays with random data /dataset data using APIS in dataTransfer.hpp
//
//		int minRand=0;
//		int maxRand=100;
//		int seed=56;
//
//		bool writeMetadat=true;
//		//The first row of the Subarray always stores the metadata,
//		LOCAL_ADDRESS_TYPE AddressOfTheStartAddress= 0; //so the start address of the inputData should be written in address zero
//		LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress=4;   //the end address of the inputData should be written in address 4
//		LOCAL_ADDRESS_TYPE DataAddress= (LOCAL_ADDRESS_TYPE) 64; // the input data starts from address 64
//		LOCAL_ADDRESS_TYPE numOFDataElements =stackedMemoryObj.totNumComputeSubarray*10+7; // to test unequal partitions
//		std::cout<<"stackedMemoryObj.totNumComputeSubarray:"<<stackedMemoryObj.totNumComputeSubarray<<std::endl;
//		ERROR_RETURN_TYPE ret=dataPartitioningObj.generateRandomlyAndPartitionEquallyAmongAllComputeSubArray (minRand, maxRand, seed, DataAddress, writeMetadat, AddressOfTheStartAddress, AddressOfTheEndAdddress, numOFDataElements );
//
//		//step 2: Initialize Subarrays selfindexes
//		stackedMemoryObj.initializeSubarraysSelfindexes();
//
//		//step 3: Determine number of iteration that we need on data
//
//		FULCRU_WORD_TYPE numberOfIteration=ceil((sizeof(FULCRU_WORD_TYPE)*8.0)/log2 (stackedMemoryObj.totNumComputeSubarray)); //
//		FULCRU_WORD_TYPE prevNumberOfSubBuckets=0;
//		FULCRU_WORD_TYPE prevNumberOfBuckets=0;
//		FULCRU_WORD_TYPE numberOfShiftsForBucketIDExtraction=sizeof(FULCRU_WORD_TYPE)*8.0;
//		FULCRU_WORD_TYPE maxNumberOfSubBuckets=stackedMemoryObj.totNumComputeSubarray;
//
//		for (FULCRU_WORD_TYPE iter=0; iter<numberOfIteration; iter++ ){
//			//TODO: add the FULCRUM initilaization overhead for each step
//
//			FULCRU_WORD_TYPE numberOfBitsForBucketIDExtraction=std::min (log2 (stackedMemoryObj.totNumComputeSubarray),sizeof(FULCRU_WORD_TYPE)*8.0- (iter*log2 (stackedMemoryObj.totNumComputeSubarray)));
//			numberOfShiftsForBucketIDExtraction-=numberOfBitsForBucketIDExtraction;
//			//FULCRU_WORD_TYPE numberofBucketsInThisIteration=pow(numberOfBitsForBucketIDExtraction,2);
//			FULCRU_WORD_TYPE numberofBucketsInThisIteration=pow(2,numberOfBitsForBucketIDExtraction);
//			FULCRU_WORD_TYPE maskForBucketExtaction = log2(numberofBucketsInThisIteration) - (1 << numberOfShiftsForBucketIDExtraction); //TODO: make sure this is correct
//
//
//			//
//			long long int stepClokCycleCounter=0;
//			FULCRU_WORD_TYPE subBucketCounter;
//			if(iter==0){ // In the first iteration there is no subucket
//
//				stackedMemoryObj.setMaskForBucketIDExtraction( maskForBucketExtaction, numberOfShiftsForBucketIDExtraction);
//				stackedMemoryObj.openANewSubBucket();
//				subBucketCounter=0;
//
//				while(!stackedMemoryObj.checkIfProcessHasEnd() && stepClokCycleCounter<maxNumClockCycle ){
//					stackedMemoryObj.runOneSubClokCycle(); //
//
//					if(stackedMemoryObj.checkIfSubBlockLimitIsReached() & subBucketCounter++<maxNumberOfSubBuckets){ //TODO: do we need to make sure that we have exactly maxNumberOfSubBuckets subBuckets?
//
//						stackedMemoryObj.sealAllSubBuckets();
//						stackedMemoryObj.incrementSubarraysSelfindexes();
//						stackedMemoryObj.openANewSubBucket();
//						subBucketCounter++;
//						stepClokCycleCounter+=100; //TODO: replace 100 with clock cycle required for above tasks or model them in the runOneSubClockCycle
//
//					}else{
//						stepClokCycleCounter++;
//					}
//
//				}
//
//
//			}else{ // after the first iteration subBuckets have been formed.
//				//We need to process buckets of the previous iteration in serial but subBuckets can help paralleize the process
//				for (FULCRU_WORD_TYPE bucketCounter=0; bucketCounter<prevNumberOfBuckets ; bucketCounter++ ){
//
//
//				}
//
//			}
//			prevNumberOfSubBuckets=subBucketCounter;
//			prevNumberOfBuckets=numberofBucketsInThisIteration;
//
//
//
//			std::cout<<"stepClokCycleCounter"<< stepClokCycleCounter<<std::endl;
//			stepClokCycleCounter+=stepClokCycleCounter;
//		}
//	}
	else {
		//bool testsuccess=tstObj.runTestByTestNumber(testNumber,&confObj, &stackedMemoryObj);
	}

}




static void addEdge(u64 a, u64 b){
	dragonEdges[a][b] = true;
	dragonEdges[b][a] = true;
}

void buildNextDst(u64 src){


	u64 len[64];
	for(u64 i = 0; i < 64; i++){
		len[i] = 100000;
	}

	len[src] = 0;

	vector<u64> firstHops;

	for(u64 i = 0; i < 64; i++){
		if(dragonEdges[src][i]){
			firstHops.push_back(i);
			len[i] = 1;
			dragonNextDst[src][i] = i;
		}
	}

	shuffle(firstHops.begin(), firstHops.end(), default_random_engine(42));

	for(u64 fHop : firstHops){
		queue<u64> q;
		q.push(fHop);

		while(!q.empty()){
			u64 a = q.front();
			q.pop();
			const u64 nextLen = len[a] + 1;
			for(u64 b = 0; b < 64; b++){
				if(dragonEdges[a][b]){
					if(len[b] > nextLen){
						len[b] = nextLen;
						q.push(b);
						dragonNextDst[src][b] = fHop;
					}
				}
			}
		}
	}
}

void buildNextDstAll(){
	for(u64 i = 0; i < 64; i++){
		buildNextDst(i);
	}
}


void initDragonfly(){
	if(G_NUM_BANKS_PER_LAYER == 64){
		for(u64 i = 0; i < 64; i++){
			for(u64 j = 0; j < 64; j++){
				dragonEdges[i][j] = false;
				dragonNextDst[i][j] = -1;
			}
		}

		for(u64 i = 0; i < 64; i++){
			if((i % 4) == 0){
				addEdge(i, i + 1);
				addEdge(i, i + 2);
				addEdge(i, i + 3);
			}
			if((i % 4) == 1){
				addEdge(i, i + 1);
				addEdge(i, i + 2);
			}
			if((i % 4) == 2){
				addEdge(i, i + 1);
			}

			if((i % 16) == 1){
				addEdge(i, i + 3);
			}

			if((i % 16) == 2){
				addEdge(i, i + 6);
			}

			if((i % 16) == 11){
				addEdge(i, i + 3);
			}

			if((i % 16) == 7){
				addEdge(i, i + 6);
			}

			if((i % 16) == 3){
				addEdge(i, i + 9);
			}

			if((i % 16) == 6){
				addEdge(i, i + 3);
			}
		}

		addEdge(5, 16);
		addEdge(10, 32);
		addEdge(15, 48);
		addEdge(26, 37);
		addEdge(31, 53);
		addEdge(47, 58);

		//printDot("graph.dot");
		buildNextDstAll();
	}
}

//#endif
