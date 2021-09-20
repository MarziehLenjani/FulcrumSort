/*
 * stackedMemory.cpp
 *
 *  Created on: May 16, 2021
 *      Author: marzieh
 */

#include "stackedMemory.hpp"


stackedMemory::stackedMemory(ID_TYPE l_id, physicalComponent * l_parent): physicalComponent(l_id, l_parent){
	//std::cout<<CONF_NUMBER_OF_LAYERS_NAME <<" is "<<l_confObj->getConfig<CONF_NUMBER_OF_LAYERS_TYPE>(CONF_NUMBER_OF_LAYERS_NAME)<<std::endl;
	for (ID_TYPE i=0; i < G_NUM_LAYERS; i++){
		layerVector.push_back(new layer(i, this) ); //TODO: assign right values for the second and third dimensions
	}

	//generating a vector of compute subarrays for the stacked memory for easier acceess to all subarrays
	//we do not need to delete this in the deconstructor, because these are just pointers that will be deleted in their own classs
	for (ID_TYPE i=0; i < G_NUM_LAYERS; i++){
		for (ID_TYPE j=0; j < G_NUM_BANKS_PER_LAYER; j++){
			for (ID_TYPE k=0; k < G_NUM_SUBARRAY_PER_BANK; k++){
				computSubarray* sub = layerVector[i]->bankVector[j]->computSubarrayVector[k];
				computSubarrayVector.push_back(sub); //TODO: assign right values for the second and third dimensions
				sub->initNextSubarraySemiRing();
				//sub->initNextSubarrayCrossbar();
				//sub->initNextSubarrayDragonfly();
			}
		}
	}
}

stackedMemory::~stackedMemory(){
	//deleting all layers
	auto it = layerVector.begin();
	while (it != layerVector.end()) {
			// Remove elements while iterating
		delete (*it);
		it = layerVector.erase(it);

	}

}

void stackedMemory::runOneSubClokCycle(){
	for(computSubarray* subarray : computSubarrayVector){
		//subarray->runOneSubClokCycle();
	}
}


bool stackedMemory::checkIfProcessHasEnd(){
	//return true only if ALL subarray->readEnded is true and no in-flight data packets to process
//	for(computSubarray* subarray : computSubarrayVector){
//		if(!subarray->readEnded || subarray->incomingPackets.size()){
//			return false;
//		}
//	}
	return true;
}


bool stackedMemory::checkIfSubBlockLimitIsReached(){

//	for (ID_TYPE i=0;i<numLayers;i++){
//		for (ID_TYPE j=0;j<numBanksPerLayer;j++){
//			for (ID_TYPE k=0;k<numSubArraysPerBank;k++){
//				if(!layerVector[i]->bankVector[j]->computSubarrayVector[k]->subBlockLimitIsReached){
//
//					return true;
//
//				}
//
//			}
//		}
//	}
	return false;
}

void stackedMemory::initializeSubarraysSelfindexes(){
	FULCRU_WORD_TYPE tSelfIndex= (FULCRU_WORD_TYPE)0 ;
	for(computSubarray* subarray : computSubarrayVector){
		subarray->SelfIndex = tSelfIndex++;
	}
}

//void stackedMemory::incrementSubarraysSelfindexes(){
//	FULCRU_WORD_TYPE tSelfIndex=0;
//	for (ID_TYPE i=0;i<numLayers;i++){
//		for (ID_TYPE j=0;j<numBanksPerLayer;j++){
//			for (ID_TYPE k=0;k<numSubArraysPerBank;k++){
//				tSelfIndex=layerVector[i]->bankVector[j]->computSubarrayVector[k]->SelfIndex;
//				tSelfIndex++;
//				if(tSelfIndex>totNumComputeSubarray){
//					tSelfIndex=0;
//				}
//				layerVector[i]->bankVector[j]->computSubarrayVector[k]->SelfIndex=tSelfIndex;
//
//			}
//		}
//	}
//	return;
//}

void stackedMemory::sealAllSubBuckets(){
	for(computSubarray* subarray : computSubarrayVector){
		subarray->sealAllSubBuckets();
	}
}

void stackedMemory::openANewSubBucket(){
	for(computSubarray* subarray : computSubarrayVector){
		subarray->openANewSubBucket();
	}
}

void stackedMemory::setMaskForBucketIDExtraction(FULCRU_WORD_TYPE maskForBucketExtaction, FULCRU_WORD_TYPE numberOfShiftsForBucketIDExtraction){
	for(computSubarray* subarray : computSubarrayVector){
		subarray->setMaskForBucketIDExtraction(maskForBucketExtaction, numberOfShiftsForBucketIDExtraction);
	}
}

//void stackedMemory::initializeHistGenGlobal(){
//	for(computSubarray* subarray : computSubarrayVector){
//		subarray->initializeHistGenGlobal();
//	}
//}
//
//void stackedMemory::runHistGenGlobalOneClockCycle(){
//	for(computSubarray* subarray : computSubarrayVector){
//		subarray->runHistGenGlobalOneClockCycle();
//	}
//}
//
//void stackedMemory::initializePrefixSumWithinArrayGlobal(){
//	for(computSubarray* subarray : computSubarrayVector){
//		subarray->initializePrefixSumWithinArrayGlobal();
//	}
//}
//
//void stackedMemory::runPrefixSumWithinArrayGlobalOneClockCycle(){
//	for(computSubarray* subarray : computSubarrayVector){
//		subarray->runPrefixSumWithinArrayGlobalOneClockCycle();
//	}
//}
//
//void stackedMemory::initializePrefixSumNextArrayGlobal(){
//	for(computSubarray* subarray : computSubarrayVector){
//		subarray->initializePrefixSumNextArrayGlobal();
//	}
//}
//
//void stackedMemory::runPrefixSumNextArrayGlobalOneClockCycle(){
//	for(computSubarray* subarray : computSubarrayVector){
//		subarray->runPrefixSumNextArrayGlobalOneClockCycle();
//	}
//}
//
//void stackedMemory::initializePlacementGlobal(){
//	for(computSubarray* subarray : computSubarrayVector){
//		subarray->initializePlacementGlobal();
//	}
//}
//
//void stackedMemory::runPlacementGlobalOneClockCycle(){
//	for(computSubarray* subarray : computSubarrayVector){
//		subarray->runPlacementGlobalOneClockCycle();
//	}
//}



