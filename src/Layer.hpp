/*
 * layer.hpp
 *
 *  Created on: Feb 12, 2020
 *      Author: ml2au
 */
#include <cstddef>
#ifndef LAYER_HPP_
#define LAYER_HPP_
#include "PhysicalComponent.hpp"
#include <vector>
class Bank;

class Layer:public PhysicalComponent{
public:
	std::vector<Bank *> bankVector;
	Layer(ID_TYPE l_id, PhysicalComponent * l_parent);
	~Layer();
};




#endif /* LAYER_HPP_ */
