/*
 * layer.cpp
 *
 *  Created on: Feb 14, 2020
 *      Author: ml2au
 */

#include "layer.hpp"
#include "bank.hpp"
#include "layer.hpp"
#include <vector>


layer::layer(ID_TYPE l_id, physicalComponent * l_firstDimOwner, physicalComponent * l_secondDimOwner, physicalComponent * l_thirdDimOwner)
:physicalComponent(l_id, l_firstDimOwner, l_secondDimOwner, l_thirdDimOwner){
	//TODO: do initializations specific for the layer class here
	for (ID_TYPE i=0;i < G_NUM_BANKS_PER_LAYER; i++){
		bankVector.push_back(new bank(i, this, NULL,NULL) ); //TODO: assign right values for the second and third dimensions
	}
}

layer::~layer(){
	auto it = bankVector.begin();
	while (it != bankVector.end()) {
			// Remove elements while iterating
		delete (*it);
		it = bankVector.erase(it);

	}

}
void layer::runOneSubClokCycle(){
	for(bank* ptr : bankVector){
		ptr->runOneSubClokCycle();

	}
}


