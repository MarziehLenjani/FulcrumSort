/*
 * layer.hpp
 *
 *  Created on: Feb 12, 2020
 *      Author: ml2au
 */
#include <cstddef>
#ifndef LAYER_HPP_
#define LAYER_HPP_
#include "physicalComponent.hpp"
#include <vector>
class bank;

class layer:public physicalComponent{
public:
	std::vector<bank *> bankVector;
	layer(ID_TYPE l_id, physicalComponent * l_firstDimOwner, physicalComponent * l_secondDimOwner, physicalComponent * l_thirdDimOwner);
	~layer();
	void runOneSubClokCycle();
};




#endif /* LAYER_HPP_ */
