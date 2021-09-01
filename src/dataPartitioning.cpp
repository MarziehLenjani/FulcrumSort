/*
 * dataPartitioning.cpp
 *
 *  Created on: May 17, 2021
 *      Author: marzieh
 */
#include "dataPartitioning.hpp"
#include "stackedMemory.hpp"
#include "MemoryObject.hpp"
#include  <boost/utility/binary.hpp>

dataPartitioning::dataPartitioning(stackedMemory * l_stackObj){
	stackObj=l_stackObj;
	l_stackObj->dataPartitioningObj=this;
}

//ERROR_RETURN_TYPE dataPartitioning::globalAddressToLocalAddressTranslation(PHYSICAL_EXTERNAL_ADDRESS_TYPE address, ID_TYPE & stackId, ID_TYPE & layerId, ID_TYPE & bankId, ID_TYPE & computeSubarrayID){
//	//TODO: complete this for all possible patterns
//	//TODO: make it more general
//	mapStringToMappingEnum tmap;
//	ERROR_RETURN_TYPE err =PP_SUCCESS;
//	std::string addresMappingType=confObj->getConfig<CONF_ADDRESS_MAPPING_TYPE>(CONF_ADDRESS_MAPPING_NAME);
//	switch  (tmap.xmap[addresMappingType]){
//	//TODO: this is hard coded , adjust it based on the size of each memory arrays, number of subarrays in a bank, number of layers, etc
////use set bit boost library APIS
////https://stackoverflow.com/questions/2611764/can-i-use-a-binary-literal-in-c-or-c
////https://www.boost.org/doc/libs/1_42_0/libs/utility/utility.htm#BOOST_BINARY
////TODO by using BOOST_BINARY_U we force the address to be unsiged, fix that to support other adddress types
////TODO: to support 8 GB fulcrum we need 33 bits, so the extrenal address should be 64 bit format or we should use byte address bits
//#ifdef PHYSICAL_EXTERNAL_ADDRESS_TYPE_UNSIGHED_LONG_INT
//		case MLBS:
//
//
//			stackId= (ID_TYPE)0;
//			layerId=           (address & BOOST_BINARY_UL(1110 0000 0000 0000 0000 0000 0000 0000 0))>>30; // 8 layers= 2^3=> 3 bits => address[32:30]
//			bankId=            (address & BOOST_BINARY_UL(0001 1111 1000 0000 0000 0000 0000 0000 0))>>24;   // 64 banks per layer=> 2^6=> 6 bits address[29:24]
//			computeSubarrayID= (address & BOOST_BINARY_UL(0000 0000 0111 1000 0000 0000 0000 0000 0))>>20; // 16 compute subarrays per bank => 4  bits => address[23:20]
//#else
//#ifdef PHYSICAL_EXTERNAL_ADDRESS_TYPE_UNSIGHED_INT
//		case MLBS:
//
//			//TODO: this is hard coded , adjust it based on the size of each memory arrays, number of subarrays in a bank, number of layers, etc
//			//use set bit boost library APIS
//			//https://stackoverflow.com/questions/2611764/can-i-use-a-binary-literal-in-c-or-c
//			//https://www.boost.org/doc/libs/1_42_0/libs/utility/utility.htm#BOOST_BINARY
//			//TODO by using BOOST_BINARY_U we force the address to be unsiged, fix that to support other adddress types
//			//TODO: to support 8 GB fulcrum we need 33 bits, so the extrenal address should be 64 bit format
//			stackId= (ID_TYPE)0;
//			layerId=           (address & BOOST_BINARY_U(1110 0000 0000 0000 0000 0000 0000 0000 ))>>29; // 8 layers= 2^3=> 3 bits => address[31:29]
//			bankId=            (address & BOOST_BINARY_U(0001 1111 1000 0000 0000 0000 0000 0000 ))>>23;   // 64 banks per layer=> 2^6=> 6 bits address[28:23]
//			computeSubarrayID= (address & BOOST_BINARY_U(0000 0000 0111 1000 0000 0000 0000 0000 ))>>19; // 16 compute subarrays per bank => 4  bits => address[22:19]
//#endif
//#endif
//			 break;
//			 /*
//		case MVLBS:
//			//TODO: implement this
//			 *
//			 * break;
//			 */
//
//		default:
//			 std::cout<<__FILE__<<":"<<__LINE__<<":Assert message:"<<"the address mapping is not implemented yet"<<std::endl;
//		     assert(false);
//
//			break;
//
//
//	}
//	return err; //TODO: return error for invalid addresses that can not be translated
//}

// when we broadcast data we have three steps: 1. writing the data itself, wrinting the start and the the end address into the local subarrays in needed
ERROR_RETURN_TYPE  dataPartitioning::broadcastDataToAllComputeSubArray (LOCAL_ADDRESS_TYPE DataAddress, bool writeMetadat, LOCAL_ADDRESS_TYPE AddressOfTheStartAddress, LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress, READ_DATA_TYPE_IN_MEMORY_ARRAY * broadcastedData, LOCAL_ADDRESS_TYPE sizeOFData ){
	ERROR_RETURN_TYPE ret;
	for(computSubarray* ptr : stackObj->computSubarrayVector){
		ptr->memoryArrayObj->write(DataAddress, sizeOFData, broadcastedData);
		if(writeMetadat){
			LOCAL_ADDRESS_TYPE startAaddress=localAddressToLocalMetadata(DataAddress);
			ptr->memoryArrayObj->write(AddressOfTheStartAddress, sizeof(LOCAL_ADDRESS_TYPE), (READ_DATA_TYPE_IN_MEMORY_ARRAY*) (& startAaddress));
			LOCAL_ADDRESS_TYPE endAaddress=localAddressToLocalMetadata(DataAddress+sizeOFData);
			ptr->memoryArrayObj->write(AddressOfTheEndAdddress, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) (& endAaddress ));
		}
	}
	return ret;
}
// when we partition data we have three steps: 1. deivinding data , wrinting the start and the the end address into the local subarrays in needed
ERROR_RETURN_TYPE  dataPartitioning::partitionEquallyAmongAllComputeSubArray (LOCAL_ADDRESS_TYPE DataAddress, bool writeMetadat, LOCAL_ADDRESS_TYPE AddressOfTheStartAddress, LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress, READ_DATA_TYPE_IN_MEMORY_ARRAY * dataToBePartitionedData, LOCAL_ADDRESS_TYPE numOfDataElements ){

	ERROR_RETURN_TYPE ret;
	LOCAL_ADDRESS_TYPE i=0;
	LOCAL_ADDRESS_TYPE eachPartition = numOfDataElements / G_NUM_TOTAL_SUBARRAY;
	LOCAL_ADDRESS_TYPE remainingElems =  numOfDataElements - eachPartition * G_NUM_TOTAL_SUBARRAY;
	assert(remainingElems < G_NUM_TOTAL_SUBARRAY);

	u64 totElemDistributed = 0;	//just for checking if all data elements are distributed
	for(computSubarray* ptr : stackObj->computSubarrayVector){
		LOCAL_ADDRESS_TYPE numDataElementsInSubarray = eachPartition;
		if(remainingElems){
			numDataElementsInSubarray++;
			remainingElems--;
		}

		ptr->memoryArrayObj->write(DataAddress, numDataElementsInSubarray * sizeof(KEY_TYPE), dataToBePartitionedData + totElemDistributed * sizeof(KEY_TYPE));
		totElemDistributed += numDataElementsInSubarray;
		if(writeMetadat){
			//LOCAL_ADDRESS_TYPE startAaddress=localAddressToLocalMetadata(DataAddress);
			//ptr->memoryArrayObj->write(AddressOfTheStartAddress, sizeof(LOCAL_ADDRESS_TYPE), (READ_DATA_TYPE_IN_MEMORY_ARRAY*) (& startAaddress));
			//LOCAL_ADDRESS_TYPE endAaddress=localAddressToLocalMetadata(DataAddress + numDataElementsInSubarray * sizeof(KEY_TYPE));
			//ptr->memoryArrayObj->write(AddressOfTheEndAdddress, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) (& endAaddress ));
			//ptr->memoryArrayObj->write32(AddressOfTheStartAddress, DataAddress);
			ptr->memoryArrayObj->write32(AddressOfTheEndAdddress, DataAddress + numDataElementsInSubarray * sizeof(KEY_TYPE));
		}
		i++;
	}
	assert(totElemDistributed == numOfDataElements);
	return ret;
}

//TODO: change this function if other sort of metadata is required
LOCAL_ADDRESS_TYPE dataPartitioning::localAddressToLocalMetadata(LOCAL_ADDRESS_TYPE localAddress){
	return (localAddress/4);
}
LOCAL_ADDRESS_TYPE dataPartitioning::metadatatoLocalAddress(LOCAL_ADDRESS_TYPE localMetadata){
	return (localMetadata*4);
}
//TODO: test this function
ERROR_RETURN_TYPE dataPartitioning::readData(LOCAL_ADDRESS_TYPE& DataAddress, bool readByMetadat, bool printData, LOCAL_ADDRESS_TYPE AddressOfTheStartAddress, LOCAL_ADDRESS_TYPE AddressOfTheEndAdddress, READ_DATA_TYPE_IN_MEMORY_ARRAY** pointerToReadDataPointer, LOCAL_ADDRESS_TYPE& sizeOFData, ID_TYPE layerId, ID_TYPE bankID, ID_TYPE computeSubArrayID ){
	ERROR_RETURN_TYPE ret;
	if(readByMetadat){ //we should assign value to size by reading metadata

		LOCAL_ADDRESS_TYPE startMetadatAddress;

		stackObj->layerVector[layerId]->bankVector[bankID]->computSubarrayVector[computeSubArrayID]->memoryArrayObj->read(AddressOfTheStartAddress, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(startMetadatAddress));
		DataAddress=metadatatoLocalAddress (startMetadatAddress);
		LOCAL_ADDRESS_TYPE endMetadatAddress;
		stackObj->layerVector[layerId]->bankVector[bankID]->computSubarrayVector[computeSubArrayID]->memoryArrayObj->read(AddressOfTheEndAdddress, sizeof(LOCAL_ADDRESS_TYPE),(READ_DATA_TYPE_IN_MEMORY_ARRAY*) &(endMetadatAddress));
		sizeOFData=metadatatoLocalAddress (endMetadatAddress-startMetadatAddress);
		(*pointerToReadDataPointer)= (READ_DATA_TYPE_IN_MEMORY_ARRAY*) calloc(sizeOFData, sizeof(READ_DATA_TYPE_IN_MEMORY_ARRAY));

	}else{ //read by size, size is a read value here
			}
	stackObj->layerVector[layerId]->bankVector[bankID]->computSubarrayVector[computeSubArrayID]->memoryArrayObj->read(DataAddress, sizeOFData, (*pointerToReadDataPointer));


	if(printData){
		std::cout<<"_________printing memory array conents in addresss:" <<DataAddress<<std::endl;
       for(LOCAL_ADDRESS_TYPE increment=0; increment<sizeOFData; increment++) {
			READ_DATA_TYPE_IN_MEMORY_ARRAY tRead=*(*pointerToReadDataPointer + increment);

			printf("%d , ", tRead);

		}
		std::cout<<std::endl;

	}
	return ret;
}




