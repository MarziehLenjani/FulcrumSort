/*
 * dataPartitioning.hpp
 *
 *  Created on: May 17, 2021
 *      Author: marzieh
 */

#ifndef DATAPARTITIONING_HPP_
#define DATAPARTITIONING_HPP_
#include "types.hpp"
#include "statConfigNamesAndTypes.hpp"
#include "statConfigNamesAndTypes.hpp"
#include <stdio.h>
#include <iostream>
#include "types.hpp"
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include "PhysicalComponent.hpp"
#include "Stack.hpp"
#include <cstring>
#include "PulleySystem.hpp"

class dataPartitioning{
	public:
	//Stack * stackObj;
	//dataPartitioning(Stack * l_stackObj);
	PulleySystem* devices;
	dataPartitioning(PulleySystem* devices);
	ERROR_RETURN_TYPE globalAddressToLocalAddressTranslation(PHYSICAL_EXTERNAL_ADDRESS_TYPE address, ID_TYPE & stackId, ID_TYPE & layerId, ID_TYPE & bankId, ID_TYPE & computeSubarrayID);
	//ERROR_RETURN_TYPE broadcastDataToAllComputeSubArray (LOCAL_ADDRESS_TYPE DataAddress, bool writeMetadat, LOCAL_ADDRESS_TYPE AddressOfTheStartAddress, LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress, READ_DATA_TYPE_IN_MEMORY_ARRAY* broadcastedData, LOCAL_ADDRESS_TYPE sizeOFData );

	ERROR_RETURN_TYPE partitionEquallyAmongAllComputeSubArray (KEY_TYPE* dataToBePartitionedData, u64 numOfDataElements ){
		ERROR_RETURN_TYPE ret;
		u64 eachPartition = numOfDataElements / G_NUM_TOTAL_SUBARRAY;
		u64 remainingElems =  numOfDataElements - eachPartition * G_NUM_TOTAL_SUBARRAY;

		assert(eachPartition == elemPerSubarray);

		//assert(remainingElems < G_NUM_TOTAL_SUBARRAY);
		assert(remainingElems == 0);

		u64 totElemDistributed = 0;	//just for checking if all data elements are distributed
		for(Subarray* sub : devices->subarrayVector){
			u64 numDataElementsInSubarray = eachPartition;
			if(remainingElems){
				numDataElementsInSubarray++;
				remainingElems--;
			}

			memcpy(sub->keys, &dataToBePartitionedData[totElemDistributed], numDataElementsInSubarray * sizeof(KEY_TYPE));

			//ptr->memoryArrayObj->write(DataAddress, numDataElementsInSubarray * sizeof(KEY_TYPE), dataToBePartitionedData + totElemDistributed * sizeof(KEY_TYPE));
			totElemDistributed += numDataElementsInSubarray;
			//if(writeMetadat){
				//LOCAL_ADDRESS_TYPE startAaddress=localAddressToLocalMetadata(DataAddress);
				//ptr->memoryArrayObj->write(AddressOfTheStartAddress, sizeof(LOCAL_ADDRESS_TYPE), (READ_DATA_TYPE_IN_MEMORY_ARRAY*) (& startAaddress));
				//LOCAL_ADDRESS_TYPE endAaddress=localAddressToLocalMetadata(DataAddress + numDataElementsInSubarray * sizeof(KEY_TYPE));
				//ptr->memoryArrayObj->write(AddressOfTheEndAdddress, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) (& endAaddress ));
				//ptr->memoryArrayObj->write32(AddressOfTheStartAddress, DataAddress);
				//ptr->memoryArrayObj->write32(AddressOfTheEndAdddress, DataAddress + numDataElementsInSubarray * sizeof(KEY_TYPE));
			//}
		}
		assert(totElemDistributed == numOfDataElements);
		return ret;
	}


	ERROR_RETURN_TYPE generateRandomlyAndPartitionEquallyAmongAllComputeSubArray (KEY_TYPE minRand, KEY_TYPE  maxRand, KEY_TYPE seed, u64 numOfDataElements){
		//TODO: this implementation is only for int, complete it for other types
		srand (seed);
		dataArray=(KEY_TYPE*) calloc(numOfDataElements, sizeof(KEY_TYPE));
		assert(dataArray!=NULL);
		for (u64 i=0; i < numOfDataElements; i++){
			//randArray[i]= (VALU_T) (rand() % maxRand + minRand);
			dataArray[i]= (KEY_TYPE) (rand() % (maxRand - minRand) + minRand);
		}
		ERROR_RETURN_TYPE ret= partitionEquallyAmongAllComputeSubArray (dataArray,  numOfDataElements);
		//free(randArray);
		return ret;
	}


//	ERROR_RETURN_TYPE generateSequetiallyAndPartitionEquallyAmongAllComputeSubArray (VALUE_TYPE seed, LOCAL_ADDRESS_TYPE DataAddress, bool writeMetadat, LOCAL_ADDRESS_TYPE AddressOfTheStartAddress, LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress, LOCAL_ADDRESS_TYPE numOfDataElements ){
//		std::vector<VALUE_TYPE> dataArr;
//		dataArr.reserve(numOfDataElements);
//
//		// Generate data in the form: 1, 2, 2, 3, 3, 3, 4, 4, 4, 4, ...
//		u64 idx = 0;
//		for(u64 i = 1; i <= numOfDataElements; i++){
//			for(u64 j = 0; j < i; j++){
//				dataArr.push_back(i);
//				idx++;
//				if(idx == numOfDataElements){
//					break;
//				}
//			}
//			if(idx == numOfDataElements){
//				break;
//			}
//		}
//
//		//for(int i = 0; i < 40; i++){
//		//	std::cout << dataArr[i] << ", ";
//		//}
//		//std::cout << std::endl;
//
//		//Shuffle to randomize order. After sorting, we should get back 1, 2, 2, 3, 3, 3, 4, 4, 4, 4, ...
//		std::shuffle(dataArr.begin(), dataArr.end(), std::default_random_engine(seed));
//
//		//for(int i = 0; i < 40; i++){
//		//	std::cout << dataArr[i] << ", ";
//		//}
//		//std::cout << std::endl;
//
//		READ_DATA_TYPE_IN_MEMORY_ARRAY* dataToBePartitionedData = (READ_DATA_TYPE_IN_MEMORY_ARRAY*)(dataArr.data());
//		return partitionEquallyAmongAllComputeSubArray (DataAddress, writeMetadat, AddressOfTheStartAddress,  AddressOfTheEndAdddress,  dataToBePartitionedData,  numOfDataElements);
//	}


	LOCAL_ADDRESS_TYPE localAddressToLocalMetadata(LOCAL_ADDRESS_TYPE localAddress);
	LOCAL_ADDRESS_TYPE metadatatoLocalAddress(LOCAL_ADDRESS_TYPE localMetadata);
	//ERROR_RETURN_TYPE readData(LOCAL_ADDRESS_TYPE& DataAddress, bool readByMetadat, bool printData, LOCAL_ADDRESS_TYPE AddressOfTheStartAddress, LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress, READ_DATA_TYPE_IN_MEMORY_ARRAY** pointerToReadDataPointer, LOCAL_ADDRESS_TYPE& sizeOFData, ID_TYPE layerId, ID_TYPE bankID, ID_TYPE computeSubArrayID);
};



#endif /* DATAPARTITIONING_HPP_ */
