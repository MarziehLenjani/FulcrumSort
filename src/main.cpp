#include <stdio.h>
#include <iostream>
#include "statConfigNamesAndTypes.hpp"
#include "dataPartitioning.hpp"
#include "test.hpp"
#include "Device.hpp"
#include <math.h>
#include "PulleySystem.hpp"
#include "PhysicalComponent.hpp"
#include "Stack.hpp"
#include "Subarray.hpp"
#include <cassert>
#include <algorithm>
#include <random>
#include <omp.h>

using namespace std;

void initDragonfly();

//#ifdef USE_THE_MAIN_MAIN
int main(int argc, char **argv)
{
	if(argc != 2){
		cout << "usage: <elem_per_subarray>" << endl;
		exit(-1);
	}

	elemPerSubarray = atol(argv[1]);
	u64 G_NUM_OF_DATA_ELEMENTS = elemPerSubarray * G_NUM_TOTAL_SUBARRAY;
	if(G_NUM_OF_DATA_ELEMENTS > 0xFFFFFFFFULL){
		assert(sizeof(HIST_ELEM_TYPE) == 8);
	}

	double totalSimTimeLocalSort = 0;
	double totalSimTimeLocalHistGen = 0;
	double totalSimTimeHistPrefixSum = 0;
	//double totalSimTimePrePlacement = 0;
	double totalSimTimePlacement = 0;
	double currStepTime = 0;

	u64 totNumSubToSubPackets = 0;
	u64 totNumBankToBankPackets = 0;
	u64 totNumSegTSVPackets = 0;
	u64 totNumRowActivations = 0;
	u64 totNumBitwiseOp = 0;
	u64 totNumShiftToSide = 0;
	u64 totNumAdditions = 0;

    assert((G_NUM_BANKS_PER_LAYER % 2) == 0);

	//----------------
	CONF_TEST_NUMBER_TYPE testNumber = 0;
	initDragonfly();

	cout << "Total devices: " << G_NUM_DEVICES << endl;
	cout << "Total stacks: " << G_NUM_TOTAL_STACKS << endl;
	cout << "Total layers: " << G_NUM_TOTAL_LAYERS << endl;
	cout << "Total banks: " << G_NUM_TOTAL_BANKS << endl;
	cout << "Total subarrays: " << G_NUM_TOTAL_SUBARRAY << endl;


	if(testNumber==(CONF_TEST_NUMBER_TYPE)0){

		PulleySystem pulley(0, nullptr);


		//-----------------------building dataPartitioning object to its APIs
		dataPartitioning dataPartitioningObj(&pulley);

		int minRand = 0;
		int maxRand = ((1UL << G_KEY_BITS) - 1);
		int seed = 56;

		bool writeMetadat=true;

		// Subarray is organized as:
		// [metadata]   [histogram]   [read_array]   [write_array]
		// Read and write array is swapped every iteration

		// [metadata] is organized as:
		// [hist_start_addr] [hist_end_addr] [read_start_addr] [read_end_addr] [write_start_addr] [write_end_addr] ... others


		cout << "Total data elements: " << G_NUM_OF_DATA_ELEMENTS << endl;
		cout << endl << endl;

		//placementPacketAllocator = new PacketAllocator<PlacementPacket>(G_NUM_OF_DATA_ELEMENTS);
		packetPool = (Packet<PlacementPacket>*)malloc(G_NUM_OF_DATA_ELEMENTS * sizeof(Packet<PlacementPacket>));

		// Initialize known meta data
		//histStartAddr = G_NUM_BYTES_IN_ROW;		//start of 2nd row
		//histEndAddr = histStartAddr + G_NUM_HIST_ELEMS * sizeof(HIST_ELEM_TYPE);

		//read array starts right after the histogram's reserved space
		//read end address is filled by partition function
		//u64 readStartAddr = ceil(histEndAddr * 1.0 / G_NUM_BYTES_IN_ROW) * G_NUM_BYTES_IN_ROW;

		//check the alignment of readStartAddr (should be aligned to a row)
		//u64 rowMask = G_NUM_BYTES_IN_ROW - 1;
		//assert((readStartAddr & rowMask) == 0UL);

		//u64 maxElemsPerSubarray = G_NUM_OF_DATA_ELEMENTS / G_NUM_TOTAL_SUBARRAY;
		//if(G_NUM_OF_DATA_ELEMENTS % G_NUM_TOTAL_SUBARRAY){
		//	maxElemsPerSubarray++;
		//}
		//reservedBytesForReadWriteArray = maxElemsPerSubarray * sizeof(KEY_TYPE);
		//Reserved bytes should be a power of two, and should be aligned to a row.
		//This is to make sure that placement location can be calculated with bitwise operations rather than division/mod.
		//reservedBytesForReadWriteArray = getNextPow2(reservedBytesForReadWriteArray, G_NUM_BYTES_IN_ROW);
		//locShiftAmt = round(log2(reservedBytesForReadWriteArray / sizeof(KEY_TYPE)));
		locShiftAmt = round(log2(elemPerSubarray));

		//u64 writeStartAddr = readStartAddr + reservedBytesForReadWriteArray;

		//check the alignment of writeStartAddr (should be aligned to a row)
		//assert((writeStartAddr & rowMask) == 0UL);

		//check if the end of write array fits within the subarray
		//assert((writeStartAddr + reservedBytesForReadWriteArray) <=	G_SIZE_OF_SUBARRAY_IN_BYTE);

//		#pragma omp parallel for
//		for(Subarray* sub : pulley.subarrayVector){
//			sub->writeData<LOCAL_ADDRESS_TYPE>(G_ADDR_OF_HIST_START_ADDR, histStartAddr);
//			sub->writeData<LOCAL_ADDRESS_TYPE>(G_ADDR_OF_HIST_END_ADDR, histEndAddr);
//			sub->writeData<LOCAL_ADDRESS_TYPE>(G_ADDR_OF_READ_START_ADDR, readStartAddr);
//			sub->writeData<LOCAL_ADDRESS_TYPE>(G_ADDR_OF_WRITE_START_ADDR, writeStartAddr);
//		}

		ERROR_RETURN_TYPE ret = dataPartitioningObj.generateRandomlyAndPartitionEquallyAmongAllComputeSubArray (minRand, maxRand, seed, G_NUM_OF_DATA_ELEMENTS);

//		cout << "Initial: " << endl;
//		for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
//			sub->printReadElements();
//		}
//		cout << endl << endl;

		for(u64 i = 0; i < 8; i++){
			stateCounter[i] = 0;
		}

		u64 totWaitCyclesInQ = 0;
		u64 rowPerSubForKeys = ceil(1.0 * elemPerSubarray * sizeof(KEY_TYPE) / G_NUM_BYTES_IN_ROW);
		u64 rowPerSubForPlcPkt = ceil(1.0 * elemPerSubarray * sizeof(PlacementPacket) / G_NUM_BYTES_IN_ROW);

#ifdef G_BANK_LEVEL_HISTOGRAM
		u64 rowPerBankForHist = ceil(1.0 * G_NUM_HIST_ELEMS * sizeof(HIST_ELEM_TYPE) / G_NUM_BYTES_IN_ROW);
#endif

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
			#pragma omp parallel for
			for(Subarray* sub : pulley.subarrayVector){
				sub->runLocalRadixSort();
				//sub->printReadElements();
			}
			currStepTime = elemPerSubarray * (radixEndBit - radixStartBit) * G_LOCAL_SORT_CYCLES_PER_ELEM_PER_BIT * G_LOGIC_CLOCK_PERIOD_NS;
			simTimeNs += currStepTime;
			totalSimTimeLocalSort += currStepTime;

			//row activations for local sorting (2 for reading from src and writing to dst)
			totNumRowActivations += rowPerSubForKeys * 2 * (radixEndBit - radixStartBit) * G_NUM_TOTAL_SUBARRAY;

			//have to move all the data elements once to the side of the subarray, then write back
			totNumShiftToSide += G_NUM_OF_DATA_ELEMENTS * 2;

			//bitwise op for local sorting (2 for SHIFT and AND operations (extracting radix))
			totNumBitwiseOp += G_NUM_OF_DATA_ELEMENTS * 2 * (radixEndBit - radixStartBit);

			printSimTime("\tFinished local sort");

			// Initialization that are needed only once per radix bits
			#pragma omp parallel for
			for(Subarray* sub : pulley.subarrayVector){
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

				u64 maxProcessedElems = 0;	//max processed elem by a subarray
				for(Bank* bank : pulley.bankVector){
					u64 procElems = bank->runLocalHist();
					if(procElems > maxProcessedElems){
						maxProcessedElems = procElems;
					}
				}
				cout << "Max processed elem: " << maxProcessedElems << endl;
				//currStepTime = (maxProcessedElems * G_LOCAL_HIST_CYCLES_PER_ELEM) + (G_LOCAL_HIST_RESET_CYCLES_PER_ELEM * G_NUM_HIST_ELEMS);
				currStepTime = (maxProcessedElems * G_LOCAL_HIST_CYCLES_PER_ELEM) * G_LOGIC_CLOCK_PERIOD_NS + (G_NUM_SUBARRAY_PER_BANK - 1) * G_INTCNT_CLOCK_PERIOD_NS;
				simTimeNs += currStepTime;
				totalSimTimeLocalHistGen += currStepTime;
				printSimTime("\t\tFinished building local histogram");
				//for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
				//	sub->printHist();
				//}


				//----------------------------------- Histogram Prefix sum -------------------------------------
//				for(u64 i = 1; i < G_NUM_TOTAL_SUBARRAY; i++){
//					HIST_ELEM_TYPE* currSubHist = (HIST_ELEM_TYPE*)(pulley.subarrayVector[i]->memoryArrayObj->data + histStartAddr);
//					HIST_ELEM_TYPE* prevSubHist = (HIST_ELEM_TYPE*)(pulley.subarrayVector[i-1]->memoryArrayObj->data + histStartAddr);
//
//					for(u64 j = 0; j < G_NUM_HIST_ELEMS; j++){
//						currSubHist[j] += prevSubHist[j];
//					}
//				}

				for(u64 i = 1; i < G_NUM_TOTAL_BANKS; i++){
					HIST_ELEM_TYPE* currBankHist = pulley.bankVector[i]->histogram;
					HIST_ELEM_TYPE* prevBankHist = pulley.bankVector[i-1]->histogram;

					for(u64 j = 0; j < G_NUM_HIST_ELEMS; j++){
						currBankHist[j] += prevBankHist[j];
					}
				}
				//Approximate the timing of histogram reduction
				currStepTime = 	G_NUM_HIST_ELEMS * G_LOGIC_CLOCK_PERIOD_NS
								+ G_NUM_TOTAL_BANKS * (G_LOGIC_CLOCK_PERIOD_NS + G_INTCNT_CLOCK_PERIOD_NS);
				simTimeNs += currStepTime;
				totalSimTimeHistPrefixSum += currStepTime;

				//Calculate prefix sum of the last bank
				HIST_ELEM_TYPE prefixSum[G_NUM_HIST_ELEMS];
				prefixSum[0] = lastIdx;
				HIST_ELEM_TYPE* lastBankHist = pulley.bankVector[G_NUM_TOTAL_BANKS-1]->histogram;
				for(u64 i = 1; i < G_NUM_HIST_ELEMS; i++){
					prefixSum[i] = prefixSum[i-1] + lastBankHist[i-1];
				}
				lastIdx = prefixSum[G_NUM_HIST_ELEMS - 1] + lastBankHist[G_NUM_HIST_ELEMS - 1];

				//Add offset to get final location
//				for(Subarray* sub : pulley.subarrayVector){
//					HIST_ELEM_TYPE* histArray = (HIST_ELEM_TYPE*)(sub->memoryArrayObj->data + histStartAddr);
//					for(u64 i = 0; i < G_NUM_HIST_ELEMS; i++){
//						histArray[i] += prefixSum[i];
//					}
//				}
				#pragma omp parallel for
				for(Bank* bank : pulley.bankVector){
					HIST_ELEM_TYPE* histArray = bank->histogram;
					for(u64 i = 0; i < G_NUM_HIST_ELEMS; i++){
						histArray[i] += prefixSum[i];
					}
				}
				//Approximate the timing of offset calculation (we can also ignore this part as the time required would be really small)
				//currStepCycles = G_NUM_HIST_ELEMS * G_OFFSET_PER_HIST_ELEM_CYCLES;
				//simCycles += currStepCycles;
				//totalSimCyclesHistGen += currStepCycles;
				printSimTime("\t\tFinished reducing histogram");


				//--------------------------------------- Placement ------------------------------------------
				numOfInFlightPackets = 0;
				#pragma omp parallel for
				for(Bank* bank : pulley.bankVector){
					bank->producePackets();
				}

				while(numOfInFlightPackets != 0){
					for(Bank* bank : pulley.bankVector){
						bank->routePacketsOneCycle();
					}
					const vector<Subarray*>& eq = placementEventQ.getCurrEventList();
					for(Subarray* sub : eq){
						sub->checkPlacementBuffer();
					}
//					for(Subarray* sub : pulley.subarrayVector){
//						sub->runPlacementOneCycle();
//					}
#if (G_NUM_STACKS_PER_DEVICE > 1)
					for(LogicLayer* logicLayer : pulley.logicLayerVector){
						logicLayer->runOneCycle();
					}
#endif
#if (G_NUM_DEVICES > 1)
					for(Device* dev : pulley.deviceVector){
						dev->runLinkOneClock();
					}
#endif

					placementEventQ.clock();
					simTimeNs += G_INTCNT_CLOCK_PERIOD_NS;
					totalSimTimePlacement += G_INTCNT_CLOCK_PERIOD_NS;
					//totalSimCyclesPrePlacement++;
					//totWaitCyclesInQ += numOfInFlightPackets;
				}
				//cout << "Produced packets: " << producedPackets << endl;
				printSimTime("\t\tFinished Placement");
			}

			//--------------------------------------- Placement ------------------------------------------

//			numOfProcessedSubarrays = 0;
//			while(numOfProcessedSubarrays != G_NUM_TOTAL_SUBARRAY){
//				#pragma omp parallel for
//				for(Subarray* sub : pulley.subarrayVector){
//					sub->runPlacementOneCycle();
//				}
//				simCycles++;
//				totalSimCyclesPlacement++;
//			}
//
//			printSimCycle("\t\tFinished Placement");


			//Prepare for next radix
			//swap src and dst arrays
			//for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
			//	sub->swapReadWriteArray();
			//}
			lastIdx = 0;

//			for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
//				sub->printReadElements();
//			}
			//numSubToSubPackets += elemPerSubarray * G_NUM_SUBARRAY_PER_BANK * (G_NUM_SUBARRAY_PER_BANK - 1);


			//Energy calculation
#ifdef G_BANK_LEVEL_HISTOGRAM
			assert(G_KEY_BITS == 32); //for now, this part only works for 32-bit keys

			//local hist
			totNumRowActivations += rowPerSubForKeys * G_NUM_TOTAL_SUBARRAY + rowPerBankForHist * G_NUM_TOTAL_BANKS * 16;
			totNumSubToSubPackets += (elemPerSubarray * G_NUM_SUBARRAY_PER_BANK * (G_NUM_SUBARRAY_PER_BANK - 1) / 2) * G_NUM_TOTAL_BANKS;
			totNumBitwiseOp += G_NUM_OF_DATA_ELEMENTS * 2; //for extracting radix (SHIFT + AND)
			totNumAdditions += G_NUM_OF_DATA_ELEMENTS + G_NUM_HIST_ELEMS * 15 * G_NUM_TOTAL_BANKS; //One comparison per element
			totNumShiftToSide += G_NUM_OF_DATA_ELEMENTS * 2 + G_NUM_HIST_ELEMS * 16 * G_NUM_TOTAL_BANKS; //One for keys being sent to the side, one for reduction at the bank


			//prefix sum
			totNumRowActivations += rowPerBankForHist * G_NUM_TOTAL_BANKS * 2; //one for read, one for write
			totNumBankToBankPackets += (G_NUM_HIST_ELEMS - 1) * G_NUM_TOTAL_BANKS;
			totNumSegTSVPackets += (G_NUM_HIST_ELEMS - 1) * G_NUM_LAYERS_PER_STACK * G_NUM_BANKS_PER_LAYER / 2;
			totNumAdditions += (G_NUM_HIST_ELEMS - 1) * G_NUM_TOTAL_BANKS;
			totNumShiftToSide += G_NUM_OF_DATA_ELEMENTS * 2;


			//Placement (fixed portions are added here, rest added during simulation)
			totNumRowActivations += rowPerSubForKeys * G_NUM_TOTAL_SUBARRAY * 2 + rowPerBankForHist * G_NUM_TOTAL_BANKS;	//for packet generation
			//totNumRowActivations += rowPerSubForPlcPkt * G_NUM_TOTAL_SUBARRAY;	//for append
			//bitwise ops: extract radix (SHIFT + AND), determine location (SHIFT for subarray id + AND for offset)
			totNumBitwiseOp += 4 * G_NUM_OF_DATA_ELEMENTS;
			totNumAdditions += G_NUM_OF_DATA_ELEMENTS;	//one subtraction per element
			totNumShiftToSide += G_NUM_OF_DATA_ELEMENTS + G_NUM_HIST_ELEMS * G_NUM_TOTAL_BANKS //move key and histogram to APLU
								+ G_NUM_OF_DATA_ELEMENTS;	//move key from buffer to correct location


#else
#error
#endif


			printSimTime("\tdone");
			cout << endl;
		}

//		cout << "After sort: " << endl;
//		for(computSubarray* sub : stackedMemoryObj.computSubarrayVector){
//			sub->printReadElements();
//		}

		printSimTime("FINISHED!");
		//u64 totalTimeNs = totalSimTimeLocalSort + totalSimTimesHistGen + totalSimTimePlacement;
		cout << endl;
		cout << "            Total time (ns): " << (u64)simTimeNs << endl;
		cout << "       Local Sort time (ns): " << (u64)totalSimTimeLocalSort << " (" << totalSimTimeLocalSort * 100.0 / simTimeNs << " %)" << endl;
		//cout << "   Local Hist gen time (ns): " << (u64)totalSimTimeLocalHistGen << " (" << totalSimTimeLocalHistGen * 100.0 / simTimeNs << " %)" << endl;
		//cout << "  Hist prefix-sum time (ns): " << (u64)totalSimTimeHistPrefixSum << " (" << totalSimTimeHistPrefixSum * 100.0 / simTimeNs << " %)" << endl;
		//cout << "    Pre-palcement cycles: " << totalSimCyclesPrePlacement << " (" << totalSimCyclesPrePlacement * 100.0 / totalCycles << " %)" << endl;
		u64 totHistGenTime = totalSimTimeLocalHistGen + totalSimTimeHistPrefixSum;
		cout << "   Total Hist gen time (ns): " << totHistGenTime << " (" << totHistGenTime * 100.0 / simTimeNs << " %)" << endl;
		cout << "        Placement time (ns): " << (u64)totalSimTimePlacement << " (" << totalSimTimePlacement * 100.0 / simTimeNs << " %)" << endl;

		for(Bank* bank : pulley.bankVector){
			totNumSubToSubPackets += bank->numSubToSubPackets * sizeof(PlacementPacket) / 4;
			totNumBankToBankPackets += bank->numBankToBankPackets  * sizeof(PlacementPacket) / 4;
			totNumSegTSVPackets += bank->numSegTSVPackets  * sizeof(PlacementPacket) / 4;
			totNumRowActivations += bank->numRowActivations;
		}
		cout << endl << "Energy measurements: " << endl;
//		cout << "          row activations: " << totNumRowActivations << endl;
//		cout << "       Sub-to-sub packets: " << totNumSubToSubPackets << endl;
//		cout << "     Bank-to-bank-packets: " << totNumBankToBankPackets << endl;
//		cout << "    Segmented TSV packets: " << totNumSegTSVPackets << endl;
//		cout << "              Bitwise ops: " << totNumBitwiseOp << endl;
//		cout << "                Additions: " << totNumAdditions << endl;
//		cout << "         Shifting to side: " << totNumShiftToSide << endl;
		cout << totNumRowActivations << " ";
		cout <<	totNumSubToSubPackets << " ";
		cout <<	totNumBankToBankPackets << " ";
		cout <<	totNumSegTSVPackets << " ";
		cout <<	totNumBitwiseOp << " ";
		cout <<	totNumAdditions << " ";
		cout <<	totNumShiftToSide << endl;


		//delete placementPacketAllocator;
		free(packetPool);
		packetPool = nullptr;

		//Check validity of output
		sort(dataArray, dataArray + G_NUM_OF_DATA_ELEMENTS);

		//1. Merge output of subarrays
		//vector<KEY_TYPE> outData;
		//outData.reserve(G_NUM_OF_DATA_ELEMENTS);
		u64 currIdx = 0;
		for(Subarray* sub : pulley.subarrayVector){
			for(u64 i = 0; i < elemPerSubarray; i++){
				//outData.push_back(sub->memoryArrayObj->readKey(startAddr));
				KEY_TYPE out = sub->keys[i];
				if(out != dataArray[currIdx]){
					cout << "[FAIL at " << currIdx << "] expected: " << dataArray[currIdx] << "       actual: " << out << endl;
					free(dataArray);
					return -1;
				}
				currIdx++;
			}
		}

		assert(currIdx == G_NUM_OF_DATA_ELEMENTS);

//		cout << "State counters: " << endl;
//		for(u64 i = 0; i < 8; i++){
//			cout << stateCounter[i] * 1.0 / G_NUM_OF_DATA_ELEMENTS << endl;
//		}

//		cout << endl << "Average hop count: " << hopCounter * 1.0 / G_NUM_OF_DATA_ELEMENTS << endl;
//		cout << endl << "Average wait cycles in Q: " << totWaitCyclesInQ * 1.0 / G_NUM_OF_DATA_ELEMENTS << endl;

		//cout << endl << "Placement row accesses: " << placementRowMiss + placementRowHit << endl;
//		cout << endl << "Placement row hits: " << placementRowHit - placementRowMiss << endl;
		//cout << "Maximum placement Q size: " << maxPlacementQSize << endl;

		cout << endl;
		cout << "Throughput (GB/s): " << G_NUM_OF_DATA_ELEMENTS * sizeof(KEY_TYPE) * 1e9 / 1024 / 1024 / 1024 / simTimeNs << endl;
		double totEnergy = 	totNumRowActivations * G_ENR_ROW_ACT
							+ totNumSubToSubPackets * G_ENR_SUB_TO_SUB_PKT
							+ totNumBankToBankPackets * G_ENR_BANK_TO_BANK_PKT
							+ totNumSegTSVPackets * G_ENR_SEG_TSV_PKT
							+ totNumBitwiseOp * G_ENR_BIT_OP
							+ totNumAdditions * G_ENR_INTEGER_ADDITION
							+ totNumShiftToSide * G_ENR_SHIFT_TO_SIDE;
		cout << "Power (W): " << totEnergy / simTimeNs << endl;

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
