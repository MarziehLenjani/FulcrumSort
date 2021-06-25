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
#include "memoryArray.hpp"
#include "types.hpp"
#include <stdlib.h>
#include <iostream>
class dataPartitioning{
	public:
	stackedMemory * stackObj;
	configAndStats * confObj;
	dataPartitioning(stackedMemory * l_stackObj, configAndStats * l_confObj);
	ERROR_RETURN_TYPE globalAddressToLocalAddressTranslation(PHYSICAL_EXTERNAL_ADDRESS_TYPE address, ID_TYPE & stackId, ID_TYPE & layerId, ID_TYPE & bankId, ID_TYPE & computeSubarrayID);
	ERROR_RETURN_TYPE broadcastDataToAllComputeSubArray (LOCAL_ADDRESS_TYPE DataAddress, bool writeMetadat, LOCAL_ADDRESS_TYPE AddressOfTheStartAddress, LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress, READ_DATA_TYPE_IN_MEMORY_ARRAY* broadcastedData, LOCAL_ADDRESS_TYPE sizeOFData );
	ERROR_RETURN_TYPE partitionEquallyAmongAllComputeSubArray (LOCAL_ADDRESS_TYPE DataAddress, bool writeMetadat, LOCAL_ADDRESS_TYPE AddressOfTheStartAddress, LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress, READ_DATA_TYPE_IN_MEMORY_ARRAY* dataToBePartitionedData, LOCAL_ADDRESS_TYPE sizeOFData );
	//template functions should be implemented inside hpp files
	template<typename VALU_T, typename SEED_T>  ERROR_RETURN_TYPE generateRandomlyAndPartitionEquallyAmongAllComputeSubArray (VALU_T minRand, VALU_T  maxRand, SEED_T seed,  LOCAL_ADDRESS_TYPE DataAddress, bool writeMetadat, LOCAL_ADDRESS_TYPE AddressOfTheStartAddress, LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress, LOCAL_ADDRESS_TYPE numOFDataElements ){
		//TODO: this implementation is only for int, complete it for other types
		srand (seed);
		VALU_T* randArray=NULL;
		randArray=(VALU_T*) calloc(numOFDataElements, sizeof(VALU_T));
		ASSERT_EX(randArray!=NULL, std::cout<<"calloc returns ");
		assert(randArray!=NULL);
		for (LOCAL_ADDRESS_TYPE i=0; i<numOFDataElements ;i++){

			randArray[i]= (VALU_T) (rand() % maxRand + minRand);
		}
		READ_DATA_TYPE_IN_MEMORY_ARRAY* dataToBePartitionedData= (READ_DATA_TYPE_IN_MEMORY_ARRAY*)randArray;
		LOCAL_ADDRESS_TYPE sizeOFData=numOFDataElements* sizeof(VALU_T);
		ERROR_RETURN_TYPE ret= partitionEquallyAmongAllComputeSubArray (DataAddress, writeMetadat, AddressOfTheStartAddress,  AddressOfTheEndAdddress,  dataToBePartitionedData,  sizeOFData );
		free(randArray);
		return ret;
	}
	LOCAL_ADDRESS_TYPE localAddressToLocalMetadata(LOCAL_ADDRESS_TYPE localAddress);
	LOCAL_ADDRESS_TYPE metadatatoLocalAddress(LOCAL_ADDRESS_TYPE localMetadata);
	ERROR_RETURN_TYPE readData(LOCAL_ADDRESS_TYPE& DataAddress, bool readByMetadat, bool printData, LOCAL_ADDRESS_TYPE AddressOfTheStartAddress, LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress, READ_DATA_TYPE_IN_MEMORY_ARRAY** pointerToReadDataPointer, LOCAL_ADDRESS_TYPE& sizeOFData, ID_TYPE layerId, ID_TYPE bankID, ID_TYPE computeSubArrayID);
};



#endif /* DATAPARTITIONING_HPP_ */
