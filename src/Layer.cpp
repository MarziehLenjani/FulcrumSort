/*
 * layer.cpp
 *
 *  Created on: Feb 14, 2020
 *      Author: ml2au
 */

#include "globals.hpp"
#include "Layer.hpp"
#include "Bank.hpp"
#include <vector>


Layer::Layer(ID_TYPE l_id, PhysicalComponent * l_parent)
:PhysicalComponent(l_id, l_parent){
	//TODO: do initializations specific for the layer class here
	for (ID_TYPE i=0;i < G_NUM_BANKS_PER_LAYER; i++){
		bankVector.push_back(new Bank(i, this) ); //TODO: assign right values for the second and third dimensions
	}
}

Layer::~Layer(){
	for(auto it : bankVector){
		delete it;
	}
}



