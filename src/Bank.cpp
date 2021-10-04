/*
 * bank.cpp
 *
 *  Created on: Feb 19, 2020
 *      Author: ml2au
 */

#include "types.hpp"
#include "globals.hpp"
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

//A bit of approximation here to make the simulation much faster!
void Bank::producePackets(){
	for(i64 i = G_NUM_SUBARRAY_PER_BANK - 1; i >= 0; i--){
		subarrayVector[i]->prePlacementProducePackets(packetQ, histogram);
	}
}

void Bank::routePacketsOneCycle(){
	if(!packetQ.empty()){
		Packet<PlacementPacket>* packet = packetQ.front();
		if(__builtin_expect(packet->dstBankAddr == selfIndex, 0)){
			if(stalledSub == 0){
				packetQ.pop();
				//in the correct bank, write to the destination subarray
				subarrayVector[packet->dstSubId]->appendPacket(packet->payload);

				//done with this packet
				//placementPacketAllocator->free(packet); //Modified: no need to free any longer

				//#pragma omp atomic
				//numOfInFlightPackets--;
			}
		}
		else{
			packetQ.pop();
			//forward packet
			//hopCounter++;
			//auto nextSubArrayQ = getNextComputeSubArrayQIdeal(tmpPacket->dstSubAddr);
			//auto nextSubArrayQ = getNextComputeSubArrayQSemiRing(tmpPacket->dstSubAddr);
			//auto nextSubArrayQ = getNextComputeSubArrayQCrossbar(tmpPacket->dstSubAddr);
			//auto nextSubArrayQ = getNextComputeSubArrayQDragonfly(tmpPacket->dstSubAddr);
			auto nextSubArrayQ = getNextBankQ_dragonfly(packet->dstBankAddr);
			nextSubArrayQ->push(packet);
		}
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
		lowerLayerQ = &stack->layerVector[currLayer-1]->bankVector[id]->packetQ;
	}

	if(currLayer != (G_NUM_LAYERS_PER_STACK - 1)){
		upperLayerQ = &stack->layerVector[currLayer+1]->bankVector[id]->packetQ;
	}
}

std::queue <Packet<PlacementPacket>*>* Bank::getNextBankQ_dragonfly(ID_TYPE dstBankAddr){
	if(extractUptoStack(dstBankAddr) == extractUptoStack(selfIndex)){
		//correct stack
		ID_TYPE dstBankId = extractBankId(dstBankAddr);
		if(id != dstBankId){
			//incorrect bank, go towards correct bank using dragonfly topology
			numBankToBankPackets++;
			u64 nextBank = dragonNextDst[id][dstBankId];
			Layer* layer = (Layer*)parent;
			return &(layer->bankVector[nextBank]->packetQ);
		}
		else{
			//correct bank, go towards correct layer
			numSegTSVPackets++;
			u64 currLayer = extractLayerId(selfIndex);
			u64 destLayer = extractLayerId(dstBankAddr);

			if(currLayer < destLayer){
				//go up
				return upperLayerQ;
			}
			else if(currLayer > destLayer){
				//go down
				return lowerLayerQ;
			}
		}
	}
	else{
		//incorrect stack, go towards logic layer
		numSegTSVPackets++;
		return lowerLayerQ;
	}

	return nullptr;
}

u64 Bank::runLocalHist(){
	memset(histogram, 0, G_NUM_HIST_ELEMS * sizeof(HIST_ELEM_TYPE));
	//u64 maxProcessedElems = 0;
	u64 totalProcessedElems = 0;
	for(Subarray* sub : subarrayVector){
		totalProcessedElems += sub->runLocalHist(histogram);
//		if(subProcElems > maxProcessedElems){
//			maxProcessedElems = subProcElems;
//		}
	}
	return totalProcessedElems;
}


