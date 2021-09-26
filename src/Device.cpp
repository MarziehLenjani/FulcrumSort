
#include "NVLink.hpp"
#include "Device.hpp"
#include "PulleySystem.hpp"


Device::Device(ID_TYPE l_id, PhysicalComponent* l_parent) : PhysicalComponent(l_id, l_parent) {
	for(u64 i = 0; i < G_NUM_STACKS_PER_DEVICE; i++){
		stackVector.push_back(new Stack(i, this));
	}
	for(u64 i = 0; i < G_NUM_DEVICES; i++){
		nvLinks[i] = nullptr;
	}
}

Device::~Device(){
	for(Stack* it : stackVector){
		delete it;
	}

	for(u64 i = 0; i < G_NUM_DEVICES; i++){
		if(nvLinks[i] != nullptr){
			delete nvLinks[i];
			nvLinks[i] = nullptr;
		}
	}
}


void Device::initLinks(){
	PulleySystem* pulley = (PulleySystem*) parent;
	for(u64 i = 0; i < G_NUM_DEVICES; i++){
		if(i != id){
			nvLinks[i] = new NVLink(pulley->deviceVector[i]);
		}
	}
}

void Device::runLinkOneClock(){
	for(u64 i = 0; i < G_NUM_DEVICES; i++){
		if(i != id){
			nvLinks[i]->clock();
		}
	}
}
