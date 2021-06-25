/*
 * bank.hpp
 *
 *  Created on: Feb 12, 2020
 *      Author: ml2au
 */

#ifndef BANK_HPP_
#define BANK_HPP_
#include <cstddef>

#include "types.hpp"

#include "physicalComponent.hpp"
#include "computSubarray.hpp"
#include "configAndStats.hpp"


class bank:public physicalComponent{
public:
	std::vector<computSubarray *> computSubarrayVector;
	bank(ID_TYPE l_id, configAndStats * l_confObj, physicalComponent * l_firstDimOwner, physicalComponent * l_secondDimOwner, physicalComponent * l_thirdDimOwner);
	~bank();
	void runOneSubClokCycle();

	CONF_NUMBER_OF_COMPUTE_SUBARRAY_PER_BANK_TYPE nSubPerBank;


};





#endif
