/*
 * bank.cpp
 *
 *  Created on: Feb 19, 2020
 *      Author: ml2au
 */
#include "bank.hpp"
bank::bank(ID_TYPE l_id, configAndStats * l_confObj, physicalComponent * l_firstDimOwner, physicalComponent * l_secondDimOwner, physicalComponent * l_thirdDimOwner)
:physicalComponent(l_id, l_confObj, l_firstDimOwner, l_secondDimOwner, l_thirdDimOwner){
	//TODO: do initializations specific for the layer class here
	for (ID_TYPE i=0;i<l_confObj->getConfig<CONF_NUMBER_OF_COMPUTE_SUBARRAY_PER_BANK_TYPE>(CONF_NUMBER_OF_COMPUTE_SUBARRAY_PER_BANK_NAME);i++){
		computSubarrayVector.push_back(new computSubarray(i, l_confObj, this, NULL,NULL) ); //TODO: assign right values for the second and third dimensions
	}
}

bank::~bank(){

	auto it = computSubarrayVector.begin();
	while (it != computSubarrayVector.end()) {
			// Remove elements while iterating
		delete (*it);
		it = computSubarrayVector.erase(it);

	}

}
void bank::runOneSubClokCycle(){
	for(computSubarray* ptr : computSubarrayVector){
		ptr->runOneSubClokCycle();

	}
}



