/*
 * stackedMemory.cpp
 *
 *  Created on: May 16, 2021
 *      Author: marzieh
 */

#include "Stack.hpp"
#include "LogicLayer.hpp"


Stack::Stack(ID_TYPE l_id, PhysicalComponent * l_parent): PhysicalComponent(l_id, l_parent){
	//std::cout<<CONF_NUMBER_OF_LAYERS_NAME <<" is "<<l_confObj->getConfig<CONF_NUMBER_OF_LAYERS_TYPE>(CONF_NUMBER_OF_LAYERS_NAME)<<std::endl;
	for (ID_TYPE i=0; i < G_NUM_LAYERS_PER_STACK; i++){
		layerVector.push_back(new Layer(i, this) ); //TODO: assign right values for the second and third dimensions
	}

	logicLayer = new LogicLayer(this);

	//generating a vector of compute subarrays for the stacked memory for easier acceess to all subarrays
	//we do not need to delete this in the deconstructor, because these are just pointers that will be deleted in their own classs
//	for (ID_TYPE i=0; i < G_NUM_LAYERS_PER_STACK; i++){
//		for (ID_TYPE j=0; j < G_NUM_BANKS_PER_LAYER; j++){
//			for (ID_TYPE k=0; k < G_NUM_SUBARRAY_PER_BANK; k++){
//				Subarray* sub = layerVector[i]->bankVector[j]->subarrayVector[k];
//				computSubarrayVector.push_back(sub); //TODO: assign right values for the second and third dimensions
//				sub->initNextSubarraySemiRing();
//				//sub->initNextSubarrayCrossbar();
//				//sub->initNextSubarrayDragonfly();
//			}
//		}
//	}
}

Stack::~Stack(){
	for(auto it : layerVector){
		delete it;
	}
	delete logicLayer;
}

//void Stack::runOneSubClokCycle(){
//	for(Subarray* subarray : computSubarrayVector){
//		//subarray->runOneSubClokCycle();
//	}
//}


//bool Stack::checkIfProcessHasEnd(){
	//return true only if ALL subarray->readEnded is true and no in-flight data packets to process
//	for(computSubarray* subarray : computSubarrayVector){
//		if(!subarray->readEnded || subarray->incomingPackets.size()){
//			return false;
//		}
//	}
//	return true;
//}


//bool Stack::checkIfSubBlockLimitIsReached(){

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
//	return false;
//}

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

//void Stack::sealAllSubBuckets(){
//	for(Subarray* subarray : computSubarrayVector){
//		subarray->sealAllSubBuckets();
//	}
//}
//
//void Stack::openANewSubBucket(){
//	for(Subarray* subarray : computSubarrayVector){
//		subarray->openANewSubBucket();
//	}
//}
//
//void Stack::setMaskForBucketIDExtraction(FULCRU_WORD_TYPE maskForBucketExtaction, FULCRU_WORD_TYPE numberOfShiftsForBucketIDExtraction){
//	for(Subarray* subarray : computSubarrayVector){
//		subarray->setMaskForBucketIDExtraction(maskForBucketExtaction, numberOfShiftsForBucketIDExtraction);
//	}
//}

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



