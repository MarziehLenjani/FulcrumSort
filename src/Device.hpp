#pragma once

#include <cstdlib>
#include <vector>
#include "globals.hpp"
#include "stackedMemory.hpp"
#include "physicalComponent.hpp"

class Device : public physicalComponent {
public:
	std::vector<stackedMemory*> stackVector;

	Device(ID_TYPE l_id, physicalComponent* l_parent) : physicalComponent(l_id, l_parent) {
		for(u64 i = 0; i < G_NUM_STACKS; i++){
			stackVector.push_back(new stackedMemory(i, this));
		}
	}

	~Device(){
		for(stackedMemory* it : stackVector){
			delete it;
		}
	}
};
