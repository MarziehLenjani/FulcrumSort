#include <stdio.h>
#include <iostream>
#include "configAndStats.hpp"
#include "physicalComponent.hpp"
#include "stackedMemory.hpp"
#include "statConfigNamesAndTypes.hpp"
#include "dataPartitioning.hpp"
#include "test.hpp"
#include <math.h>


//#ifdef USE_THE_MAIN_MAIN
int main(int argc, char **argv)
{
    long long int c=0;
	configAndStats confObj;
	confObj.parseOptions(argc, argv);
	tests tstObj;

	//----------------------building a 3d stack component
	stackedMemory stackedMemoryObj((ID_TYPE)0,&confObj, NULL, NULL, NULL);
	//-----------------------building dataPartitioning object to its APIs
	dataPartitioning dataPartitioningObj(&stackedMemoryObj,&confObj);

	//----------------
	CONF_TEST_NUMBER_TYPE testNumber=confObj.getConfig< CONF_TEST_NUMBER_TYPE>(std::string(CONF_TEST_NUMBER_NAME));
	std::cout<<"testNumber is "<<testNumber<<std::endl;

	if(testNumber==(CONF_TEST_NUMBER_TYPE)0){
		int maxNumClockCycle=1000; //temp required before implementation ends

		//step 1: Filling Subarrays with random data /dataset data using APIS in dataTransfer.hpp

		int minRand=0;
		int maxRand=100;
		int seed=56;

		bool writeMetadat=true;
		//The first row of the Subarray always stores the metdadat,
		LOCAL_ADDRESS_TYPE AddressOfTheStartAddress= 0; //so the start address of the inputData should be written in address zero
		LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress=4;   //the end address of the inputData should be written in address 4
		LOCAL_ADDRESS_TYPE DataAddress= (LOCAL_ADDRESS_TYPE) 64; // the input data starts from address 64
		LOCAL_ADDRESS_TYPE numOFDataElements =stackedMemoryObj.totNumComputeSubarray*10+7; // to test unequal partitions
		std::cout<<"stackedMemoryObj.totNumComputeSubarray:"<<stackedMemoryObj.totNumComputeSubarray<<std::endl;
		ERROR_RETURN_TYPE ret=dataPartitioningObj.generateRandomlyAndPartitionEquallyAmongAllComputeSubArray (minRand, maxRand, seed, DataAddress, writeMetadat, AddressOfTheStartAddress, AddressOfTheEndAdddress, numOFDataElements );

		//step 2: Initialize Subarrays selfindexes
		stackedMemoryObj.initializeSubarraysSelfindexes();

		//step 3: Determine number of iteration that we need on data

		FULCRU_WORD_TYPE maskForBucketExtaction=log2 (stackedMemoryObj.totNumComputeSubarray)-1;

		FULCRU_WORD_TYPE numberOfBitsForBucketIDExtraction=log2 (stackedMemoryObj.totNumComputeSubarray);
		FULCRU_WORD_TYPE numberOfShiftsForBucketIDExtraction=0;
		int numberOfIteration=ceil((sizeof(FULCRU_WORD_TYPE)*8.0)/numberOfBitsForBucketIDExtraction); //
		for (int i=0; i<numberOfIteration; i++ ){
			//TODO: add the FULCRUM initilaization overhead for each step
			//
			long long int stepClokCycleCounter=0;
			stackedMemoryObj.setMaskForBucketIDExtraction( maskForBucketExtaction, numberOfShiftsForBucketIDExtraction);
			stackedMemoryObj.openANewSubBucket();
			while(!stackedMemoryObj.checkIfProcessHasEnd() && stepClokCycleCounter<maxNumClockCycle ){
				//stackedMemoryObj.runOneSubClokCycle(); //TODO: fix the segmentation fault

				if(stackedMemoryObj.checkIfSubBlockLimitIsReached()){

					stackedMemoryObj.sealAllSubBuckets();
					stackedMemoryObj.incrementSubarraysSelfindexes();
					stackedMemoryObj.openANewSubBucket();
					stepClokCycleCounter+=100; //TODO: replace 100 with clock cycle required for above tasks or model them in the runOneSubClockCycle

				}else{
					stepClokCycleCounter++;
				}

			}
			maskForBucketExtaction=0;//TODO: make sure this is correct
			numberOfShiftsForBucketIDExtraction+=numberOfBitsForBucketIDExtraction;

			std::cout<<"stepClokCycleCounter"<< stepClokCycleCounter<<std::endl;
			stepClokCycleCounter+=stepClokCycleCounter;
		}
	}else{
		bool testsuccess=tstObj.runTestByTestNumber(testNumber,&confObj, &stackedMemoryObj);
	}



}
//#endif
