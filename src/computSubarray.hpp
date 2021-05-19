/*
 * computSubarray.hpp
 *
 *  Created on: May 16, 2021
 *      Author: marzieh
 */

#ifndef COMPUTSUBARRAY_HPP_
#define COMPUTSUBARRAY_HPP_
#include "physicalComponent.hpp"
#include "configAndStats.hpp"
#include "memoryArray.hpp"
class computSubarray:public physicalComponent{
public:
	computSubarray(ID_TYPE l_id, configAndStats * l_confObj, physicalComponent * l_firstDimOwner, physicalComponent * l_secondDimOwner, physicalComponent * l_thirdDimOwner);
	~computSubarray();
	void runOneSubClokCycle();
	memoryArray * memoryArrayObj;

};




#endif /* COMPUTSUBARRAY_HPP_ */
