#pragma once

#include <cstdlib>
#include <vector>
#include <cassert>
#include "globals.hpp"
#include "Device.hpp"
#include "PhysicalComponent.hpp"
#include "Subarray.hpp"
#include "LogicLayer.hpp"

class PulleySystem : public PhysicalComponent {
public:
	std::vector<Device*> deviceVector;
	std::vector<Subarray*> subarrayVector;
	std::vector<LogicLayer*> logicLayerVector;

	PulleySystem(ID_TYPE l_id, PhysicalComponent* l_parent) : PhysicalComponent(l_id, l_parent) {

		assert(G_NUM_STACKS_PER_DEVICE <= 4);

		for(u64 i = 0; i < G_NUM_DEVICES; i++){
			deviceVector.push_back(new Device(i, this));
		}


		//assign global id's to subarrays
		//and also init layer Q pointers
		u64 sid = 0;
		for(u64 i = 0; i < G_NUM_DEVICES; i++){
			Device* device = deviceVector[i];
			device->initLinks();
			for(u64 j = 0; j < G_NUM_STACKS_PER_DEVICE; j++){
				Stack* stack = device->stackVector[j];
				logicLayerVector.push_back(stack->logicLayer);
				stack->logicLayer->initLinks();
				for(u64 k = 0; k < G_NUM_LAYERS_PER_STACK; k++){
					Layer* layer = stack->layerVector[k];
					for(u64 l = 0; l < G_NUM_BANKS_PER_LAYER; l++){
						Bank* bank = layer->bankVector[l];
						bank->initLayerQ();
						for(u64 m = 0; m < G_NUM_SUBARRAY_PER_BANK; m++){
							Subarray* sub = bank->subarrayVector[m];
							subarrayVector.push_back(sub);
							sub->SelfIndex = sid++;
						}
					}
				}
			}
		}

		assert(subarrayVector.size() == G_NUM_TOTAL_SUBARRAY);
		assert(logicLayerVector.size() == G_NUM_TOTAL_STACKS);
	}

	~PulleySystem(){
		for(Device* it : deviceVector){
			delete it;
		}
	}
};
