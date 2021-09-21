/*
 * bank.cpp
 *
 *  Created on: Feb 19, 2020
 *      Author: ml2au
 */

#include "Stack.hpp"
#include "Bank.hpp"
#include "Layer.hpp"
#include "LogicLayer.hpp"

Bank::Bank(ID_TYPE l_id, PhysicalComponent * l_parent) : PhysicalComponent(l_id, l_parent){
	//TODO: do initializations specific for the layer class here
	for (ID_TYPE i=0; i < G_NUM_SUBARRAY_PER_BANK; i++){
		subarrayVector.push_back(new Subarray(i, this));
	}
}

Bank::~Bank(){
	for(auto it : subarrayVector){
		delete it;
	}
}

void Bank::initLayerQ(){
	Layer* layer = (Layer*)parent;
	Stack* stack = (Stack*)layer->parent;
	u64 currLayer = layer->id;

	if(currLayer == 0){
		//lowerLayerQ = logicLayerQ
		lowerLayerQ = &stack->logicLayer->packetQs[id];
	}
	else{
		lowerLayerQ = &stack->layerVector[currLayer-1]->bankVector[id]->subarrayVector[0]->incomingPackets;
	}

	if(currLayer != (G_NUM_LAYERS_PER_STACK - 1)){
		upperLayerQ = &stack->layerVector[currLayer+1]->bankVector[id]->subarrayVector[0]->incomingPackets;
	}
}
