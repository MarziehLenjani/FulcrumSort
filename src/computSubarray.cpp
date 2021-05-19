/*
 * computSubarray.cpp
 *
 *  Created on: May 16, 2021
 *      Author: marzieh
 */
#include "computSubarray.hpp"


computSubarray::computSubarray(ID_TYPE l_id, configAndStats * l_confObj, physicalComponent * l_firstDimOwner, physicalComponent * l_secondDimOwner, physicalComponent * l_thirdDimOwner)
:physicalComponent(l_id, l_confObj, l_firstDimOwner, l_secondDimOwner, l_thirdDimOwner){
	//TODO: do initializations specific for the subarray class here
	memoryArrayObj= new memoryArray(0, l_confObj, this, NULL, NULL);

}

computSubarray::~computSubarray(){
	//TODO: make sure it calls the decosntuctor
	delete memoryArrayObj;

}
void computSubarray::runOneSubClokCycle(){
	//TODO: implement this function
}

