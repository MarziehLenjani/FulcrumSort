/*
 * stackedMemory.cpp
 *
 *  Created on: May 16, 2021
 *      Author: marzieh
 */

#include "stackedMemory.hpp"


stackedMemory::stackedMemory(ID_TYPE l_id, configAndStats * l_confObj, physicalComponent * l_firstDimOwner, physicalComponent * l_secondDimOwner, physicalComponent * l_thirdDimOwner):
	physicalComponent(l_id, l_confObj, l_firstDimOwner, l_secondDimOwner, l_thirdDimOwner){
	//std::cout<<CONF_NUMBER_OF_LAYERS_NAME <<" is "<<l_confObj->getConfig<CONF_NUMBER_OF_LAYERS_TYPE>(CONF_NUMBER_OF_LAYERS_NAME)<<std::endl;
	for (ID_TYPE i=0;i<l_confObj->getConfig<CONF_NUMBER_OF_LAYERS_TYPE>(CONF_NUMBER_OF_LAYERS_NAME);i++){
		layerVector.push_back(new layer(i, l_confObj, this, NULL,NULL) ); //TODO: assign right values for the second and third dimensions
	}

	numLayers=l_confObj->getConfig<CONF_NUMBER_OF_LAYERS_TYPE>(CONF_NUMBER_OF_LAYERS_NAME);
	numBanksPerLayer=l_confObj->getConfig<CONF_NUMBER_OF_BANKS_PER_LAYER_TYPE>(CONF_NUMBER_OF_BANKS_PER_LAYER_NAME);
	numSubArraysPerBank=l_confObj->getConfig<CONF_NUMBER_OF_COMPUTE_SUBARRAY_PER_BANK_TYPE>(CONF_NUMBER_OF_COMPUTE_SUBARRAY_PER_BANK_NAME);
	totNumComputeSubarray=numLayers*numBanksPerLayer*numSubArraysPerBank;
	//generating a vector of compute subarrays for the stacked memory for easier acceess to all subarrays
	//we do not need to delete this in the deconstructor, because these are just pointers that will be deleted in their own classs
	for (ID_TYPE i=0;i<numLayers;i++){
		for (ID_TYPE j=0;j<numBanksPerLayer;j++){
			for (ID_TYPE k=0;k<numSubArraysPerBank;k++){
				computSubarrayVector.push_back(layerVector[i]->bankVector[j]->computSubarrayVector[k]); //TODO: assign right values for the second and third dimensions
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
	for(layer* ptr : layerVector){
		ptr->runOneSubClokCycle();

	}
}



