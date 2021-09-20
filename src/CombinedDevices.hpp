#pragma once

#include <cstdlib>
#include <vector>
#include "globals.hpp"
#include "Device.hpp"
#include "physicalComponent.hpp"

class CombinedDevices : public physicalComponent {
public:
	std::vector<Device*> deviceVector;

	CombinedDevices(ID_TYPE l_id, physicalComponent* l_parent) : physicalComponent(l_id, l_parent) {
		for(u64 i = 0; i < G_NUM_DEVICES; i++){
			deviceVector.push_back(new Device(i, this));
		}
	}

	~CombinedDevices(){
		for(Device* it : deviceVector){
			delete it;
		}
	}
};
