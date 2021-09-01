/*
 * dataPartitioning.hpp
 *
 *  Created on: May 17, 2021
 *      Author: marzieh
 */

#ifndef DATAPARTITIONING_HPP_
#define DATAPARTITIONING_HPP_
#include "stackedMemory.hpp"
#include "types.hpp"
#include "statConfigNamesAndTypes.hpp"
#include "physicalComponent.hpp"
#include "statConfigNamesAndTypes.hpp"
#include <stdio.h>
#include <iostream>
#include "types.hpp"
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <random>

class dataPartitioning{
	public:
	stackedMemory * stackObj;
	dataPartitioning(stackedMemory * l_stackObj);
	ERROR_RETURN_TYPE globalAddressToLocalAddressTranslation(PHYSICAL_EXTERNAL_ADDRESS_TYPE address, ID_TYPE & stackId, ID_TYPE & layerId, ID_TYPE & bankId, ID_TYPE & computeSubarrayID);
	ERROR_RETURN_TYPE broadcastDataToAllComputeSubArray (LOCAL_ADDRESS_TYPE DataAddress, bool writeMetadat, LOCAL_ADDRESS_TYPE AddressOfTheStartAddress, LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress, READ_DATA_TYPE_IN_MEMORY_ARRAY* broadcastedData, LOCAL_ADDRESS_TYPE sizeOFData );
	ERROR_RETURN_TYPE partitionEquallyAmongAllComputeSubArray (LOCAL_ADDRESS_TYPE DataAddress, bool writeMetadat, LOCAL_ADDRESS_TYPE AddressOfTheStartAddress, LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress, READ_DATA_TYPE_IN_MEMORY_ARRAY* dataToBePartitionedData, LOCAL_ADDRESS_TYPE sizeOFData );
	//template functions should be implemented inside hpp files

	ERROR_RETURN_TYPE generateRandomlyAndPartitionEquallyAmongAllComputeSubArray (KEY_TYPE minRand, KEY_TYPE  maxRand, KEY_TYPE seed,  LOCAL_ADDRESS_TYPE DataAddress, bool writeMetadat, LOCAL_ADDRESS_TYPE AddressOfTheStartAddress, LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress, LOCAL_ADDRESS_TYPE numOfDataElements ){
		//TODO: this implementation is only for int, complete it for other types
		srand (seed);
		dataArray=(KEY_TYPE*) calloc(numOfDataElements, sizeof(KEY_TYPE));
		assert(dataArray!=NULL);
		for (LOCAL_ADDRESS_TYPE i=0; i<numOfDataElements ;i++){

			//randArray[i]= (VALU_T) (rand() % maxRand + minRand);
			dataArray[i]= (KEY_TYPE) (rand() % (maxRand - minRand) + minRand);
		}
		READ_DATA_TYPE_IN_MEMORY_ARRAY* dataToBePartitionedData= (READ_DATA_TYPE_IN_MEMORY_ARRAY*)dataArray;
		ERROR_RETURN_TYPE ret= partitionEquallyAmongAllComputeSubArray (DataAddress, writeMetadat, AddressOfTheStartAddress,  AddressOfTheEndAdddress,  dataToBePartitionedData,  numOfDataElements);
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
	ERROR_RETURN_TYPE readData(LOCAL_ADDRESS_TYPE& DataAddress, bool readByMetadat, bool printData, LOCAL_ADDRESS_TYPE AddressOfTheStartAddress, LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress, READ_DATA_TYPE_IN_MEMORY_ARRAY** pointerToReadDataPointer, LOCAL_ADDRESS_TYPE& sizeOFData, ID_TYPE layerId, ID_TYPE bankID, ID_TYPE computeSubArrayID);
};



#endif /* DATAPARTITIONING_HPP_ */
