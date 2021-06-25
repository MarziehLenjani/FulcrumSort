/*
 * test.hpp
 *
 *  Created on: Jun 24, 2021
 *      Author: marzieh
 */
#ifndef TEST_HPP
#define TEST_HPP
#include "configAndStats.hpp"
#include "stackedMemory.hpp"
#include "dataPartitioning.hpp"
#include <stdio.h>
#include <iostream>
class tests{
public:
	bool runTestByTestNumber(CONF_TEST_NUMBER_TYPE testNumber,configAndStats *confObj, stackedMemory *stackedMemoryObj ){
		bool testSuccess=false;
		switch (testNumber){

				case 0:{
					std::cout<<"test number 0 should be the main simulator"<<std::endl;

					break;
				}
				case 1:{
					testSuccess=testStatAndConfigFile(confObj);
					break;
				}
				case 2:{
					testSuccess=testDataPartitioningAddressTranslation(confObj, stackedMemoryObj );
					break;
				}
				case 3:{
					testSuccess=testBroadcasting(confObj, stackedMemoryObj );
					break;
				}
				case 4:{
					testSuccess=testNextComputeSubarray(confObj);
					break;
				}
				default:{


					break;
				}
		}
		return testSuccess;
	}


	bool testStatAndConfigFile(configAndStats *confObj){
		bool testSuccess=false;
		long long int clockCycle= 100000;
		float energyConsumption= 0.998;
		confObj->addValueToStatVector("clockCycle",std::to_string(clockCycle));
		confObj->addValueToStatVector("energyInNJ",confObj->toStringScientific(energyConsumption));
		confObj->printStatsAndConfig(false,true );
		confObj->printStatsAndConfig(true,false );
		confObj->printStatsAndConfig(false,false );
		return testSuccess;


	}
	bool testDataPartitioningAddressTranslation(configAndStats *confObj, stackedMemory *stackedMemoryObj ){
		bool testSuccess=false;
		//-----------testing data partitioning APIS
		dataPartitioning dataPartitioningObj(stackedMemoryObj,confObj);
		#ifdef PHYSICAL_EXTERNAL_ADDRESS_TYPE_UNSIGHED_LONG_INT
		PHYSICAL_EXTERNAL_ADDRESS_TYPE address=0xFFFFFFFFFFFFFFFF;
		#else
		PHYSICAL_EXTERNAL_ADDRESS_TYPE address=0xFFFFFFFF;
		#endif
		ID_TYPE stackId;
		ID_TYPE layerId;
		ID_TYPE bankId;
		ID_TYPE computeSubarrayID;
		dataPartitioningObj.globalAddressToLocalAddressTranslation( address,  stackId, layerId, bankId,  computeSubarrayID);
		std::cout<<"address:"<<address<<std::endl;
		std::cout<<"stackId:"<<stackId<<std::endl;
		std::cout<<"layerId:"<<layerId<<std::endl;
		std::cout<<"bankId:"<<bankId<<std::endl;
		std::cout<<"computeSubarrayID:"<<computeSubarrayID<<std::endl;
		return testSuccess;
	}
	bool testBroadcasting(configAndStats *confObj, stackedMemory *stackedMemoryObj ){
		bool testSuccess=false;
		//---------------testing random generate and partitioning among all subarrays
		dataPartitioning dataPartitioningObj(stackedMemoryObj,confObj);
		int minRand=0;
		int maxRand=100;
		int seed=56;
		LOCAL_ADDRESS_TYPE DataAddress= (LOCAL_ADDRESS_TYPE) 64;
		bool writeMetadat=true;
		LOCAL_ADDRESS_TYPE AddressOfTheStartAddress= 0;
		LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress=4;
		LOCAL_ADDRESS_TYPE numOFDataElements =stackedMemoryObj->totNumComputeSubarray*10+7; // to test unequal partitions
		std::cout<<"stackedMemoryObj.totNumComputeSubarray:"<<stackedMemoryObj->totNumComputeSubarray<<std::endl;
		ERROR_RETURN_TYPE ret=dataPartitioningObj.generateRandomlyAndPartitionEquallyAmongAllComputeSubArray (minRand, maxRand, seed, DataAddress, writeMetadat, AddressOfTheStartAddress, AddressOfTheEndAdddress, numOFDataElements );
		LOCAL_ADDRESS_TYPE sizeOFFirstPartition=std::ceil(numOFDataElements*sizeof(int)/stackedMemoryObj->totNumComputeSubarray);
		stackedMemoryObj->computSubarrayVector[0]->memoryArrayObj->printData(DataAddress, sizeOFFirstPartition);
		stackedMemoryObj->computSubarrayVector[0]->memoryArrayObj->printData(AddressOfTheStartAddress, sizeof (LOCAL_ADDRESS_TYPE));
		stackedMemoryObj->computSubarrayVector[0]->memoryArrayObj->printData(AddressOfTheEndAdddress, sizeof (LOCAL_ADDRESS_TYPE));

		return testSuccess;

	}
	bool testNextComputeSubarray(configAndStats *confObj){
		bool testSuccess=false;
		return testSuccess;

	}
};

#endif /* TEST_HPP */
