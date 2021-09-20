/*
 * bank.cpp
 *
 *  Created on: Feb 19, 2020
 *      Author: ml2au
 */
#include "bank.hpp"
bank::bank(ID_TYPE l_id, physicalComponent * l_parent) : physicalComponent(l_id, l_parent){
	//TODO: do initializations specific for the layer class here
	for (ID_TYPE i=0; i < G_NUM_SUBARRAY_PER_BANK; i++){
		computSubarrayVector.push_back(new computSubarray(i, this) );
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
		//ptr->runOneSubClokCycle();

	}
}



