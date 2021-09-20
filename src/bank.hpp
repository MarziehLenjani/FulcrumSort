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

class bank:public physicalComponent{
public:
	std::vector<computSubarray *> computSubarrayVector;
	bank(ID_TYPE l_id, physicalComponent * l_parent);
	~bank();
	void runOneSubClokCycle();
};





#endif
